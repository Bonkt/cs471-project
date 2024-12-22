/*
 * Copyright (C) 2021, Alexandre Iooss <erdnaxe@crans.org>
 *
 * Log instruction execution with memory access.
 *
 * License: GNU GPL, version 2 or later.
 *   See the COPYING file in the top-level directory.
 */
#include <glib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <qemu-plugin.h>

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

/* Store last executed instruction on each vCPU as a GString */
static GPtrArray *last_exec;
static GMutex expand_array_lock;

static GPtrArray *imatches;
static GArray *amatches;

// =======================================================================
// trex code:
// =======================================================================

#define TREX_ENABLE_NATIVE_EXECLOG_TRACE 0

#define TREX_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct trex_insn_metadata_t {
    uint8_t direct_branch : 1;
    uint8_t indirect_branch : 1;
    uint8_t other : 6; 
} trex_insn_metadata_t;

typedef struct trex_insn_t {
    uint64_t vaddr;
    trex_insn_metadata_t metadata;
} trex_insn_t;

// New: Used for binary loging output. 
static FILE *trex_binary_ins_log_file;

static uint64_t trex_instructions_simulated = 0;


// indirect branch instructions
static const char* trex_indirect_branch_insns[] = {
    "ret ",
    "br ",
    "blr ", 
    "eret " 
};

// direct branch instructions:
static const char* trex_direct_branch_insns[] = {
    "b ",
    "b.ne ",	
    "b.eq ",
    "b.cs ",	
    "b.hs ",	
    "b.cc ",	
    "b.lo ",	
    "b.mi ",	
    "b.pl ",	
    "b.vs ",	
    "b.vc ",	
    "b.hi ",	
    "b.ls ",	
    "b.ge ",	
    "b.lt ",	
    "b.gt ",	
    "b.le ",
    "bx ",  // might not be used by data_caching
    "bl ",
    "blx ",
    "cbz ",  
    "cbnz ",
    "tbz ",
    "tbnz ",

};

static bool trex_strcmp_delimiter(const char *str_a, const char *str_b, char delim) {
    const char* str_a_copy = str_a;
    const char* str_b_copy = str_b;

    while (*str_a_copy != delim && *str_b_copy != delim) {
        if (*str_a_copy != *str_b_copy) {
            return false;
        }
        str_a_copy++;
        str_b_copy++;
    }

    // If both iterators reached delim, means they were equal
    if (*str_a_copy == delim && *str_b_copy == delim) {
        return true;
    }
    return false;
}

static bool trex_parse_insn_to_metadata(char *insn_disas, trex_insn_metadata_t* metadata) {

    // Special case for "b." instructions:
    // if (insn_disas[0] == 'b' && insn_disas[1] == '.') {
    //     metadata->direct_branch = true;
    //     return false;
    // }


    // indirect:
    for (int32_t i = 0; i < TREX_ARRAY_SIZE(trex_indirect_branch_insns); i++) {
        if(trex_strcmp_delimiter(trex_indirect_branch_insns[i], insn_disas, ' ')) {
            metadata->indirect_branch = true;
            return false;
        }
    }

    // direct:
    for (int32_t i = 0; i < TREX_ARRAY_SIZE(trex_direct_branch_insns); i++) {
        if(trex_strcmp_delimiter(trex_direct_branch_insns[i], insn_disas, ' ')) {
            metadata->direct_branch = true;
            return false;
        }
    }
    return true;
}

// =======================================================================



/*
 * Expand last_exec array.
 *
 * As we could have multiple threads trying to do this we need to
 * serialise the expansion under a lock. Threads accessing already
 * created entries can continue without issue even if the ptr array
 * gets reallocated during resize.
 */
static void expand_last_exec(int cpu_index)
{
    g_mutex_lock(&expand_array_lock);
    while (cpu_index >= last_exec->len) {
        GString *s = g_string_new(NULL);
        g_ptr_array_add(last_exec, s);
    }
    g_mutex_unlock(&expand_array_lock);
}

/**
 * Add memory read or write information to current instruction log
 */
static void vcpu_mem(unsigned int cpu_index, qemu_plugin_meminfo_t info,
                     uint64_t vaddr, void *udata)
{
// #if TREX_ENABLE_NATIVE_EXECLOG_TRACE
    GString *s;

    /* Find vCPU in array */
    g_assert(cpu_index < last_exec->len);
    s = g_ptr_array_index(last_exec, cpu_index);
    


    /* Indicate type of memory access */
    // if (qemu_plugin_mem_is_store(info)) {
    //     g_string_append(s, ", store");
    // } else {
    //     g_string_append(s, ", load");
    // }

    /* If full system emulation log physical address and device name */
    // struct qemu_plugin_hwaddr *hwaddr = qemu_plugin_get_hwaddr(info, vaddr);
    // if (hwaddr) {
    //     uint64_t addr = qemu_plugin_hwaddr_phys_addr(hwaddr);
    //     const char *name = qemu_plugin_hwaddr_device_name(hwaddr);
    //     g_string_append_printf(s, ", 0x%08"PRIx64", %s", addr, name);
    // } else {
    //     g_string_append_printf(s, ", 0x%08"PRIx64, vaddr);
    // }
// #endif
}


/**
 * On plugin exit, print last instruction in cache
 */
static void plugin_exit(qemu_plugin_id_t id, void *p)
{
    guint i;
    GString *s;
    for (i = 0; i < last_exec->len; i++) {
        s = g_ptr_array_index(last_exec, i);
        if (s->str) {
            qemu_plugin_outs(s->str);
            qemu_plugin_outs("\n");
        }
    }
}


/**
 * Log instruction execution
 */
static void trex_insn_exec_cb(unsigned int cpu_index, void *udata)
{
    // extract the trex_insn of the current instruction.
    trex_insn_t* trex_insn = (trex_insn_t*)udata;

    // two billions, then exit.
    if (trex_instructions_simulated >= 2000000000) {
        // Close file, and terminate process
        fclose(trex_binary_ins_log_file);
        qemu_plugin_id_t temp = {0};
        plugin_exit(temp, NULL);
        exit(0);
    }

    // only count and log instructions from the server cpu:
    if (cpu_index == 1) {
        trex_instructions_simulated++;
        if (trex_instructions_simulated % 10000000 == 0) {
            printf("# of trex_instructions_simulated: %lu \n", trex_instructions_simulated);
            // printf("vaddr: 0x%016" PRIx64 ", metadata: 0x%02X\n", trex_insn->vaddr, trex_insn->metadata);
        }

        fwrite(&trex_insn->vaddr, sizeof(trex_insn->vaddr), 1, trex_binary_ins_log_file);
        fwrite(&trex_insn->metadata, sizeof(trex_insn->metadata), 1, trex_binary_ins_log_file);
    }
}



/**
 * On translation block new translation
 *
 * QEMU convert code by translation block (TB). By hooking here we can then hook
 * a callback on each instruction and memory access.
 */
static void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
    struct qemu_plugin_insn *insn;
    bool skip = (imatches || amatches);

    size_t n = qemu_plugin_tb_n_insns(tb);
    for (size_t i = 0; i < n; i++) {
        char *insn_disas;
        uint64_t insn_vaddr;

        /*
         * `insn` is shared between translations in QEMU, copy needed data here.
         * `output` is never freed as it might be used multiple times during
         * the emulation lifetime.
         * We only consider the first 32 bits of the instruction, this may be
         * a limitation for CISC architectures.
         */
        insn = qemu_plugin_tb_get_insn(tb, i);
        insn_disas = qemu_plugin_insn_disas(insn);
        insn_vaddr = qemu_plugin_insn_vaddr(insn);

        /*
         * If we are filtering we better check out if we have any
         * hits. The skip "latches" so we can track memory accesses
         * after the instruction we care about.
         */
        if (skip && imatches) {
            int j;
            for (j = 0; j < imatches->len && skip; j++) {
                char *m = g_ptr_array_index(imatches, j);
                if (g_str_has_prefix(insn_disas, m)) {
                    skip = false;
                }
            }
        }

        if (skip && amatches) {
            int j;
            for (j = 0; j < amatches->len && skip; j++) {
                uint64_t v = g_array_index(amatches, uint64_t, j);
                if (v == insn_vaddr) {
                    skip = false;
                }
            }
        }



        if (skip) {
            g_free(insn_disas);
        } else {

            // /* Register callback on memory read or write */
            // qemu_plugin_register_vcpu_mem_cb(insn, vcpu_mem,
            //                                  QEMU_PLUGIN_CB_NO_REGS,
            //                                  QEMU_PLUGIN_MEM_RW, NULL);
            

            // udata which saves the callback for the current instruction.
            // on the heap, so that the vcpu_insn_exec_cb can read it.
            trex_insn_t* trex_insn = g_malloc(sizeof(trex_insn_t));
            trex_insn->metadata.direct_branch = 0;
            trex_insn->metadata.indirect_branch = 0;
            trex_insn->metadata.other = 0;
            trex_insn->vaddr = insn_vaddr;
            trex_parse_insn_to_metadata(insn_disas, &trex_insn->metadata);

            /* Register callback on instruction */
            qemu_plugin_register_vcpu_insn_exec_cb(insn, trex_insn_exec_cb,
                                                   QEMU_PLUGIN_CB_NO_REGS, trex_insn);

            /* reset skip */
            skip = (imatches || amatches);
        }

    }
}



/* Add a match to the array of matches */
static void parse_insn_match(char *match)
{
    if (!imatches) {
        imatches = g_ptr_array_new();
    }
    g_ptr_array_add(imatches, match);
}

static void parse_vaddr_match(char *match)
{
    uint64_t v = g_ascii_strtoull(match, NULL, 16);

    if (!amatches) {
        amatches = g_array_new(false, true, sizeof(uint64_t));
    }
    g_array_append_val(amatches, v);
}

/**
 * Install the plugin
 */
QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
                                           const qemu_info_t *info, int argc,
                                           char **argv)
{
    /*
     * Initialize dynamic array to cache vCPU instruction. In user mode
     * we don't know the size before emulation.
     */
    if (info->system_emulation) {
        last_exec = g_ptr_array_sized_new(info->system.max_vcpus);
    } else {
        last_exec = g_ptr_array_new();
    }

    const char *binary_ins_log_path = "trex_binary_ins_log";

    for (int i = 0; i < argc; i++) {
        char *opt = argv[i];
        g_auto(GStrv) tokens = g_strsplit(opt, "=", 2);
        if (g_strcmp0(tokens[0], "ifilter") == 0) {
            parse_insn_match(tokens[1]);
        } else if (g_strcmp0(tokens[0], "afilter") == 0) {
            parse_vaddr_match(tokens[1]);
        } else if (g_strcmp0(tokens[0], "binary_ins_log") == 0) {
            // if (tokens[1] == NULL || tokens[1][0] == '\0') {
            //     fprintf(stderr, "Invalid or missing value for binary_ins_log: %s\n", opt);
            //     return -1;
            // }
            // binary_ins_log_path = tokens[1];
            // fprintf(stderr, "binary_ins_log_path set to: %s\n", binary_ins_log_path);
        } else {
            fprintf(stderr, "option parsing failed: %s\n", opt);
            return -1;
        }
    }

    /**
     * Create/open the binary_ins_log file:
     *  Is closed at termination conditional. 
     *  */    
    trex_binary_ins_log_file = fopen(binary_ins_log_path, "w");

    /* Register translation block and exit callbacks */
    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
    qemu_plugin_register_atexit_cb(id, plugin_exit, NULL);

    return 0;
}
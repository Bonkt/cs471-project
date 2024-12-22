CS471 Cloud workloads characterization by traceability

## Folder Hierarchy
\ main - root folder\
 | plot - contains the resulting plot\
 | plugin - contains files related to plugin or QEMU\
 | python - contails all python scripts\
 | text - serves as the default output of scripts\
 | trace-selection - contains all C files related to the trace selection algorithm\

## Python Scripts
### processing.py

This script is responsible for transforming the data from the previous execlog into the correct format. (deprecated)

## C files and header
### main.c

Main file loading the files and starting the analysis of the execution log.

### datatype.h

Contains all struct definition relevant to all files and some useful constant for other files.

### parser.c

Contains functions to extract and give the data in a meaningful way to other script. This script should be the only one to really interact with the opened file (except checking address)

### trace-selection.c

Contains functions to build, check and process the traces using the execution log. This script should contain all functions with regards to traces .

### hashmap.c

Contains functions to build a hashmap and give an interface for other script to use the hashmap. This script should contain all functions with direct access to the hashmap.

## Shell script and executable
### run_vm.sh

Run the QEMU simulator with the correct plugin to retrieve the execution log (needs to be placed in a correct QEMU configuration).

### main

Usage: ./main <input_log> [-o <output_file>] [-O <hashmap_output_file>] [-i <max_instructions_per_trace>] [-b <max_blocks_per_trace>] [-pt] [-pb]

### benchmark_script.sh

Usage: ./benchmark_script.sh <input_log>
Run the specified execution log through the main script and outputs in the text folder.

### trace_distance_mo

Usage: ./trace_distance_mo <input_file> -o <output_file>
Takes the previous script output as input and output for each reuse distance a pair (reuse distance, count).
import time

t = time.time()
f = open("python/trace_output2.log", "r")
lines = f.readlines()
f.close()
print("Time to read file: " + str(time.time()-t))
t = time.time()

#ARM branch instructions
jumps_t = ("br", "blr", "ret", "bx") #may need to add more or remove some
#removed 
jumps = ("b ", "bl ", "cbz ", "cbnz ", "tbz ", "tbnz ", "b.")

f = open("processed_trace2", "wb")

for i, line in enumerate(lines):
    s = line.split(", ")
    if s[0] == "0":
        # read string as hex number from format 0x1234
        addr = int(s[1], 16)
        metadata = 0
        if s[3].split("\"")[1].startswith(jumps):
            metadata += 1
        if s[3].split("\"")[1].startswith(jumps_t):
            metadata += 2
        #convert addr in array of bytes
        addr = addr.to_bytes(8, byteorder='little')
        f.write(addr + metadata.to_bytes(1, byteorder='little'))
        


print("Time to print results: " + str(time.time()-t))
import time

'''
This file is deprecated, it is not used in the final version of the project.

'''


t = time.time()
f = open("python/trace_output2.log", "r")
lines = f.readlines()
f.close()
print("Time to read file: " + str(time.time()-t))
t = time.time()

dict_block = {}
inst = {}
count = 0
NOT_SEEN = "N"
SEEN = "S"
REUSED = "R"
PC = 0
LENGTH = 0
previous = ""

#ARM branch instructions
jumps = ("br", "blr", "ret", "bx") #may need to add more or remove some
#removed 
#removed "b ", "bl ", "cbz ", "cbnz ", "tbz ", "tbnz ", "b."

def incr(str, dict):
    if str in dict:
        dict[str] = (dict[str][0]+1, dict[str][1]+(PC-dict[str][2]), PC, dict[str][3])
    else:
        dict[str] = (1, 0, PC, 0)

prev_br = False

for i, line in enumerate(lines):
    if line.split(", ")[0] == "0":
        LENGTH += 1
        s = line.split(", ")
        if prev_br:
            if len(s) > 1:
                if previous in dict_block:
                    dict_block[previous] = (dict_block[previous][0], dict_block[previous][1], dict_block[previous][2], dict_block[previous][3]+LENGTH)
                LENGTH = 0
                incr(s[1], dict_block)
                previous = s[1]
            prev_br = False
        if line.split("\"")[1].startswith(jumps):
            prev_br = True
        if len(s) > 1:
            incr(s[1], inst)
        PC += 1

print("Time to parse file: " + str(time.time()-t))
t = time.time()

result = [{"key": key, "value": (value[0], value[1]/value[0], value[3])} for key, value in dict_block.items()]
result_inst = [{"key": key, "value": (value[0])} for key, value in inst.items()]
result_sorted = sorted(result, key=lambda x: x["value"][0])

for item in result_sorted:
    if item["value"][0] > 10000:
        print(item["key"] + ": " + str(item["value"]))

#total = sum(item["value"] for item in result_sorted)

equal_to_1 = [item for item in result_sorted if item["value"][0] == 1]
superior_to_1 = [item for item in result_sorted if item["value"][0] > 1]
print("Ratio of jump destinations executed more than once to the total number of jump destinations: " + str(len(superior_to_1)/len(result_sorted)))
print("Ratio of jump destinations executed more than once to the total number of jump destinations (summed on occurance): " + str((sum(item["value"][0] for item in superior_to_1)-len(superior_to_1))/sum(item["value"][0] for item in result_sorted)))

total = sum(item["value"][0] for item in superior_to_1)
TRD = sum(item["value"][1]*item["value"][0] for item in superior_to_1)/total
print("Trace Reuse Distance: " + str(TRD))
print("Total number of executed blocks, executed more than once: " + str(total))
sorted_by_distance = sorted(superior_to_1, key=lambda x: x["value"][1])
median = sorted_by_distance[len(sorted_by_distance)//2]
print("Median distance: " + str(median["value"][1]))

#print("Number of jump destinations executed only once: " + str(len(equal_to_1)))
#print("Number of jump destinations executed more than once: " + str(len(superior_to_1)))

print("Trace coverage: " + str(len([item["value"] for item in result_inst if item["value"] > 1])/len(result_inst))) 
print("Average Trace Length: " + str(sum(item["value"][2] for item in superior_to_1)/sum(item["value"][0] for item in superior_to_1)))
#median
sorted_by_length = sorted(superior_to_1, key=lambda x: x["value"][2]/x["value"][0])
median = sorted_by_length[len(sorted_by_length)//2]
print("Median length: " + str(median["value"][2]/median["value"][0]))

#find and print max distance, only distance
max_distance = 0
max_distance_key = ""
for item in superior_to_1:
    if item["value"][2]/item["value"][0] > max_distance:
        max_distance = item["value"][2]/item["value"][0]
        max_distance_key = item["key"]
print("Max distance: " + str(max_distance))
print("Max distance key: " + max_distance_key)
print("max distance key: " + str(dict_block[max_distance_key]))


#write in a file "output.csv" results from superior_to_1
f = open("python/output.csv", "w")
for item in superior_to_1:
    if item["value"][1] < 50000:
        f.write(str(item["value"][0]) + ";" + str(item["value"][1]).replace(".", ",") + ";" + str(item["value"][2]) + "\n")

print(len([item for item in superior_to_1 if item["value"][1] < 50000])/len(superior_to_1))
f.close()

print("Time to print results: " + str(time.time()-t))
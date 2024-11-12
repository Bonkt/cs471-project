f = open("trace_output.log", "r")
lines = f.readlines()
f.close()

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
jumps = ("b ", "bl ", "bx ", "cbz ", "cbnz ", "tbz ", "tbnz ", "br ", "blr ", "ret", "b.") #may need to add more or remove some

def incr(str, dict):
    if str in dict:
        dict[str] = (dict[str][0]+1, dict[str][1]+(PC-dict[str][2]), PC, dict[str][3])
    else:
        dict[str] = (1, 0, PC, 0)

for i, line in enumerate(lines):
    LENGTH += 1
    if line.split("\"")[1].startswith(jumps):
        s = lines[i+1].split(", ")
        if len(s) > 1:
            if previous in dict_block and dict_block[previous][3] == 0:
                dict_block[previous] = (dict_block[previous][0], dict_block[previous][1], dict_block[previous][2], LENGTH)
            LENGTH = 0
            incr(s[1], dict_block)
            previous = s[1]
    s = line.split(", ")
    if len(s) > 1:
        incr(s[1], inst)
    PC += 1

result = [{"key": key, "value": (value[0], value[1]/value[0], value[3])} for key, value in dict_block.items()]
result_inst = [{"key": key, "value": (value[0])} for key, value in inst.items()]
result_sorted = sorted(result, key=lambda x: x["value"][0])

for item in result_sorted:
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

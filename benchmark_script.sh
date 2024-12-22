#!/bin/bash

# Default input file name
input_file="data_caching_log"

# Check if a custom input file is provided as the first argument
if [ ! -z "$1" ]; then
    input_file="$1"
    echo "Using input file: $input_file"
else
    echo "No input file specified. Using default: $input_file"
fi

# Define arrays for values of -i and -b
#instructions=(16 32 64 128 256 1024 4096)
instructions=(8 16 32 1024)
#blocks=(4 8 16 32 128 512 1024)
blocks=(4 512)

# Maximum number of parallel processes
max_processes=8
current_processes=0

# Loop through pairwise combinations of instructions and blocks
for i in "${instructions[@]}"; do
    for b in "${blocks[@]}"; do
        # Define the output filename based on the current pair
        output_file="text/data_caching_i${i}_b${b}.csv"
        
        # Run the benchmark command in the background
        echo "Running benchmark with -i $i and -b $b using input file $input_file"
	echo "$input_file -o $output_file -i $i -b $b "
	./main "$input_file" -o "$output_file" -i "$i" -b "$b" &
        
        # Increment the process count
        ((current_processes++))

        # If we've reached the max number of processes, wait for any to finish
        if [ $current_processes -ge $max_processes ]; then
            wait -n  # Wait for any background process to finish
            ((current_processes--))  # Decrease the process count
        fi
    done
done

# Wait for all remaining background processes to finish
wait

echo "All benchmarks have completed!"

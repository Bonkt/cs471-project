#!/bin/bash
# Directory containing the .csv files
input_dir="text/"
# Output directory for the results
output_dir="text/"

# Create the output directory if it doesn't exist
if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
fi

# Function to process a single file
process_file() {
    local file="$1"
    local filename=$(basename -- "$file")
    local name="${filename%.*}"
    local output_file="$output_dir/${name}_distance.txt"

    echo "Processing $file..."
    ./trace_distance_mo "$file" -o "$output_file"
}

# Export the function for parallel execution
export -f process_file
export output_dir

# Find all .csv files and process them with 10 parallel jobs
find "$input_dir" -maxdepth 1 -name "*.csv" | xargs -n 1 -P 10 -I {} bash -c 'process_file "$@"' _ {}

echo "All files processed."

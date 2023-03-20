#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=1
#SBATCH --time=00:05:00
#SBATCH --mem=500mb
#SBATCH --export=None

# add one to all input values(only work for integrals)

# Check that the correct number of arguments have been passed
if [ $# -ne 2 ]
then
    echo "Usage: $0 input_file output_file"
    echo $@
    exit 1
fi

input_file=$1
output_file=$2


# Check that the input file exists
if [ ! -f "$input_file" ]
then
    echo "Input file $input_file does not exist"
    exit 1
fi

# Empty the output file if it exists
if [ -f "$2" ]; then
  > "$2" # Empty the file if it exists
fi

# echo "7.3 8 9" >> "$output_file"
# echo "10.2 11 12" >> "$output_file"

# loop over lines in input file, adding 1 to each value
while read -r line; do
    # split line into array of values
    read -ra values <<< "$line"
    for i in "${!values[@]}"; do
        # add 1 to value
        ((values[i]++))
    done
    # join values array into a string with spaces between each value
    modified_line=$(printf "%s " "${values[@]}")
    # append modified line to output file
    echo "$modified_line" >> "$output_file"
done < "$input_file"
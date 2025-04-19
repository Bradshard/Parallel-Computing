#!/bin/bash

FROM=$1
TO=$2
NPROCS=$3

# Validate input arguments
if [ -z "$FROM" ] || [ -z "$TO" ]; then
    echo "Usage: $0 <FROM> <TO> <NPROCS>"
    exit 1
fi

# Create output directory if it doesn't exist
output_dir="slurm_outs"
mkdir -p "$output_dir"
mkdir -p parallel_solutions
rm -f parallel_solutions/*

# Loop over the range using seq
for arg in $(seq "$FROM" "$TO"); do
    # Compute the shifted value
    size=$((1 << arg))

    # Print information to the terminal
    echo "Running cyclic_parallel with size: $size, with nprocs: $NPROCS"

    # Submit the job with Slurm
    sbatch -o "$output_dir/$size.out" --ntasks="$NPROCS" slurm_test.sh $size 

    echo "Job submitted for $size. Output will be written to $output_dir/$size.out"
    echo ""

done

# Wait for all jobs to finish
echo "Waiting for results... (checking squeue for $USER)"
while squeue | grep -q "$USER"; do
    sleep 1
done

echo "All jobs are completed!"


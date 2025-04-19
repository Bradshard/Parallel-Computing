#!/bin/bash

# Check if at least three arguments are supplied
if [ "$#" -lt 3 ]; then
  echo "Usage: $0 <arg1> <arg2> <arg3> [--no-generate]"
  echo "Give sizes in log2, <arg1> <arg2>"
  echo "<arg3> is process count"
  echo "[--no-generate] optional argument to skip matrix generation"
  exit 1
fi

FROM=$1
TO=$2
NPROCS=$3
SKIP_GENERATE=false

# Check for optional argument to skip matrix generation
if [ "$4" == "--no-generate" ]; then
  SKIP_GENERATE=true
fi

if [ "$FROM" -lt 7 ]; then
  echo "Starting size cannot be smaller than 7"
  exit 1
fi
if [ "$TO" -gt 27 ]; then
  echo "End size cannot be larger than 25"
  exit 1
fi

# Conditionally generate matrices
if [ "$SKIP_GENERATE" == false ]; then
  echo "Generating matrices..."
  mkdir -p matrices
  rm -f matrices/*
  python3 matrix_gen.py "$FROM" "$((TO + 1))"
else
  echo "Skipping matrix generation"
fi

./run_cyclic_parallel.sh "$FROM" "$TO" "$NPROCS"

echo "No jobs matching $USER in the queue. Proceeding to run tests..."
./test

echo "Generating results and plots in results directory..."
mkdir -p results

python3 plot_generator.py "$NPROCS"



# 577-Project

## Build the Program
To compile the program, run:
```bash
make
```

---

## Run the Program
Use the `run.sh` script to execute the program with specific arguments.

**Usage:**
```bash
./run.sh <FROM> <TO> <NPROCS> [--no-generate]
```

- `<FROM>`: Starting matrix size in log2 (e.g., `7` for \(2^7\)).
- `<TO>`: Ending matrix size in log2 (e.g., `25` for \(2^{25}\)).
  
- `<NPROCS>`: Number of processes to use (e.g., `16`).
- `--no-generate` (optional): Skip matrix generation.

  **Important:** You must run the script at least once without the `--no-generate` option to generate the necessary matrices before skipping generation in subsequent runs.

**Examples:**
1. Generate matrices and run with 16 processes:
   ```bash
   ./run.sh 7 25 16
   ```

2. Skip matrix generation and run with 32 processes:
   ```bash
   ./run.sh 7 25 32 --no-generate
   ```

## Results
You can find the results in the `results` directory. `greyfurt_results` includes results of runs with 16 and 32 million sized matrices with process count going from 2 to 64 in powers of 2.

For running the program with different number of processes, you can use the following command:
```bash
for n in $(seq 1 6); do ./run.sh 7 25 $((2**n)) --no-generate; done

```

Then you can use the `ideal_speedup_plot.py` script to plot the ideal speedup graph in `greyfurt_results`. Copy it to the results then run it. Plot will be saved as `ideal_speedup_plot.png`.



---


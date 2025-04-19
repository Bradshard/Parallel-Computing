import os
import sys
import matplotlib.pyplot as plt


nproc = sys.argv[1]
# File path to the text file
file_path = "analytics.txt"

# Ensure results directory exists
os.makedirs("./results", exist_ok=True)

# Read the file and parse the data
results = []
with open(file_path, "r") as f:
    for line in f:
        values = [float(x) for x in line.strip().split(",")]
        results.append(values)

# Sort results by the 'size' field (last column)
results.sort(key=lambda x: x[8])

# Separate the results into individual lists and multiply times by 1000 (convert to ms)
dgtsv_time = [r[0] * 1000 for r in results]
dgtsv_err = [r[1] for r in results]  # Read error but don't use it yet
dgesv_time = [r[2] * 1000 for r in results]
dgesv_err = [r[3] for r in results]  # Read error but don't use it yet
cyclic_seq_time = [r[4] * 1000 for r in results]
cyclic_seq_err = [r[5] for r in results]  # Read error but don't use it yet
cyclic_par_time = [r[6] * 1000 for r in results]
cyclic_par_err = [r[7] for r in results]  # Read error but don't use it yet
sizes = [r[8] for r in results]

# Filter out entries with DGESV time of 0
filtered_sizes = [s for s, t in zip(sizes, dgesv_time) if t != 0]
filtered_dgesv_time = [t for t in dgesv_time if t != 0]

# Define consistent colors
colors = {
    "dgtsv": "blue",
    "cyclic_seq": "green",
    "cyclic_par": "red",
    "dgesv": "orange"
}

# Save execution times to file
with open(f"./results/execution_times{nproc}.txt", "w") as out_file:
    out_file.write("Size,DGTSV Time (ms),DGESV Time (ms),Cyclic Sequential Time (ms),Cyclic Parallel Time (ms)\n")
    for size, dgtsv, dgesv, cyclic_seq, cyclic_par in zip(sizes, dgtsv_time, dgesv_time, cyclic_seq_time, cyclic_par_time):
        out_file.write(f"{size},{dgtsv},{dgesv},{cyclic_seq},{cyclic_par}\n")

# Plot DGTSV, Cyclic Sequential, and Cyclic Parallel on a separate plot
plt.figure(figsize=(12, 6))
plt.plot(sizes, dgtsv_time, label="DGTSV Time", marker="o", color=colors["dgtsv"])
plt.plot(sizes, cyclic_seq_time, label="Cyclic Sequential Time", marker="o", color=colors["cyclic_seq"])
plt.plot(sizes, cyclic_par_time, label="Cyclic Parallel Time", marker="o", color=colors["cyclic_par"])
plt.xlabel("Matrix Size")
plt.ylabel("Execution Time (ms)")
plt.title("DGTSV vs Cyclic Times")
plt.legend()
plt.savefig(f"./results/dgtsv_cyclic_times{nproc}.png", dpi=300, bbox_inches="tight")
plt.close()

# Plot execution times (all)
plt.figure(figsize=(12, 6))
plt.plot(sizes, dgtsv_time, label="DGTSV Time", marker="o", color=colors["dgtsv"])
plt.plot(filtered_sizes, filtered_dgesv_time, label="DGESV Time", marker="o", color=colors["dgesv"])
plt.plot(sizes, cyclic_seq_time, label="Cyclic Sequential Time", marker="o", color=colors["cyclic_seq"])
plt.plot(sizes, cyclic_par_time, label="Cyclic Parallel Time", marker="o", color=colors["cyclic_par"])
plt.xlabel("Matrix Size")
plt.ylabel("Execution Time (ms)")
plt.title("Execution Times")
plt.legend()
plt.savefig(f"./results/execution_times{nproc}.png", dpi=300, bbox_inches="tight")
plt.close()

# Plot speedup
speedup = [seq / par for seq, par in zip(cyclic_seq_time, cyclic_par_time)]
plt.figure(figsize=(12, 6))
plt.plot(sizes, speedup, label="Cyclic Parallel Speedup", marker="o", color=colors["cyclic_par"])
plt.xlabel("Matrix Size")
plt.ylabel("Speedup (Sequential Time / Parallel Time)")
plt.title("Parallel Speedup")
plt.legend()
plt.savefig(f"./results/speedup{nproc}.png", dpi=300, bbox_inches="tight")
plt.close()


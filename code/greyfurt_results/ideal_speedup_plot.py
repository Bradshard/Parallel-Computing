import matplotlib.pyplot as plt

results_16 = []
results_33 = []

# Read data for both 16 million and 33 million matrices
for i in [2, 4, 8, 16, 32, 64]:
    with open(f"./execution_times{i}.txt", "r") as f:
        f.readline()  # Skip the header
        data = f.readlines()
        m16 = data[-2].split(',')
        m33 = data[-1].split(',')
        results_16.append(tuple(m16[-2:]))
        results_33.append(tuple(m33[-2:]))

sizes = [2, 4, 8, 16, 32, 64]

# Calculate speedups
speedup_16 = [float(x[0].strip()) / float(x[1].strip()) for x in results_16]
speedup_33 = [float(x[0].strip()) / float(x[1].strip()) for x in results_33]

# Plot speedups
plt.figure(figsize=(14, 8))
plt.plot(sizes, speedup_16, label="Speedup 16M Matrix", marker="o", color="tab:red", linewidth=2)
plt.plot(sizes, speedup_33, label="Speedup 33M Matrix", marker="o", color="tab:green", linewidth=2)

# Add dashed y=x line
plt.axline((0, 0), slope=1, linestyle="--", color="tab:blue", linewidth=2, label="Ideal Speedup")

# Annotate speedup values
for x, y in zip(sizes, speedup_16):
    plt.annotate(f"{y:.2f}", (x, y), textcoords="offset points", xytext=(0, 10), ha='center', fontsize=10, color="tab:red")
for x, y in zip(sizes, speedup_33):
    plt.annotate(f"{y:.2f}", (x, y), textcoords="offset points", xytext=(0, 10), ha='center', fontsize=10, color="tab:green")

# Set axis limits to ensure the graph starts at (0, 0)
plt.xlim(0, sizes[-1] * 1.1)  # Extend slightly beyond the largest size
plt.ylim(0, max(max(speedup_16), max(speedup_33)) * 1.2)

# Set x-axis ticks
plt.xticks(sizes, labels=[str(size) for size in sizes])

# Add grid, labels, and title
plt.grid(visible=True, linestyle="--", linewidth=0.5, alpha=0.7)
plt.xlabel("Number of Processes", fontsize=14)
plt.ylabel("Speedup (Sequential Time / Parallel Time)", fontsize=14)
plt.title("Parallel Speedup vs Number of Processes", fontsize=16)

# Adjust legend placement
plt.legend(loc="upper left", fontsize=12)
plt.tight_layout()
plt.savefig("ideal_speedup_plot.png", dpi=300)

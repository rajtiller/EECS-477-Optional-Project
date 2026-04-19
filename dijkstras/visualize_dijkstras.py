#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv("dijkstras_running_times.csv")

# densities you care about
densities = [0.1, 0.55, 1.0]

# metrics to plot
metrics = [
    ("seconds", "Runtime (seconds)"),
    ("total_distance", "Total Distance")
]

fig, axes = plt.subplots(len(densities), len(metrics), figsize=(14, 12))

# ensure consistent ordering
categories = df["category"].unique()

for i, density in enumerate(densities):
    df_d = df[df["edge_density"] == density]

    for j, (metric, ylabel) in enumerate(metrics):
        ax = axes[i, j]

        for category in categories:
            sub = df_d[df_d["category"] == category].sort_values("num_nodes")

            ax.plot(
                sub["num_nodes"],
                sub[metric],
                marker="o",
                label=category
            )

        ax.set_xscale("linear")

        # runtime plot should be log-log (important)
        if metric == "seconds":
            ax.set_yscale("linear")

        ax.set_title(f"density={density}, {ylabel}")
        ax.set_xlabel("Num Nodes")
        ax.set_ylabel(ylabel)

        ax.grid(True, which="both", linestyle="--", alpha=0.4)

        if i == 0 and j == 0:
            ax.legend()

plt.tight_layout()
plt.show()
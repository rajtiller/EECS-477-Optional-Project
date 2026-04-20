#!/usr/bin/env python3
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


def main() -> None:
    here = Path(__file__).resolve().parent
    csv_path = here / "selection_running_times.csv"
    if not csv_path.is_file():
        csv_path = here / "running_times.csv"
    df = pd.read_csv(csv_path)
    if "category" not in df.columns:
        df = pd.read_csv(csv_path, names=["category", "size", "seconds"], header=None)

    # Keep category order stable for easier comparison across runs.
    categories = list(df["category"].dropna().unique())

    plt.figure(figsize=(11, 6))
    for category in categories:
        sub = df[df["category"] == category].sort_values("size")
        if sub.empty:
            continue
        plt.plot(sub["size"], sub["seconds"], marker="o", label=category)

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Input size (log scale)")
    plt.ylabel("Runtime (seconds, log scale)")
    plt.title("Selection Runtime by Category")
    plt.grid(True, which="both", linestyle="--", alpha=0.4)
    plt.legend()
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

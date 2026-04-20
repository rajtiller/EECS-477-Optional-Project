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
        # Backward-compatible parsing for headerless CSV files.
        df = pd.read_csv(csv_path, names=["category", "size", "seconds"], header=None)

    # Keep category order stable for easier comparison across runs.
    categories = list(df["category"].dropna().unique())

    fig, axes = plt.subplots(2, 1, figsize=(11, 10))

    # ---- Plot 1: Runtime ----
    ax = axes[0]
    for category in categories:
        sub = df[df["category"] == category].sort_values("size")
        if sub.empty:
            continue
        ax.plot(sub["size"], sub["seconds"], marker="o", label=category)

    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("Input size (log scale)")
    ax.set_ylabel("Runtime (seconds, log scale)")
    ax.set_title("Selection Runtime by Category")
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend()

    # ---- Plot 2: Guessed median correctness ----
    ax = axes[1]
    if "guessed_median" in df.columns:
        baseline = (
            df[df["category"] == "sorting"][["size", "guessed_median"]]
            .drop_duplicates(subset=["size"])
            .rename(columns={"guessed_median": "true_median"})
        )
        if not baseline.empty:
            merged = df.merge(baseline, on="size", how="left")
            merged["median_abs_error"] = (merged["guessed_median"] - merged["true_median"]).abs()
            for category in categories:
                sub = merged[merged["category"] == category].sort_values("size")
                if sub.empty:
                    continue
                ax.plot(sub["size"], sub["median_abs_error"], marker="o", label=category)
            ax.set_xscale("log")
            ax.set_yscale("log")
            ax.set_xlabel("Input size (log scale)")
            ax.set_ylabel("|guessed median - true median|")
            ax.set_title("Median Accuracy by Category")
            ax.grid(True, which="both", linestyle="--", alpha=0.4)
            ax.legend()
        else:
            ax.text(
                0.5,
                0.5,
                "No 'sorting' baseline found for guessed_median correctness check.",
                ha="center",
                va="center",
                transform=ax.transAxes,
            )
            ax.set_axis_off()
    else:
        ax.text(
            0.5,
            0.5,
            "CSV has no guessed_median column.\nRe-run benchmark to include correctness plot.",
            ha="center",
            va="center",
            transform=ax.transAxes,
        )
        ax.set_axis_off()

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()

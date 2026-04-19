#!/usr/bin/env python3
import csv
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


def load_df(path: str) -> pd.DataFrame:
    path = Path(path)
    with path.open(newline="", encoding="utf-8") as f:
        header = next(csv.reader(f))
    if "pattern_length" in header:
        return pd.read_csv(path)
    if len(header) == 4 and header[0] == "category":
        # Rows are category,text_length,pattern_length,seconds,matches
        return pd.read_csv(
            path,
            names=["category", "text_length", "pattern_length", "seconds", "matches"],
            skiprows=1,
        )
    # Oldest format: no pattern_length column
    return pd.read_csv(
        path,
        names=["category", "text_length", "seconds", "matches"],
        skiprows=1,
    )


def _plot_metric(ax, df_pl, categories: list[str], ycol: str, ylabel: str) -> None:
    for cat in categories:
        sub = df_pl[df_pl["category"] == cat].sort_values("text_length")
        if sub.empty:
            continue
        ax.plot(sub["text_length"], sub[ycol], marker="o", label=cat)
    ax.set_xscale("log")
    if ycol == "seconds":
        ax.set_yscale("log")
    ax.set_xlabel("Text length (log)")
    ax.set_ylabel(ylabel)
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.legend(fontsize=8)


def main() -> None:
    here = Path(__file__).resolve().parent
    df = load_df(str(here / "pattern_matching_running_times.csv"))

    preferred = ["naive", "no-checks", "checks", "logn", "logn + checks"]
    seen = set(df["category"].unique())
    categories = [c for c in preferred if c in seen] + [
        c for c in df["category"].unique() if c not in preferred
    ]

    pattern_lengths = [1, 4, 16]
    if "pattern_length" not in df.columns:
        fig, ax = plt.subplots(figsize=(10, 5))
        _plot_metric(ax, df, categories, "seconds", "Runtime (s, log)")
        ax.set_title("Runtime vs text length (no pattern_length in CSV)")
        plt.tight_layout()
        plt.show()
        plt.close()
        fig, ax = plt.subplots(figsize=(10, 5))
        _plot_metric(ax, df, categories, "matches", "Matches")
        ax.set_title("Matches vs text length (no pattern_length in CSV)")
        plt.tight_layout()
        plt.show()
        plt.close()
        return

    # ---- Runtime: one row, subplot per pattern length ----
    fig, axes = plt.subplots(1, 3, figsize=(15, 5), sharey=True)
    for ax, pl in zip(axes, pattern_lengths):
        df_pl = df[df["pattern_length"] == pl]
        if df_pl.empty:
            ax.set_title(f"Pattern length = {pl}")
            ax.text(
                0.5,
                0.5,
                "No rows for this pattern length.\nRegenerate CSV with current benchmark.",
                ha="center",
                va="center",
                transform=ax.transAxes,
            )
            ax.set_axis_off()
            continue
        _plot_metric(ax, df_pl, categories, "seconds", "Runtime (s, log)")
        ax.set_title(f"Pattern length = {pl}")
    fig.suptitle("Runtime vs text length", y=1.02)
    plt.tight_layout()
    plt.show()
    plt.close()

    # ---- Matches: one row, subplot per pattern length ----
    fig, axes = plt.subplots(1, 3, figsize=(15, 5), sharey=True)
    for ax, pl in zip(axes, pattern_lengths):
        df_pl = df[df["pattern_length"] == pl]
        if df_pl.empty:
            ax.set_title(f"Pattern length = {pl}")
            ax.text(
                0.5,
                0.5,
                "No rows for this pattern length.\nRegenerate CSV with current benchmark.",
                ha="center",
                va="center",
                transform=ax.transAxes,
            )
            ax.set_axis_off()
            continue
        _plot_metric(ax, df_pl, categories, "matches", "Matches")
        ax.set_title(f"Pattern length = {pl}")
    fig.suptitle("Matches vs text length", y=1.02)
    plt.tight_layout()
    plt.show()
    plt.close()


if __name__ == "__main__":
    main()

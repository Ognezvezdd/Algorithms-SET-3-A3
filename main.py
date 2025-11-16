import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

df = pd.read_csv("input.csv", sep=r", ", engine="python")

types = sorted(df["type"].unique())
for tp in types:
    d = df[df["type"] == tp].copy()

    # Первый
    plt.figure(figsize=(8, 5))
    d_quick = d[(d["algo"] == "quick") | (d["threshold"] < 0)].sort_values("size")
    plt.plot(d_quick["size"], d_quick["avg_ms"], label="quick", lw=2)

    ths = sorted(d.loc[d["algo"] == "best", "threshold"].unique())
    for th in ths:
        dh = d[(d["algo"] == "best") & (d["threshold"] == th)].sort_values("size")
        plt.plot(dh["size"], dh["avg_ms"], label=f'"best" algo')
    plt.xlabel("Размер (n)")
    plt.ylabel("Время (ms)")
    plt.title(f"{tp}: время сортировки")
    plt.grid()
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"time for {tp}.png")
    plt.close()

    # Второй
    best_rows = []
    q_by_n = dict(zip(d_quick["size"], d_quick["avg_ms"]))
    for n in sorted(d["size"].unique()):
        dh_n = d[(d["algo"] == "best") & (d["size"] == n)]
        if not dh_n.empty and n in q_by_n:
            best_h = dh_n["avg_ms"].min()
            best_th = int(dh_n.loc[dh_n["avg_ms"].idxmin(), "threshold"])
            ratio = best_h / q_by_n[n] if q_by_n[n] > 0 else np.nan
            best_rows.append((n, best_th, best_h, q_by_n[n], ratio))
    best_df = pd.DataFrame(best_rows,
                           columns=["size", "best_threshold", "best_h_ms", "quick_ms", "ratio"])

    plt.figure(figsize=(8, 5))
    plt.plot(best_df["size"], best_df["ratio"], label="best / quick")
    plt.axhline(1.0, color="k", ls="--")
    plt.xlabel("size (n)")
    plt.ylabel("Отношение (меньше 1 — быстрее best)")
    plt.title(f"{tp}: выигрыш best")
    plt.grid()
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"who best {tp}.png")
    plt.close()

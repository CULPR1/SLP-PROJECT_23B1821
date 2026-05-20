import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv("180_50%_throttle_vs_encoder.csv")

# Sort by angle (VERY IMPORTANT)
df = df.sort_values(by="encoder")

# ---- Plot ----
fig, ax1 = plt.subplots(figsize=(12, 6))

# Motor output vs angle
ax1.plot(df["encoder"], df["Motor_output"], linewidth=2, label="Motor Output")

ax1.set_xlabel("Encoder Angle (degrees)")
ax1.set_ylabel("Motor Output (µs)")
ax1.set_xlim(0, 360)
ax1.set_ylim(1000, 1500)

# ---- Show window band on secondary axis ----
ax2 = ax1.twinx()

ax2.fill_between(
    df["encoder"],
    df["Window_lower"],
    df["Window_upper"],
    alpha=0.2,
    label="Active Window"
)

ax2.set_ylabel("Window Range (degrees)")
ax2.set_ylim(0, 360)

# ---- Legends ----
lines1, labels1 = ax1.get_legend_handles_labels()
lines2, labels2 = ax2.get_legend_handles_labels()

ax1.legend(lines1 + lines2, labels1 + labels2, loc="upper right")

plt.title("Motor Output vs Encoder Angle with Control Window")
plt.grid(True)
plt.tight_layout()
plt.show()
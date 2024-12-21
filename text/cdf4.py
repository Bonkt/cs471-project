import matplotlib.pyplot as plt
from collections import Counter
import numpy as np
import time
import sys

start = time.time()
# Step 1: Read args from python command line
#if len(sys.argv) < 4:
#    print("Usage: python cdf4.py filename1> <filename2> <filename3>")
#    sys.exit(1)
    
filenames = ["data_caching_i8.txt","media_streaming_i8.txt", "data_serving_i8.txt"]  # Read the three filenames
names = ["Data Serving", "Media Streaming", "Data Caching"]
cdf_colors = ["#FFC300", "#7b8fff", "#FF5733"]      # Colors for CDF lines
fill_colors = ["#FFF1B0", "#C1D4FF", "#FFBD9B"]     # Lighter shades for fills

cdf_data = []  # To store CDF data and threshold info
# Define a common x-axis range
common_x = np.arange(0, 5001)

# Process each file
for i, filename in enumerate(filenames):
    with open(filename) as f:
        data = f.readlines()

    print(f"Finished reading {filename}")

    # Clean and filter data
    data = [int(x.strip()) for x in data]  # Convert strings to integers
    filtered_data = [x for x in data if x != -1]  # Ignore -1 values

    # Compute frequencies (PDF)
    counter = Counter(filtered_data)  # Count occurrences of each number
    total = sum(counter.values())  # Total number of non-(-1) values
    pdf = {k: v / total for k, v in counter.items()}  # Normalize to get probabilities

    # Compute CDF
    sorted_keys = sorted(pdf.keys())  # Sort the keys (values)
    cdf_values = np.cumsum([pdf[k] for k in sorted_keys])  # Compute cumulative probabilities

    # Initialize CDF with x=0
    cdf_dict = {0: 0.0}
    for k, v in zip(sorted_keys, cdf_values):
        cdf_dict[k] = v

    # Extend the CDF to cover all x in common_x
    extended_cdf = []
    for x in common_x:
        if x in cdf_dict:
            extended_cdf.append(cdf_dict[x])
        elif x < sorted_keys[0]:
            extended_cdf.append(0.0)
        elif x > sorted_keys[-1]:
            extended_cdf.append(1.0)
        else:
            # Find the closest lower x
            lower_x = max([key for key in sorted_keys if key < x], default=0)
            extended_cdf.append(cdf_dict.get(lower_x, 0.0))

    # Find the x-value where the CDF reaches 90%
    idx = np.searchsorted(extended_cdf, 0.9)
    x_90 = common_x[idx] if idx < len(common_x) else common_x[-1]

    # Store CDF data and the 90% threshold for sorting
    cdf_data.append((x_90, common_x, extended_cdf, cdf_colors[i], fill_colors[i], filename))

# Sort by the 90% threshold (x_90 value) in ascending order
cdf_data.sort(key=lambda x: x[0])  # Sort by x_90 (smallest to largest)

# Step 2: Plot the sorted CDFs with fills
plt.figure(figsize=(12, 6))

# Plot fill regions first
for i in range(len(cdf_data) - 1):
    _, _, cdf1, _, color, _ = cdf_data[i]
    _, _, cdf2, _, _, _ = cdf_data[i + 1]
    plt.fill_between(
        common_x,
        cdf1,
        cdf2,
        color=color,
        alpha=0.5,
    )

# Fill between the last CDF and the x-axis
_, _, cdf_last, _, color, _ = cdf_data[-1]
plt.fill_between(
    common_x,
    cdf_last,
    0,
    color=color,
    alpha=0.5
)

# Plot CDF lines on top of fills
for i, (x_90, sorted_keys, cdf_values, color, _, filename) in enumerate(cdf_data):
    # label = f"CDF {i+1} ({filename[:12]}, 90% at x={x_90})"
    plt.plot(
        sorted_keys,
        cdf_values,
        color=color,
        linewidth=2,
        label= names[i]
    )
    

for i, (x_90, sorted_keys, cdf_values, color, _, filename) in enumerate(cdf_data):
    # Add vertical line for 90% threshold
    plt.axvline(
        x=x_90,
        color=color,
        linestyle="--",
        linewidth=1.5,
        label=f"90% Threshold (x={x_90})"
    )
    
# Add vertical line for 32KB cache size
inst_line_size = 8
cache_line_size = inst_line_size*4+21
cache_entries = 32*1024//cache_line_size
    
plt.axvline(
    x=1024,
    color='black',
    linestyle="--",
    linewidth=1.5,
    label=f"32KB cache size"
)

# Configure plot aesthetics
plt.ylim(0, 1)
plt.xlabel("Trace Reuse Distance", fontsize=18)
plt.ylabel("Cumulative Distribution Function (CDF)", fontsize=18)
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend(loc='upper left', fontsize=16)
plt.tight_layout()

# Use symmetric log scale with a linear threshold around 1
plt.xscale('symlog', linthresh=1, base=2, linscale=0.2)
plt.xlim(0, 5000)

# Customize x-ticks to show both linear and logarithmic regions clearly
plt.xticks(
    ticks=[0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096],
    labels=['0', '1', '2', '4', '8', '16', '32', '64', '128', '256', '512', '1024', '2048', '4096'],
    fontsize=14
)
plt.yticks(np.arange(0, 1.1, 0.1), fontsize=14)

# Save the plot
import os
os.makedirs("plot", exist_ok=True)  # Ensure the 'plot' directory exists
plt.savefig(f"plot/cdf_I8_B32_1.png")
print(f"Plot saved to plot/cdf_I256_B32.png")

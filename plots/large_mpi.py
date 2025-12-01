import matplotlib.pyplot as plt
import numpy as np

ranks = [1, 2, 4, 8, 16]

# hardcoded values tested on  11th Gen Intel(R) Core(TM) i5-1135G7

latency = [
    None,
    0.000003107,
    0.000004400,
    0.000012172,
    0.000028289
]

bandwidth = [
    None,
    2563.92,
    2539.13,
    1708.34,
    961.87
]

lat_ranks = [ranks[i] for i, l in enumerate(latency) if l is not None]
lat_values = [l for l in latency if l is not None]

plt.figure(figsize=(10, 6))
plt.plot(lat_ranks, lat_values, marker='o', linewidth=2, markersize=8, color='red', label='Latency')
plt.xlabel('Number of Ranks', fontsize=12)
plt.ylabel('Latency (seconds)', fontsize=12)
plt.title('MPI Latency vs Number of Ranks', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.xticks(ranks[1:])
plt.yscale('log')
plt.legend(fontsize=10)

for r, l in zip(lat_ranks, lat_values):
    plt.annotate(f'{l:.2e}', (r, l), textcoords="offset points", xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('large_latency_vs_ranks.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'large_latency_vs_ranks.png'")

bw_ranks = [ranks[i] for i, b in enumerate(bandwidth) if b is not None]
bw_values = [b for b in bandwidth if b is not None]

plt.figure(figsize=(10, 6))
plt.plot(bw_ranks, bw_values, marker='o', linewidth=2, markersize=8, color='blue', label='Bandwidth')
plt.xlabel('Number of Ranks', fontsize=12)
plt.ylabel('Bandwidth (MB/s)', fontsize=12)
plt.title('MPI Bandwidth vs Number of Ranks', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.xticks(ranks[1:])
plt.legend(fontsize=10)

for r, b in zip(bw_ranks, bw_values):
    plt.annotate(f'{b:.2f}', (r, b), textcoords="offset points", xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('large_bandwidth_vs_ranks.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'large_bandwidth_vs_ranks.png'")

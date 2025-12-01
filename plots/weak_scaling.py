import matplotlib.pyplot as plt

# Weak scaling data for medium file
ranks = [2, 4, 8]
avg_times = [0.005124, 0.008419, 0.012146]  # average time from your runs

plt.figure(figsize=(10, 6))
plt.plot(ranks, avg_times, marker='o', linewidth=2, markersize=8, label='Avg Runtime')

plt.xlabel('Number of MPI Ranks', fontsize=12)
plt.ylabel('Average Runtime (seconds)', fontsize=12)
plt.title('Weak Scaling: Medium File (1 MB per rank)', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.xticks(ranks)
plt.legend(fontsize=10)

# Annotate points
for r, t in zip(ranks, avg_times):
    plt.annotate(f'{t:.3f}s', (r, t), textcoords="offset points", xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('weak_scaling_medium.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'weak_scaling_medium.png'")

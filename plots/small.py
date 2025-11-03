import matplotlib.pyplot as plt
import numpy as np

threads = [1, 2, 4, 8]
speedup = [0.9060402685, 1.8, 2.8125, 3.698630137]
efficiency = [0.9060402685, 0.9, 0.703125, 0.4623287671]
plt.figure(figsize=(10, 6))
plt.plot(threads, speedup, marker='o', linewidth=2, markersize=8, label='Actual Speedup')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup vs Number of Threads (Small File)', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.xticks(threads)

for i, (t, s) in enumerate(zip(threads, speedup)):
    plt.annotate(f'{s:.2f}', (t, s), textcoords="offset points", 
                xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('speedup_small_file.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'speedup_small_file.png'")

# Efficiency Plot
plt.figure(figsize=(10, 6))
plt.plot(threads, efficiency, marker='o', linewidth=2, markersize=8, label='Actual Efficiency', color='green')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Efficiency', fontsize=12)
plt.title('Efficiency vs Number of Threads (Small File)', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.xticks(threads)
plt.ylim(0, 1.1)  

for i, (t, e) in enumerate(zip(threads, efficiency)):
    plt.annotate(f'{e:.2f}', (t, e), textcoords="offset points",xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('efficiency_small_file.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'efficiency_small_file.png'")

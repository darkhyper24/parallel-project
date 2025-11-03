import matplotlib.pyplot as plt
import numpy as np

threads = [1, 2, 4, 8]
speedup = [0.9851756955, 1.929390681, 3.92919708, 6.805309735]
efficiency = [0.9851756955, 0.9646953405, 0.9822992701, 0.8506637168]
plt.figure(figsize=(10, 6))
plt.plot(threads, speedup, marker='o', linewidth=2, markersize=8, label='Actual Speedup')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup vs Number of Threads (medium File)', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.xticks(threads)

for i, (t, s) in enumerate(zip(threads, speedup)):
    plt.annotate(f'{s:.2f}', (t, s), textcoords="offset points", 
                xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('speedup_medium_file.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'speedup_medium_file.png'")

# Efficiency Plot
plt.figure(figsize=(10, 6))
plt.plot(threads, efficiency, marker='o', linewidth=2, markersize=8, label='Actual Efficiency', color='green')

plt.xlabel('Number of Threads', fontsize=12)
plt.ylabel('Efficiency', fontsize=12)
plt.title('Efficiency vs Number of Threads (medium File)', fontsize=14, fontweight='bold')
plt.grid(True, alpha=0.3)
plt.legend(fontsize=10)
plt.xticks(threads)
plt.ylim(0, 1.1)  

for i, (t, e) in enumerate(zip(threads, efficiency)):
    plt.annotate(f'{e:.2f}', (t, e), textcoords="offset points",xytext=(0,10), ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('efficiency_medium_file.png', dpi=300, bbox_inches='tight')
plt.show()
print("Plot saved as 'efficiency_medium_file.png'")

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import json
import glob
import os

# ——— Config ———
indir   = "data/"
outdir  = "plot/"
ext     = "png"

# cpu_info = "(2x) Intel Xeon Platinum 8360Y CPUs 2.40GHz (36) @ "
# gpu_info = "NVIDIA A100-40GB Tensor Core GPU"

cpu_info = "13th Gen Intel i5-1335U (12) @ 4.600GHz"
gpu_info = "Intel Raptor Lake-P [Iris Xe Graphics] "

algorithms = ['votess (CPU)', 'votess (GPU)', 'voro++', 'CGAL']

stub_map   = {
    algorithms[0] : 'votess-cpu',
    algorithms[1] : 'votess-gpu',
    algorithms[2] : 'voropp',
    algorithms[3] : 'cgal'
}

colors = {
    algorithms[0] : '#BB5566',
    algorithms[1] : '#004488',
    algorithms[2] : '#DDAA33',
    algorithms[3] : '#000000'
}

data = {}
N    = None

for algo in algorithms:
    stub = stub_map[algo]
    pattern = os.path.join(indir, f"profile__{stub}__*.json")
    matches = glob.glob(pattern)
    if not matches:
        raise FileNotFoundError(f"No files matching {pattern}")
    latest = max(matches, key=os.path.getmtime)
    with open(latest, 'r') as f:
        jd = json.load(f)
    if N is None:
        N = jd['metadata']['N']
    data[algo] = jd[algo]


means, stds = {}, {}
nmeans, nstds = {}, {}

for algo in algorithms:
    arrs = [np.array(data[algo].get(f"run:{n}", [])) for n in N]
    means[algo] = [a.mean() for a in arrs]
    stds[algo]  = [a.std()  for a in arrs]
    nmeans[algo]= [ (a/n).mean() for a,n in zip(arrs, N) ]
    nstds[algo] = [ (a/n).std()  for a,n in zip(arrs, N) ]

###############################################################################
### PLOT                                                                    ###
###############################################################################

gap  = 0.08
xmin = N[0] - N[0]*gap
xmax = N[-1] + N[-1]*gap

plt.style.use('classic')
plt.rcParams.update({'font.family':'serif','font.size':11})
os.makedirs(outdir, exist_ok=True)

# --------------------------------------------------------------------------- #
# Linear line plot                                                            #
# --------------------------------------------------------------------------- #

plt.figure(figsize=(10,8))
for algo in algorithms:
    plt.plot(N, means[algo], label=algo, color=colors[algo])
plt.grid(True, ls='--')
plt.xlim(xmin, xmax)
plt.xlabel('Dataset Size [N]')
plt.ylabel('Time [s]')
plt.legend(title='Algorithm', loc='lower right', prop={'size':9})
plt.text(0.02, 0.91,
         f"CPU: {cpu_info}\nGPU: {gpu_info}",
         transform=plt.gca().transAxes,
         fontsize=10, va='bottom', ha='left',
         bbox=dict(facecolor='white', alpha=0.5))
plt.savefig(os.path.join(outdir, f"linear.{ext}"), dpi=300)

# --------------------------------------------------------------------------- #
# loglog plot                                                                 #
# --------------------------------------------------------------------------- #
plt.figure(figsize=(10,8))
for algo in algorithms:
    plt.plot(N, means[algo], label=algo, color=colors[algo])
plt.grid(True, ls='--')
plt.xscale('log'); plt.yscale('log')
plt.xlim(xmin, xmax)
plt.xlabel('Dataset Size [N]')
plt.ylabel('Time [s]')
plt.legend(title='Algorithm', loc='lower right', prop={'size':9})
plt.text(0.02, 0.91,
         f"CPU: {cpu_info}\nGPU: {gpu_info}",
         transform=plt.gca().transAxes,
         fontsize=10, va='bottom', ha='left',
         bbox=dict(facecolor='white', alpha=0.5))
plt.savefig(os.path.join(outdir, f"loglog.{ext}"), dpi=300)

# --------------------------------------------------------------------------- #
# Bar plot at powers of 10                                                    #
# --------------------------------------------------------------------------- #

mask = np.isclose(np.log10(N), np.round(np.log10(N)))
N_pows = np.array(N)[mask]
x = np.arange(len(N_pows))
width = 0.8 / len(algorithms)

plt.figure(figsize=(10,8))
for i, algo in enumerate(algorithms):
    vals = np.array(means[algo])[mask]
    plt.bar(x[1:] + i*width, vals[1:] / N_pows[1:], width, label=algo, color=colors[algo])
plt.xlabel('Dataset Size [N]')
plt.ylabel('Time/Size [s][N]$^{-1}$')
plt.xticks(x + width*(len(algorithms)-1)/2, [int(n) for n in N_pows])
plt.legend(title='Algorithm', loc='upper left', prop={'size':9})
plt.grid(True, axis='y', ls='--')
plt.savefig(os.path.join(outdir, f"bar.{ext}"), dpi=300)

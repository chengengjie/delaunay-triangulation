#!/usr/bin/env python

import argparse, os
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Draw Nets/Trees')
parser.add_argument('input_file_name')
args = parser.parse_args()

# read
loci = []
adjLists = []
with open(args.input_file_name) as file:
    for line in file:
        tokens = line.split()
        x, y = float(tokens[1]), float(tokens[2])
        loci.append((x, y))
        adjLists.append([])
        for i in range(3, len(tokens)):
            adjLists[-1].append(int(tokens[i]))

# plot edges
for i in range(len(loci)):
    fromLoc = loci[i]
    for adj in adjLists[i]:
        toLoc = loci[adj]
        plt.plot([fromLoc[0], toLoc[0]], [fromLoc[1], toLoc[1]], 'k')

# plot vertices
for i in range(len(loci)):
    plt.plot(loci[i][0], loci[i][1], c='k', marker='o', mew=0, ms=3)

# format & save
plt.axis('square')
plt.axis('off')
base_name, _ = os.path.splitext(os.path.basename(args.input_file_name))
plt.savefig(base_name + '.pdf', bbox_inches='tight')
# plt.savefig(base_name + '.png', bbox_inches='tight')
#!/usr/bin/env python

import argparse, os
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Draw triangulation')
parser.add_argument('input_file_name')
args = parser.parse_args()

# read and draw
with open(args.input_file_name) as file:
    for line in file:
        tokens = line.split()
        if len(tokens) == 4:
            fromX, fromY, toX, toY = float(tokens[0]), float(tokens[1]), float(tokens[2]), float(tokens[3])
            plt.plot([fromX, toX], [fromY, toY], 'k', lw=1)

# format & save
plt.axis('square')
plt.axis('off')
base_name, _ = os.path.splitext(os.path.basename(args.input_file_name))
plt.savefig(base_name + '.pdf', bbox_inches='tight')
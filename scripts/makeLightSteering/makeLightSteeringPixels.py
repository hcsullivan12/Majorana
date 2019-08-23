#!/bin/python

'''
This script will read in a pixelization scheme and produce a
LightSteering file based on a subset of the pixels. The subset
is chosen to be the first quadrant. Assuming our disk is perfectly
symmetric, we can extrapolate to the other pixels later. This will 
decrease the amount of computation we have to do.
'''

import numpy as np
import matplotlib.pyplot as plt
import argparse
import math

parser = argparse.ArgumentParser(description="Make pixels")
parser.add_argument("-n", "--events", type=int, help="The number of events for each pixel", required=True)
parser.add_argument("-p", "--primaries", type=int, help="The number of primaries to emit", required=True)
parser.add_argument("-s", "--source", type=str, help="The pixelization file", required=True)
args = parser.parse_args()

events    = args.events
primaries = args.primaries
pixel_file = args.source

_pixels = dict()
with open(pixel_file) as pf:
    # skip header
    line = pf.readline()

    while True:
        line = pf.readline()
        lineVec = line.split()
        if len(lineVec) < 1: break
        _id = int(lineVec[0])
        _x  = float(lineVec[1])
        _y  = float(lineVec[2])
        if _x >= 0 and _y >= 0:
            _pixels[_id] = [_x, _y]

with open('./LightSourceSteering.txt', 'w') as lf:
    # write header
    lf.write('pixelID n\n')
    for p,v in _pixels.iteritems():
        for evt in range(0, events):
            lf.write(str(p)+' '+str(primaries)+'\n')

_x, _y = [], []
for k,v in _pixels.iteritems():
    _x.append(v[0])
    _y.append(v[1])

plt.scatter(_x, _y, marker='s')        
plt.show()
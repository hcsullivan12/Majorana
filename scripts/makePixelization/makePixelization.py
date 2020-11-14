#!/bin/python

import numpy as np
import matplotlib.pyplot as plt
import argparse
import math


parser = argparse.ArgumentParser(description="Make pixels")
parser.add_argument("-r", "--diskradius", type=float, help="The disk radius", required=True)
parser.add_argument("-s", "--spacing", type=float, help="The pixel spacing", required=True)
args = parser.parse_args()
diskradius = args.diskradius
spacing    = args.spacing

# Make a list of xAxis pixels, and then translate
xAxisPixelPos = []
# Start at center and increase x
x, y = 0, 0
while x < diskradius:
    #print x,y
    xAxisPixelPos.append([x,y])
    x = x + spacing
# decrease x
x, y = 0 - spacing, 0
while x > -1*diskradius:
    #print x,y
    xAxisPixelPos.append([x,y])
    x = x - spacing

# write to our files
pixels = []
s = 'pixelization'+str(spacing)+'.txt'
with open(s, 'w') as pixf:
    # header
    pixf.write('pixelID x y\n')
    # Now shift
    pixelID = 1
    y = 0
    while y < diskradius:
        for p in xAxisPixelPos:
            # pixelization
            thisX = p[0]
            thisY = p[1] + y
            r = math.sqrt(thisX*thisX + thisY*thisY)
            if r >= diskradius: continue
            pixf.write(str(pixelID)+ ' '+str(thisX)+' '+str(thisY)+'\n')
            pixels.append([thisX, thisY])
            pixelID = pixelID + 1
        y = y + spacing
    y = 0 + spacing
    while y < diskradius:
        for p in xAxisPixelPos:
            # pixelization
            thisX = p[0]
            thisY = p[1] - y
            r = math.sqrt(thisX*thisX + thisY*thisY)
            if r >= diskradius: continue
            pixf.write(str(pixelID)+ ' '+str(thisX)+' '+str(thisY)+'\n')
            pixels.append([thisX, thisY])
            pixelID = pixelID + 1
        y = y + spacing

pixf.close()
x, y = [], []
print "Created ", len(pixels), "pixels"
for pixel in pixels:
    x.append(pixel[0])
    y.append(pixel[1])

plt.scatter(x, y, marker='s')        
plt.show()




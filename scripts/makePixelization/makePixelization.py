#!/bin/python

import numpy as np
import matplotlib.pyplot as plt
import argparse
import math

# current configuration
#diskradius = 14.5
#spacing    = 1.0
#nevents    = 10
#nprimaries      = 50000

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Make pixels")
    parser.add_argument("-r", "--diskradius", type=float, default=None, help="The disk radius")
    parser.add_argument("-s", "--spacing", type=float, default=None, help="The pixel spacing")
    parser.add_argument("-n", "--nevents", type=int, default=1, help="The number of events to simulate for this pixel")
    parser.add_argument("-p", "--nprimaries", type=int, default=1, help="The number of primaries for each event")
    args = parser.parse_args()

    diskradius = args.diskradius
    spacing    = args.spacing
    nevents    = args.nevents
    nprimaries = args.nprimaries
    

    # Make a list of xAxis pixels, and then translate
    xAxisPixelPos = []
    # Start at center and increase x
    x = 0
    y = 0
    while x < diskradius:
        #print x,y
        xAxisPixelPos.append([x,y])
        x = x + spacing
    # decrease x
    x = 0 - spacing
    y = 0
    while x > -1*diskradius:
        #print x,y
        xAxisPixelPos.append([x,y])
        x = x - spacing

    # write to our files
    pixels = []
    s = 'pixelization'+str(spacing)+'.txt'
    with open('./LightSourceSteering.txt', 'w') as lssf, open(s, 'w') as pixf:
        # header
        lssf.write('pixelID n\n')
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
                # light source generator 
                event = 1
                while event <= nevents:
                    lssf.write(str(pixelID)+' '+str(nprimaries)+'\n')
                    event = event + 1
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
                # light source generator
                event = 1
                while event <= nevents:
                    lssf.write(str(pixelID)+' '+str(nprimaries)+'\n')
                    event = event + 1
                pixelID = pixelID + 1
            y = y + spacing

    lssf.close()
    pixf.close()

    x = []
    y = []
    print "Created ", len(pixels), "pixels"
    for pixel in pixels:
        x.append(pixel[0])
        y.append(pixel[1])

    plt.scatter(x, y, marker='s')        
    #plt.show()




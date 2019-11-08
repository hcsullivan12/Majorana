#!/bin/python

import argparse
import random

def getEntry(diskradius, nprim):
    x = random.uniform(-diskradius, diskradius)
    y = random.uniform(-diskradius, diskradius)
    n = random.uniform(nprim-20000, nprim+20000)

    r = (x*x+y*y)**0.5
    if r >= (diskradius-1): 
      return getEntry(diskradius, nprim)
    else:
      return x,y,n

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Make pixels")
    parser.add_argument("-r", "--diskradius", type=float, help="The disk radius", required=True)
    parser.add_argument("-n", "--npos", type=int, default=1, help="The number of events to simulate for this pixel")
    parser.add_argument("-p", "--nprimaries", type=int, default=1, help="The number of primaries for each event")
    args = parser.parse_args()

    diskradius = args.diskradius
    npos       = args.npos
    nprimaries = args.nprimaries

    with open('LightSourceSteering.txt', 'w') as f:
        random.seed()

        f.write('x y n\n')
        for p in range(1,npos):
            x,y,n = getEntry(diskradius, nprimaries)
            f.write(str(x)+' '+str(y)+' '+str(n)+'\n')

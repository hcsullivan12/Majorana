#!/bin/python

import argparse
import random

def getPosition(diskradius):
    x = random.uniform(-diskradius, diskradius)
    y = random.uniform(-diskradius, diskradius)

    r = (x*x+y*y)**0.5
    if (r >= (diskradius-1) or r <=0.3*diskradius): getPosition(diskradius)
    return x,y


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
            x,y = getPosition(diskradius)
            f.write(str(x)+' '+str(y)+' '+str(nprimaries)+'\n')
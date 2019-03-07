#!/bin/python

import os
import argparse

if __name__== "__main__":
    parser = argparse.ArgumentParser(description="Make pixels")
    parser.add_argument("-s", "--start", type=float, default=None, help="Starting point")
    parser.add_argument("-e", "--end", type=float, default=None, help="Ending point")
    parser.add_argument("-i", "--increment", type=float, default=None, help="Increment size from start to end")
    parser.add_argument("-d", "--diskradius", type=float, default=None, help="Disk radius")
    args = parser.parse_args()

    start = args.start
    end   = args.end
    inc   = args.increment
    diskradius = args.diskradius

    it = start
    while it <= end:
        s = 'python makePixelization.py -r '+str(diskradius)+' -s '+str(it)
        os.system(s)
        it = it + inc


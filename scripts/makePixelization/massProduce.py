#!/bin/python

import os
import argparse

if __name__== "__main__":
    parser = argparse.ArgumentParser(description="Make pixels")
    parser.add_argument("-s", "--start", type=float, help="Starting point", required=True)
    parser.add_argument("-e", "--end", type=float, help="Ending point", required=True)
    parser.add_argument("-i", "--increment", type=float, help="Increment size from start to end", required=True)
    parser.add_argument("-d", "--diskradius", type=float, help="Disk radius", required=True)
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



import matplotlib.pyplot as plt
import argparse
import math

parser = argparse.ArgumentParser(description="Make pixels")
parser.add_argument("-n", "--sipms", type=int, help="The number of sipms", required=True)
parser.add_argument("-m", "--mark", type=int, help="Highlighted sipm", required=True)
args = parser.parse_args()
assert(args.mark <= args.sipms and args.mark > 0)

_x,_y,_c = list(), list(), list()
for sid in range(0,args.sipms):
    theta = 2.*math.pi*sid/args.sipms
    #print theta, sid, args.sipms, 1.0*sid/args.sipms
    _x.append(math.cos(theta))
    _y.append(math.sin(theta))
    _c.append('b')

_c[args.mark-1]='r'
plt.scatter(_x, _y, c=_c, marker='s')
plt.show()

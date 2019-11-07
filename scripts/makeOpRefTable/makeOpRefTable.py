import ROOT
import numpy as np
import matplotlib.pyplot as plt
from matplotlib  import cm
import argparse
import math

parser = argparse.ArgumentParser(description="Make pixels")
parser.add_argument("-s", "--source", type=str, help="The sim ntuple file", required=True)
parser.add_argument("-p", "--pixel", type=str, help="The pixelization file", required=True)
parser.add_argument("-o", "--output", type=str, help="Lookup table", required=True)
args = parser.parse_args()

# read in pixels
_pixels = dict()
with open(args.pixel) as pf:
    line = pf.readline()
    while True:
        line = pf.readline()
        lineVec = line.split()
        if len(lineVec) < 1:break
        _pixels[int(lineVec[0])] = [float(lineVec[1]), float(lineVec[2])]

print 'Found', len(_pixels), 'pixels...\n'

# method for finding pid, temporary
def getPixelId(x, y):
    mindist = 100000
    minId=-1
    for k,v in _pixels.iteritems():
        distx = x - v[0]
        disty = y - v[1]
        r = (distx**2 + disty**2)**0.5
        if r < mindist:
            mindist=r
            minId=k
    return minId


# initialize data container
_data = dict()
for k,v in _pixels.iteritems():
    _data[k] = list()

f = ROOT.TFile.Open(args.source)
anatree = f.Get('anatree')
_nsipms = 0
temp_x, temp_y, temp_z = [], [], []
for ent in anatree:
    _nsipms = ent.nMPPCs
    pos = ent.sourcePosXYZ
    # ignore pixels on y axis, excluding origin pixel
    if pos[0]<0.0001 and pos[1]>0.0001: continue
    temp_x.append(pos[0])
    temp_y.append(pos[1])
    # which pixel is this?
    pid = getPixelId(pos[0], pos[1])
    if pid<0:
        print 'WARNING: PID'
    ly = {sid:c for sid,c in enumerate(ent.mppcToLY, 1)}
    _data[pid].append(ly)
    temp_z.append(math.log(ly[17]/100000.))
    #print _data[pid][0][63]

#fig=plt.figure(0)
#ax=fig.add_subplot(111)
#img = ax.scatter(temp_x, temp_y, c=temp_z, s=80, marker=',', cmap='gist_stern')
#fig.colorbar(img)
#plt.show()

_lookup_table = dict()
for pid,eventlist in _data.iteritems():
    if len(eventlist) == 0:
        continue
    # average over the events
    avg_data = {sid:0 for sid,v in eventlist[0].iteritems()}
    n_events = len(eventlist)
    #print(n_events)
    for eventdata in eventlist:
        for sid,nph in eventdata.iteritems():
            avg_data[sid] += nph
    for sid,v in avg_data.iteritems():
        avg_data[sid]/= n_events
    # this must be a new entry!
    assert(_lookup_table.get(pid) is None)
    _lookup_table[pid] = avg_data

# extrapolate to the other quadrants
# make copy
_quad_lkt = dict(_lookup_table)
for pid, lkt, in _quad_lkt.iteritems():
    x,y = _pixels[pid][0], _pixels[pid][1]
    # ignore the origin, we already have it
    if abs(x)<0.0001 and abs(y)<0.0001:continue
    #if abs(x)<0.0001:continue
    q2_pid, q3_pid, q4_pid = getPixelId(-1*y, x), getPixelId(-1*x, -1*y), getPixelId(y, -1*x)
    #print x,y,pid,q2_pid,q3_pid,q4_pid

    # we should not have seen these ids yet!
    assert(_lookup_table.get(q2_pid) is None and 'Q2')
    _lookup_table[q2_pid] = dict()
    assert(_lookup_table.get(q3_pid) is None and 'Q3')
    _lookup_table[q3_pid] = dict()
    assert(_lookup_table.get(q4_pid) is None and 'Q4')
    _lookup_table[q4_pid] = dict()

    q2_skip, q3_skip, q4_skip = _nsipms/4, 2*_nsipms/4, 3*_nsipms/4
    for sid,v in lkt.iteritems():
        # rotating reference frame by 90, 180, and 270
        q2_eq_sid = sid+q2_skip if sid+q2_skip<=_nsipms else sid+q2_skip-_nsipms
        q3_eq_sid = sid+q3_skip if sid+q3_skip<=_nsipms else sid+q3_skip-_nsipms
        q4_eq_sid = sid+q4_skip if sid+q4_skip<=_nsipms else sid+q4_skip-_nsipms
        #print sid, q2_eq_sid, q3_eq_sid, q4_eq_sid

        # these should be new sids for this pixel
        assert(_lookup_table[q2_pid].get(q2_eq_sid) is None)
        _lookup_table[q2_pid][q2_eq_sid]=v
        assert(_lookup_table[q3_pid].get(q3_eq_sid) is None)
        _lookup_table[q3_pid][q3_eq_sid]=v
        assert(_lookup_table[q4_pid].get(q4_eq_sid) is None)
        _lookup_table[q4_pid][q4_eq_sid]=v

_x, _y, _z = list(), list(), list()
for pid,lkt in _lookup_table.iteritems():
    if len(lkt) == 0:
        continue
    _x.append(_pixels[pid][0])
    _y.append(_pixels[pid][1])
    #print lkt[17]
    _z.append(math.log(lkt[17]/100000.))
    #print lkt[19]

fig=plt.figure(0)
ax=fig.add_subplot(111)
img = ax.scatter(_x, _y, c=_z, s=80, marker=',', cmap='gist_stern')
fig.colorbar(img)
plt.show()

# write to file
with open(args.output, 'w') as of:
    of.write('pixelID mppcID probability\n')
    for pid,lkt in _lookup_table.iteritems():
        for sid,p in lkt.iteritems():
            of.write(str(pid)+' '+str(sid)+' '+str(p/100000.)+'\n')


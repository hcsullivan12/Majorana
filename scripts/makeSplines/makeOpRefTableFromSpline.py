#!/bin/python

'''
The purpose of this script is to generate a new opReferenceTable for
some pixelization scheme by interpolating the splineKnot file generated from 
a production simulation.
The procedure:
    1) Read in spline and pixelization
    2) Interpolate onto pixel grid
    3) Output new opReferenceTable
'''

import argparse
import scipy
from scipy import interpolate
import numpy as np
import matplotlib.pyplot as plt

class fileReader():
	
	def __init__(self, splineFile, pixelFile):
		assert(splineFile is not None)
		assert(pixelFile is not None)
		self._splineFile  = splineFile
		self._pixelFile   = pixelFile
		self._splineTable = []
		self._pixelTable  = {}
		self._map = {}
		self.count = 0

		self.parsePixelFile()
		self.parseSplineFile()
		self.makeMap()
		
	def theSplineTable(self):
		return self._splineTable

	def thePixelTable(self):
		return self._pixelTable		

	def theMap(self):
		return self._map

	def parsePixelFile(self):
		print "\nParsing pixelization file"
		with open(self._pixelFile) as f:
			# skip first line
			line = f.readline()
			while True:
				# the scheme is:
				# 	pixelID x y
				line    = f.readline()
				lineVec = line.split()
				if len(lineVec) < 1: break
				self._pixelTable[int(lineVec[0])] = [float(lineVec[1]), float(lineVec[2])]
				if float(lineVec[2]) == 0: self.count = self.count+1

	def parseSplineFile(self):
		print "Parsing spline"
		# Open file for reading
		with open(self._splineFile) as f:
			# skip first line
			line = f.readline()
			while True:
				# the scheme is:
				# 	pixelID sipm prob
				line    = f.readline()
				lineVec = line.split()
				if len(lineVec) < 1: break
				data = [int(lineVec[0]), int(lineVec[1]), float(lineVec[2])]
				self._splineTable.append(data)

	def makeMap(self):
		for pid, sid, p in self._splineTable:
			# what's the x y position for this pid
			xy = self._pixelTable[pid]
			if sid not in self._map: self._map[sid] = []
 			theList = self._map[sid]
 			theList.append([xy[0], xy[1], p])
 			self._map[sid] = theList	

def interpolate(theMap):
    # initialize our arrays
    xs, ys, ps, tempCont = ([] for i in range(4))
    eMap = theMap[1]
    tempX = eMap[0][0]

    for x,y,p in eMap:
        if x != tempX: tempCont.append(x)
        xs.append(x)
        ys.append(y)
        ps.append(p)	
    diffX = [np.absolute(tempX - t) for t in tempCont]
    pixelSpacing = min(diffX)
    xArr = np.array(xs)
    yArr = np.array(ys)
    zArr = np.array(ps)

    #Z = scipy.interpolate.griddata(xArr, yArr, zArr, method='cubic')
    #f = scipy.interpolate.interp2d(xArr,yArr,zArr,kind='cubic')
    f_interp = scipy.interpolate.Rbf(xArr, yArr, zArr, function='cubic')
    #plt.pcolor(XI, YI, zArr, cmap=cm.jet)
    plt.pcolor(xArr, yArr, f_interp, cmap='gist_heat')
    plt.colorbar()	
    plt.show()

def writeNewOpRefTable(nsipms, ourMap, thePixelTable, theSplineTable):
    # we should now have the spline profile
    # for sipm1 (east)
    # Looping through each sipm and pixel:
    #   1) Calculate R,alpha in the sipm's coordinate
    #      system (distanceFrom, angleToNormal)
    #   2) Calculate p from our profile at (R,alpha)
    
    theRefTable = []

    beta = 2*math.pi/nsipms
    for sid in range(nsipms):
        sipmAngle = (sid-1)*beta
        for pid, xy in thePixelTable.items():
            r = math.sqrt(xy[0]*xy[0]+xy[1]*xy[1])
            theta = math.atan2(xy[1]/xy[0])
            if theta < 0: theta = theta + 2*math.pi

            # the 'rotated' positions
            theX = r*cos(theta - sipmPos[1])
            theY = r*sin(theta - sipmPo[1])                
            # what is p for this position?
            p = ourMap(theX, theY)

            theRefTable.append([pid, sid, p])

if __name__ == "__main__":
	# first read the args
    parser = argparse.ArgumentParser(description="Make OpReferenceTable from Spline")
    parser.add_argument("-s", "--spline", default=None, help="The input spline")
    parser.add_argument("-p", "--pixelization", default=None, help="The pixelization scheme")
    parser.add_argument("-o", "--output", default=None, help="The output OpRefTable")
    parser.add_argument("-n", "--nsipms", default=None, help="The output OpRefTable")
    args = parser.parse_args()

    # Step 1)
    fr = fileReader(args.spline, args.pixelization)
	# Step 2)
    ourMap = interpolate(fr.theMap())
    # Step 3)
    #writeNewOpRefTable(args.nsipms, ourMap, fr.thePixelTable(), fr.theSplineTable())
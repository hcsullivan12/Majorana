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
import math

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
		print "\nParsing pixelization..."
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
		print "Parsing spline..."
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
				data = [int(lineVec[0]), float(lineVec[1]), float(lineVec[2]), int(lineVec[3]), float(lineVec[4])]
				self._splineTable.append(data)

	def makeMap(self):
		for pid, x, y, sid, p in self._splineTable:
			# what's the x y position for this pid
			if sid not in self._map: self._map[sid] = []
 			theList = self._map[sid]
 			theList.append([x, y, p])
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
    #zArr = np.log10(zArr)

    interp = scipy.interpolate.Rbf(xArr, yArr, zArr, function='cubic')
    '''
    ti = np.linspace(-14.5,14.5,num=57)
    xx, yy = np.meshgrid(ti, ti)
    zz = interp(xx,yy)
    plt.pcolor(xx, yy, zz, cmap='gist_stern')
    plt.colorbar()	
    plt.show()
    '''
    return interp

def writeNewOpRefTable(nsipms, outputFile, theFunc, thePixelTable, theSplineTable):
    # we should now have the spline profile
    # for sipm1 (east).
    # Looping through each sipm and pixel:
    #   1) Calculate R,alpha in the sipm's coordinate
    #      system (distanceFrom, angleToNormal)
    #   2) Calculate p from our profile at (R,alpha)
    
    theRefTable = []
    beta = 2*math.pi/nsipms
    for pid, xy in thePixelTable.items():
        #print pid, '/', len(thePixelTable)

        # convert to r, theta
        r = math.sqrt(xy[0]*xy[0]+xy[1]*xy[1])
        if xy[0] == 0. and xy[1] == 0.: theta = 0
        else: theta = math.atan2(xy[1], xy[0])
        if theta < 0: theta = theta + 2*math.pi

        for sid in range(1, nsipms+1):
            sipmAngle = (sid-1)*beta
            # the 'rotated' positions
            theX = r*math.cos(theta - sipmAngle)
            theY = r*math.sin(theta - sipmAngle)                
            # what is p for this position?
            p = theFunc(theX, theY)
            theRefTable.append([pid, sid, p])

    with open(outputFile, 'w') as f:
        # write first line
        f.write('pixelID mppcID probability\n')
        for d in theRefTable:
			s = str(d[0])+' '+str(d[1])+' '+str(d[2])+'\n'
			f.write(s) 

if __name__ == "__main__":
	# first read the args
    parser = argparse.ArgumentParser(description="Make OpReferenceTable from Spline")
    parser.add_argument("-s", "--spline", help="The input spline", required=True)
    parser.add_argument("-p", "--newpixelization", help="The pixelization scheme", required=True)
    parser.add_argument("-o", "--output", help="The output OpRefTable", required=True)
    parser.add_argument("-n", "--nsipms", help="The number of sipms", required=True)
    args = parser.parse_args()

    # Step 1)
    fr = fileReader(args.spline, args.newpixelization)
    print 'Making opRefTable for', args.nsipms, 'sipms and', len(fr.thePixelTable()), 'pixels...\n'
	# Step 2)
    theFunc = interpolate(fr.theMap())
    # Step 3)
    writeNewOpRefTable(int(args.nsipms), str(args.output), theFunc, fr.thePixelTable(), fr.theSplineTable())
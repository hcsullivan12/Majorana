#!bin/python

'''
The purpose of this script is to read in the OpReferenceTable for
some pixelization scheme (ideally small pixel spacing), and produce 
a single light detection probability profile in 2D. 
The procedure:
	1) Read in OpRefTable and pixelization
	2) Average profiles for "4 corners" of the disk (since the mapping is trivial)
	3) Blur the 2D profiles?
	4) Produce a splineKnot file where now the pixel IDs and 
	   corresponding probabilities are spline knots.	   
	   The splineKnot file can be used to interpolate to a different pixelization
	   scheme. This way, long production simulations have to be done only when 
	   there is a change to the geometry or optical effects. 
'''

import numpy as np
import scipy.ndimage
import matplotlib.pyplot as plt
import argparse

thePixels = []

class fileReader():
	
	def __init__(self, opRefFile, pixelFile):
		assert(opRefFile is not None)
		assert(pixelFile is not None)
		self._opRefFile = opRefFile
		self._pixelFile = pixelFile
		self._opRefTable = []
		self._pixelTable = {}
		self._map = {}
		self.count = 0

		self.parsePixelFile()
		self.parseOpRefFile()
		self.makeMap()
		
	def theOpRefTable(self):
		return self._opRefTable

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

	def parseOpRefFile(self):
		print "Parsing opRefTable"
		# Open file for reading
		with open(self._opRefFile) as f:
			# skip first line
			line = f.readline()
			while True:
				# the scheme is:
				# 	pixelID sipm prob
				line    = f.readline()
				lineVec = line.split()
				if len(lineVec) < 1: break
				data = [int(lineVec[0]), int(lineVec[1]), float(lineVec[2])]
				self._opRefTable.append(data)

	def makeMap(self):
		for pid, sid, p in self._opRefTable:
			# what's the x y position for this pid
			xy = self._pixelTable[pid]
			if sid not in self._map: self._map[sid] = []
 			theList = self._map[sid]
 			theList.append([xy[0], xy[1], p])
 			self._map[sid] = theList

def makeImage(theMap, thePixelTable, theCount):
	# initialize our arrays
	xs, ys, ps, tempCont = ([] for i in range(4))
	tempX = theMap[0][0]
	print theMap
	for x,y,p in theMap:
		if x != tempX: tempCont.append(x)
		xs.append(x)
		ys.append(y)
		ps.append(p)	
	diffX = [np.absolute(tempX - t) for t in tempCont]
	pixelSpacing = min(diffX)
	xArr = np.array(xs)
	yArr = np.array(ys)
	zArr = np.array(ps)

	# form our pixelization grid
	pixelGrid = []
	for k,v in thePixelTable.items():
		pixelGrid.append(v)	

	# form the full square grid
	zeroGridX = np.arange(xArr.min(), xArr.max()+pixelSpacing, pixelSpacing)
	zeroGridY = np.arange(yArr.min(), yArr.max()+pixelSpacing, pixelSpacing)
	zeroGrid = []
	for x in zeroGridX:
		for y in zeroGridY:
			zeroGrid.append([x,y])	
	for p in zeroGrid:
		if p in pixelGrid: continue			
		xArr = np.append(xArr, p[0])
		yArr = np.append(yArr, p[1])
		zArr = np.append(zArr, 1)
	# convert z to logz
	zArr = np.log(zArr)

	# convert to grid coordinates
	pixels = []
	for x,y,z in zip(xArr,yArr,zArr):
		pixels.append([x,y,z])

	def sortX(val): return val[0]
	def sortY(val): return val[1]		
	pixels.sort(key=sortX)
	pixels.sort(key=sortY,reverse=True)

	# for future use
	global thePixels 
	thePixels = pixels

	theX, theY = np.mgrid[xArr.min():xArr.max():(theCount*1j), yArr.min():yArr.max():(theCount*1j)]
	theZ = np.zeros_like(theX)
	r = 0
	c = 0
	for p in pixels:
		if c == theCount: 
			c = 0
			r+=1
		theZ[r][c] = p[2]
		c+=1
			
	return theZ

def averageRefTable(theMapToAvg, theCount, thePixelTable):
	# we're taking the average of the log(prob)

	# how many sipms did we pass?
	# we're making an assumption that nSiPMs = 2^n
	nSiPMs = len(theMapToAvg)
	n = nSiPMs/4
	theCorners = [1]
	for c in range(0,3):
		theCorners.append(theCorners[c]+n)
	print "Averaging SiPMs ", theCorners

	# get the corner maps
	eMap = theMapToAvg[theCorners[0]]
	nMap = theMapToAvg[theCorners[1]]
	wMap = theMapToAvg[theCorners[2]]
	sMap = theMapToAvg[theCorners[3]]

	# make image
	eIm = makeImage(eMap, thePixelTable, theCount)
	nIm = makeImage(nMap, thePixelTable, theCount)
	wIm = makeImage(wMap, thePixelTable, theCount)
	sIm = makeImage(sMap, thePixelTable, theCount)

	# "rotate"
	nIm = np.rot90(nIm, 3)
	wIm = np.rot90(wIm, 2)
	sIm = np.rot90(sIm, 1)

	# avg
	avgIm = (eIm+nIm+wIm+sIm)/4.
	return avgIm

def writeNewOpRefTable(avgMap, pixelTable, outputFile):
	assert(outputFile is not None)

	# we have to convert from grid coordinates to pixel coordinates
	for ((c,r), p), pixel in zip(np.ndenumerate(avgMap), thePixels):
		pixel[2] = np.exp(p)

	# ignore the pixels outside our ROI
	newPixels = {}
	for testPixel in thePixels:
		testX = testPixel[0]
		testY = testPixel[1]
		testP = testPixel[2]
		# find the corresponding id
		for thePID, theXY in pixelTable.items():
			if theXY[0] == testX and theXY[1] == testY:
				newPixels[thePID] = [theXY[0], theXY[1], testP]				

	# now thePixels have the have prob
	with open(outputFile, 'w') as f:
		# write first line
		f.write('pixelID mppcID probability\n')

		for pid in sorted(newPixels.iterkeys()):
			s = str(pid) + ' 1 ' + str(newPixels[pid][2]) + '\n'
			f.write(s) 

if __name__ == "__main__":
	# first read the input op ref table
	parser = argparse.ArgumentParser(description="Make Spline from OpReferenceTable")
	parser.add_argument("-orf", "--opreftable", default=None, help="The input OpRefTable")
	parser.add_argument("-p", "--pixelization", default=None, help="The pixelization scheme")
	parser.add_argument("-o", "--output", default=None, help="The output OpRefTable")
	args = parser.parse_args()

	# Step 1)
	fr = fileReader(args.opreftable, args.pixelization)
	print "Done"

	# Step 2)
	avgMap = averageRefTable(fr.theMap(), fr.count, fr.thePixelTable())
	plt.imshow(avgMap, interpolation='nearest', cmap='gist_heat')
	plt.colorbar()	
	plt.show()

	# Step 3)
	writeNewOpRefTable(avgMap, fr.thePixelTable(), args.output)





    


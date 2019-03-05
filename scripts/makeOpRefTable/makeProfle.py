#! bin/python

import sys

class profileMaker():
	
	def __init__(self, inputFile):
		self._inputFile = inputFile

		self._map = []		

		self.parse()
		#self.printData()

	def parse(self):
		print "\nParsing data file\n"
		# Open file for reading
		with open(self._inputFile) as f:
			while True:
				# the scheme is:
				#  x y sipm prob
				line    = f.readline()
				lineVec = line.split()
				if len(lineVec) < 1: break
				
				sipm = lineVec[2]
				data = [ lineVec[0], lineVec[1], lineVec[3] ]
				self._map.append([sipm, data])

	def printData(self):
		print "In printData\n"
		for event in self._events:
			print "Run is ", event._run
			print "Subrun is ", event._subrun
			print "Event is ", event._event

			for point in event._data:
				print point


if __name__ == "__main__":
	file = "/home/hunter/projects/Majorana/analysis/03_03/2617p_128s_opReferenceTable.txt"
	pMaker = profileMaker(file)
	print "Done"


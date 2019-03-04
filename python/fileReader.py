import tpc_event
import sys

class fileReader():
	
	def __init__(self, input_file):
		self._input_file = input_file

		self._events = []		

		self.parseFile()
		#self.printData()

	def parseFile(self):
		print "\nParsing data file\n"
		# Open file for reading
		with open(self._input_file) as f:

			line = f.readline()
			line_vec = line.split()
			
			# Get the first event information
			while True:
				if line_vec[0] == 'Run':
				
					# Create event
					_tpc_event = tpc_event.tpc_event()
					# Run
					run = line_vec[1]

					# Subrun
					line = f.readline()
					line_vec = line.split()
					subrun = line_vec[1]
					
					# Event
					line = f.readline()
                                        line_vec = line.split()
					event = line_vec[1]
					
					_tpc_event._run = run
					_tpc_event._subrun = subrun
					_tpc_event._event = event					

					while True:
						line = f.readline()
						if not line: 
							break
						line_vec = line.split()	
					
						if line_vec[0] == 'Run':
							break

						# Store in data list
						points = map(float, line_vec)
						_tpc_event._data.append(points)
						
                                        if not line:
                                        	break

					if len(_tpc_event._data) == 0: continue

					# Obtained all data for event, store in events list
					self._events.append(_tpc_event)

	def printData(self):
		print "In printData\n"
		for event in self._events:
			print "Run is ", event._run
			print "Subrun is ", event._subrun
			print "Event is ", event._event

			for point in event._data:
				print point

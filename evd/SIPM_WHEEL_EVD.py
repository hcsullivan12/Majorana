#!bin/python

import os
import argparse

# get this directory
current_dir = os.getcwd()

# get top majorana directory
top_dir = os.environ.get('MAJORANADIR')
if top_dir is None:
    print('Could not find environment variable \'MAJORANADIR\'. Please point \'MAJORANADIR\' to the top directory.')
    exit()

print('')
print('Running event display...')
print('')

parser = argparse.ArgumentParser(description="Mass produce opRefTables from splines")
parser.add_argument("-d", "--daq", type=str, help="Path to DAQ tree", required=True)
args = parser.parse_args()

command = 'root -l \'./evd/EventDisplay.C(\"'+top_dir+'\", \"'+args.daq+'\")\''
os.system(command)

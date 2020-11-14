#!/bin/python

import argparse
import os

# load list of root files 
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Analyze all root files")
    parser.add_argument("-i", "--input", type=str, help="Input list of root files", required=False,default="None")
    parser.add_argument("-o", "--output", type=str, help="Output path for histograms and counts", required=True)
    parser.add_argument("-p", "--pixel", type=str, help="Path to pixelization scheme", required=False)
    parser.add_argument("-l", "--lookup", type=str, help="Path to lookup table", required=False)
    parser.add_argument("-r", "--reco", type=int, help="Option to reconstruct (default = 0)", required=False, default=0)
    args = parser.parse_args()

    if args.reco is 1: assert(args.pixel and args.lookup)
    if args.input is not "None":
        # loop over the files and run the analyzer
        with open(args.input) as theFileList:
            for theFile in theFileList:
                theFile = theFile.replace('\r', '').replace('\n', '')
                theName = theFile.rsplit('/', 1)[1].rsplit('.', 1)[0]
                theCommand = 'root -l -b \'doAnalyze_v2.C(\"'+theFile+'\", \"'+args.output+'\", \"'+theName+'\")\''
                os.system(theCommand)


    # reconstruction option
    if args.reco is 1:
        theCommand = 'root -l -b \'doReconstruct.C(\"'+args.pixel+'\", \"'+args.lookup+'\", \"'+args.output+'\")\''
        os.system(theCommand)

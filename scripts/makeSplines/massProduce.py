#!/bin/python

import os
import argparse

if __name__== "__main__":

    parser = argparse.ArgumentParser(description="Mass produce opRefTables from splines")
    parser.add_argument("-sp", "--spline", type=str, help="Spline table", required=True)
    parser.add_argument("-r", "--root", type=str, help="Top directory for production. This will search through the subdirectories for pixelization.txt", required=True)
    parser.add_argument('-s','--sipms', nargs='+', type=int, help='List of the number of sipms', required=True)
    args = parser.parse_args()

    splinef = args.spline
    root    = args.root
    nsipmsList = args.sipms

    # assuming pixel spacing subdirectories 1mm, 2mm...
    spacingDirList = [dir for dir in os.listdir(root) if os.path.isdir(os.path.join(root,dir))]
    for spacingDir in spacingDirList:
        if spacingDir[-2:] != 'mm' and spacingDir[-2:] != 'cm': continue

        theSpacing = float(spacingDir[:-2])
        fullSpacingDir = os.path.join(root,spacingDir)
        sipmDirList = [dir for dir in os.listdir(fullSpacingDir) if os.path.isdir(os.path.join(fullSpacingDir,dir))]

        # check for pixelization
        files = os.listdir(fullSpacingDir)
        if 'pixelization.txt' not in files: continue
        thePixelPath = os.path.join(fullSpacingDir,'pixelization.txt')

        for sipmDir in sipmDirList:
            fullSipmPath = os.path.join(fullSpacingDir,sipmDir)
            
            theNumber = int(sipmDir[:-5])
            if theNumber not in nsipmsList: continue
            
            outputPath = os.path.join(fullSipmPath,'splinedOpRefTable.txt')
            command = 'python makeOpRefTableFromSpline.py -s '+str(splinef)+' -p '+str(thePixelPath)+' -o '+str(outputPath)+' -n '+str(theNumber)
            os.system(command)
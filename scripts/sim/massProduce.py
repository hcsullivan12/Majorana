#!/bin/python

import os
import argparse

if __name__== "__main__":

    # get top directory
    topDirPath = os.environ['MAJORANADIR']
    fullBuildPath = os.path.join(topDirPath,'build')

    parser = argparse.ArgumentParser(description="Running multiple simulations")
    parser.add_argument("-r", "--root", type=str, help="Top directory for production. This will search through the subdirectories for pixelization.txt.", required=True)
    parser.add_argument("-o", "--output", type=str, help="Top directory for output.", required=True)
    parser.add_argument("-s", "--sipms", nargs='+', type=int, help="List of the number of sipms", required=True)
    args = parser.parse_args()

    root    = args.root
    fullOutputPath = args.output
    nsipmsList = args.sipms

    # assuming pixel spacing subdirectories 1mm, 2mm... 
    spacingDirList = [dir for dir in os.listdir(root) if os.path.isdir(os.path.join(root,dir))]
    for spacingDir in spacingDirList:
        if spacingDir[-2:] != 'mm' and spacingDir[-2:] != 'cm': continue

        theSpacing = float(spacingDir[:-2])
        fullSpacingDir = os.path.join(root,spacingDir)
        sipmDirList = [dir for dir in os.listdir(fullSpacingDir) if os.path.isdir(os.path.join(fullSpacingDir,dir))]

        # check for pixelization
        files1 = os.listdir(fullSpacingDir)
        if 'pixelization.txt' not in files1: continue
        thePixelPath = os.path.join(fullSpacingDir,'pixelization.txt')

        for sipmDir in sipmDirList:
            fullSipmPath = os.path.join(fullSpacingDir,sipmDir)
            
            theNumber = int(sipmDir[:-5])
            if theNumber not in nsipmsList: continue

            # check for pixelization
            files2 = os.listdir(fullSipmPath)
            if 'splinedOpRefTable.txt' not in files2: continue
            theOpRefTPath = os.path.join(fullSipmPath,'splinedOpRefTable.txt')
            
            s1 = 'simulateOutput_'+str(spacingDir)+'_'+str(theNumber)+'sipms.root'
            s2 = 'recoAnaTree_'+str(spacingDir)+'_'+str(theNumber)+'sipms.root'
            simOutputPath  = os.path.join(fullOutputPath,s1)
            recoOutputPath = os.path.join(fullOutputPath,s2)
            command = str(fullBuildPath)+'/simulate --c '+str(topDirPath)+'/config/Configuration.ini --ov '+str(theNumber)+' '+str(thePixelPath)+' '+str(theOpRefTPath)+' '+str(simOutputPath)+' '+str(recoOutputPath)
            os.system(command)
import argparse
import include.analyzer as analyzer
import include.reconstructor as reconstructor
import ROOT

# load list of root files 
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Analyze root files")
    parser.add_argument("-i", "--input", type=str, help="Input list of root files", required=True)
    parser.add_argument("-o", "--output", type=str, help="Output path for histograms and reconstruction results", required=True)
    parser.add_argument("-c", "--config", type=str, help="Path to reconstruction configuration", required=True)
    args = parser.parse_args()

    # create output file
    f = ROOT.TFile.Open(args.output, 'RECREATE')
    f.Close()

    # initalize 
    ana  = analyzer.analyzer(args.output)
    reco = reconstructor.reconstructor(args.config, args.output)

    # if just a single root file
    if args.input[-4:] == 'root': 
        print ' '
        print 'Analyzing the file', args.input
        ana.analyze(args.input, 1)
        print 'Reconstructing the file', args.input
        reco.reconstruct(ana.getCounts(), 1)
    else:
        # loop over the files and run the analyzer
        with open(args.input) as theFileList:
            event = 1
            for theFile in theFileList: 
                theFile = theFile.replace('\r', '').replace('\n', '')
                print ' '
                print 'Analyzing the file', theFile
                ana.analyze(theFile, event)
                print 'Reconstructing the file', theFile
                reco.reconstruct(ana.getCounts(), event)
                event += 1
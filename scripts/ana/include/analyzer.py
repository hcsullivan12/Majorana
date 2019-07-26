"""
@brief Analyzer script for root files from CAEN 32 channel FEB
"""

import ROOT
from array import array

febChannels = [12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27]
sipmIds     = [ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 ]
sipmGains      = [57.72,60.21,59.33,59.16,60.03,58.42,59.77,58.08,58.91,62.64,50.51,48.67,37.07,37.78,25,18.46]
#sipmGains    = [60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60]
sipmPed      = [50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50]
ledTriggers  = 60000.0

class analyzer:
    #------------------------------------------------------------------------
    def __init__(self, outputpath):
        self._outputpath = outputpath

    #------------------------------------------------------------------------
    def analyze(self, daqtreepath, event):  
        ROOT.gROOT.SetBatch(True)

        # initialize histogram
        self.initHistos()
        assert(len(febChannels) == len(sipmIds) == len(sipmGains) )

        # open the anatree
        theFile = ROOT.TFile.Open(daqtreepath, 'READ')
        theTree = theFile.Get("mppc")
        assert(theTree)

        # integral container
        tempCounts = [0 for x in febChannels]
        for entry in theTree: 
            # Fill the histos
            for hist, febId, c in zip(self._hists, febChannels, range(0, len(febChannels))):
                hist.Fill(entry.chg[febId])
                tempCounts[c] += entry.chg[febId]

        # normalize to one trigger
        self._counts = [0 for x in sipmGains]
        for c,g,counter in zip(tempCounts, sipmGains, range(0, len(sipmGains))):
            n = c/ledTriggers
            n = int(n/g)
            self._counts[counter] = n
        for sid, c in zip(sipmIds, self._counts):
            print 'SiPM', sid, 'saw', c, 'photons'

        # plotting
        self.plotHists(event)

    #------------------------------------------------------------------------
    def initHistos(self):
        self._hists = []
        for s in sipmIds:
            name = 'SiPM'+str(s)+'_hist'
            self._hists.append( ROOT.TH1F(name, name, 200, 0, 2000) )

    #------------------------------------------------------------------------
    def plotHists(self, event):
        ROOT.gStyle.SetOptFit(1)

        # Draw
        name = 'sipmspectra_'+str(event)
        c1 = ROOT.TCanvas(name, 'SiPM Spectra', 1000, 1000)
        xL = 4
        yL = 4
        c1.Divide(xL, yL)

        for h, counter in zip(self._hists, range(0, xL*yL)):
            c1.cd(counter+1)
            h.SetMarkerStyle(8)
            h.SetMarkerSize(1)
            h.Draw("pe1")

        f = ROOT.TFile(self._outputpath, 'UPDATE')
        c1.Write()
        f.Close()

    #------------------------------------------------------------------------
    def getCounts(self):
        return self._counts

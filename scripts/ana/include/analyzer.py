"""
@brief Analyzer script for root files from CAEN 32 channel FEB
"""

import ROOT
from array import array

febChannels  = [6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21]
sipmIds      = [ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 ]
sipmGains    = [51.85,63.21,67.41,67.19,47.46,63.8,55.83,58.44,59.94,55.59,49.93,51.39,51.68,47.52,45.4,53.45]
sipmPed      = [50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50]
ledTriggers  = 5000

class analyzer:
    #------------------------------------------------------------------------
    def __init__(self, outputpath):
        self._outputpath = outputpath

    #------------------------------------------------------------------------
    def analyze(self, daqtreepath,event,TrigSize,reco,Th):
        ROOT.gROOT.SetBatch(True)
        TName = daqtreepath.replace("/home/ilker/Desktop/for_ilker/evd/test/", "").strip(".root")
        # initialize histogram
        self.initHistos()
        assert(len(febChannels) == len(sipmIds) == len(sipmGains) )

        # open the anatree
        theFile = ROOT.TFile.Open(daqtreepath, 'READ')
        theTree = theFile.Get("mppc")
        assert(theTree)

        # integral container
        tempCounts = [0 for x in febChannels]
        self._counts = [0 for x in sipmGains]

        print theTree.GetEntries()

        subEvent=1
        subEventCount=1
        ProblemList=[]

        for entry in theTree:
            for febID,cnt in zip(febChannels,range(0,len(febChannels))):

                #Apply the threshold
                if(Th>0):
                    if(entry.chg[febID]>=Th):
                        tempCounts[cnt] +=entry.chg[febID]
                else:
                    tempCounts[cnt] += entry.chg[febID]


            # Compares the TrigSize with event
            if (TrigSize == subEvent or subEvent==theTree.GetEntries()):
                print 'Analyzing Event id: ' + str(subEventCount)

                #Obtain number of PEs.
                for c, g, counter in zip(tempCounts, sipmGains, range(0, len(sipmGains))):
                    n= c/TrigSize
                    n = int(n / g)
                    self._counts[counter] = n


                subEventCount += subEvent
                FileName = str(subEventCount) + "_" + TName

                TotalCounts = sum(self._counts) # Total number of PEs

                #Checks if Number of PEs are too low to cause issue
                if (TotalCounts > 0):
                    reco.reconstruct(self._counts, FileName,event)
                else:
                    error = "Problem --> File : " + TName + ".root eventID= " + str(subEvent) + "=" + str(
                        TotalCounts) + " photons !"
                    print(error)
                    ProblemList.append(error)

                tempCounts = [0 for x in febChannels]
                self._counts = [0 for x in sipmGains]
                subEvent = 0

            subEvent += 1
            # Fill the histos
            """ for hist, febId, c in zip(self._hists, febChannels, range(0, len(febChannels))):
                hist.Fill(entry.chg[febId])
                tempCounts[c] += entry.chg[febId]
            """
        # normalize to one trigger

        """ 
         
          for c,g,counter in zip(tempCounts, sipmGains, range(0, len(sipmGains))):
             n = c/ledTriggers
             n = int(n/g)
             self._counts[counter] = n
            
           for sid, c in zip(sipmIds, self._counts):
             print 'SiPM', sid, 'saw', c, 'photons'

        """

        # plotting
        #self.plotHists(event)

        #List the problems
        for i in ProblemList:
            print("\n" + i)

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
        name = 'SIPMspectra_'+str(event)
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

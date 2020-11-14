"""
@brief Analyzer script for root files from CAEN 32 channel FEB
"""

import ROOT
from array import array

febChannels  = [6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21]
sipmIds      = [ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 ]
sipmGains    = [51.85,63.21,67.41,67.19,47.46,63.8,55.83,58.44,59.94,55.59,49.93,51.39,51.68,47.52,45.4,53.45]
sipmPed      = [50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50]
ProblemList=[]

class analyzer:
    #------------------------------------------------------------------------
    def __init__(self, outputpath):
        self._outputpath = outputpath

    #------------------------------------------------------------------------
    def analyze(self, daqtreepath,event,TrigSize,reco,Th,Method,TrueInfo,DataFileName):
        ROOT.gROOT.SetBatch(True)

        #obtain the file name
        TheFile=daqtreepath.split("/")
        TName = TheFile[len(TheFile)-1].strip(".root")

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



        Nentries=int(theTree.GetEntries())
        TrackTrigger=0
        EventID=1
        ProblemList=[]
        EventCount=0
        AnaEventCount=0
        FilteredEventsCount=0


        for entry in theTree:
            EventCount += 1
            TrackTrigger += 1

            for histo, febID,cnt in zip(self._hists,febChannels,range(0,len(febChannels))):

                # Apply the threshold
                if(Th>0):
                    if((entry.chg[febID]>=Th and febID%2==0) and (entry.chg[febID+1]>=Th)):
                        tempCounts[cnt] +=entry.chg[febID]
                        tempCounts[cnt+1] +=entry.chg[febID+1]
                        self._hists[cnt].Fill(entry.chg[febID])

                        self._hists[cnt+1].Fill(entry.chg[febID+1])
                    else:
                        FilteredEventsCount += 1

                else:
                    tempCounts[cnt] += entry.chg[febID]
                    self._hists[cnt].Fill(entry.chg[febID])

            # Compares the TrigSize with event
            if ((TrigSize == TrackTrigger or EventCount==Nentries) and Method==1):

                self.AnaMain(reco,tempCounts,TrigSize,EventID,TName,Method,EventCount,Nentries,TrueInfo,DataFileName)
                AnaEventCount += TrackTrigger
                TrackTrigger = 0
                EventID += 1  # How many time we are in this loop


            elif(EventCount==Nentries and Method==0):

                self.AnaMain(reco,tempCounts,TrigSize,EventID,TName,Method,EventCount,Nentries,TrueInfo,DataFileName)

                AnaEventCount += TrackTrigger
                TrackTrigger = 0
                EventID += 1  # How many time we are in this loop


        #List the problems
        for i in ProblemList:
            print("\n" + i)

        FilteredEventsCount=FilteredEventsCount/len(sipmGains)
        AnaEventCount=AnaEventCount-FilteredEventsCount
        print "\n---Summary---"
        print "Threshold: " + str(Th)
        print "TriggerSize: " +str(TrigSize)
        print "\nTotal Events: " +str(Nentries)
        print "Total Events analyzed: " + str(AnaEventCount)
        print "Missed Events: " + str(EventCount-AnaEventCount)
        print "Filtered Events: " + str(FilteredEventsCount)


    #------------------------------------------------------------------------
    def AnaMain(self,reco,tempCounts,TrigSize,EventID,TName,Method,EventCount,Nentries,TrueInfo,DataFileName):
        print 'Analyzing Event id: ' + str(EventID)
        # Obtain number of PEs.
        for c, g, counter in zip(tempCounts, sipmGains, range(0, len(sipmGains))):
            if(Method==1 and (EventCount%TrigSize)!=0 and EventCount==Nentries):
                n=c / (EventCount%TrigSize)
            else:
                n= c / TrigSize
            n = int(n / g)
            self._counts[counter] = n

        FileName = str(EventID) + "_" + TName

        TotalCounts = sum(self._counts)  # Total number PEs

        # Checks if Number of PEs are too low to cause issue
        if (TotalCounts > 0):
            reco.reconstruct(self._counts, FileName, EventID,TrueInfo,DataFileName)
        else:
            error = "Problem --> File : " + TName + ".root eventID= " + str(EventID) + "=" + str(
                TotalCounts) + " photons !"
            print(error)
            ProblemList.append(error)

        tempCounts = [0 for x in febChannels]
        self._counts = [0 for x in sipmGains]

        # Plotting the Histogram
        self.plotHists(FileName)




    #------------------------------------------------------------------------
    def initHistos(self):
        self._hists = []
        for s in sipmIds:
            name = 'SiPM'+str(s)+'_hist'
            self._hists.append( ROOT.TH1F(name, name, 200, 0, 2000) )

    #------------------------------------------------------------------------
    def plotHists(self, Name):
        ROOT.gStyle.SetOptFit(1)

        # Draw
        name = 'SIPMspectra_' +Name
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

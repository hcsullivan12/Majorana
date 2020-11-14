"""
@brief Reconstructor script for ana root files from CAEN 32 channel FEB
"""

import ROOT
from array import array
import os,glob

class reconstructor:
    #------------------------------------------------------------------------
    def __init__(self, configpath, outputpath):
        # recreate our output file
        FileCount=0

        self._outputpath = outputpath
        # parse config file
        self._configpath = configpath
        config = self.parse_config()
        assert config.sections()
        self._config = self.convert(config)

        # load pixelization and lookup table
        self._recoHelper = self.initHelper()

        code = """
            class MapConverter {
                public:
                    MapConverter() {};
                    void AddEntry(size_t s, size_t c) { theMap.emplace(s,c); };

                    std::map<size_t, size_t> theMap;
            };
        """
        ROOT.gInterpreter.ProcessLine(code)
        self._converter = ROOT.MapConverter()

    #------------------------------------------------------------------------
    def parse_config(self):
        try:                from ConfigParser import SafeConfigParser
        except ImportError: from configparser import SafeConfigParser

        cfg = SafeConfigParser()
        # want case sensitive
        cfg.optionxform = str

        if not os.path.exists(self._configpath):
            raise ValueError('No such file: %s' % self._configpath)
        cfg.read(self._configpath)
        return cfg

    #------------------------------------------------------------------------
    def convert(self, cfg):
        dat = {}
        for secname in cfg.sections():
            for k,v in cfg.items(secname):
                dat[k] = v
        return dat

    #------------------------------------------------------------------------
    def initHelper(self):
        ROOT.gInterpreter.Declare('#include "include/PixelLoader.h"')
        ROOT.gSystem.Load("include/libPixelLoader.so")
        recoHelper = ROOT.RecoHelper()

        recoHelper.theMethod          = str(self._config['method'])
        recoHelper.theDiskRadius      = float(self._config['diskRadius'])
        recoHelper.theGamma           = float(self._config['gamma'])
        recoHelper.thePixelSpacing    = float(self._config['pixelSpacing'])/10. # convert to cm
        recoHelper.theDoPenalized     = int(self._config['doPenalized'])
        recoHelper.thePenalizedIter   = int(self._config['penalizedStopId'])
        recoHelper.theUnpenalizedIter = int(self._config['unpenalizedStopId'])

        # get path to pixelization
        pixelpath = str(self._config['pixelSpacing']+'mm/pixelization.txt')
        oprefpath = str(self._config['pixelSpacing']+'mm/'+self._config['nsipms']+'_sipm/splinedOpRefTable.txt')
        recoHelper.thePixelPath = os.path.join(str(self._config['topDir']), pixelpath)
        recoHelper.theOpRefPath = os.path.join(str(self._config['topDir']), oprefpath)

        # load pixels
        ROOT.LoadPixelization(recoHelper)
        ROOT.LoadOpRefTable(recoHelper)

        return recoHelper
    
    #------------------------------------------------------------------------
    def reconstruct(self, counts,TName, event,TrueInfo,DataFileName):
        ROOT.gInterpreter.Declare('#include "include/Reconstructor.h"')
        ROOT.gSystem.Load("include/libReconstructor.so")

        ROOT.TH1.AddDirectory(False)
        
        self._converter.theMap.clear()
        for s in range(1, len(counts)):
            self._converter.AddEntry(s, counts[s-1])

        reco = ROOT.Reconstructor(self._converter.theMap, self._recoHelper.thePixelVec, self._recoHelper.theDiskRadius)
        if self._recoHelper.theMethod == 'chi2':
            reco.DoChi2(1)
            reco.Dump()
            values=reco.EstimatedValues().split(",")
        else:
            print 'Need to update the reconstructor!'

        Condition=TName.split("_")
        TotalPe=str(sum(counts))
        File=open(DataFileName,"a+")
        for f,tx,ty in TrueInfo:
            if(Condition[1]==f):
                Full = Condition[0] + " " + f + " " + Condition[2] + " " + Condition[3] +  " " + tx +  " " + ty +  " " + TotalPe + " " + values[0] +  " " + values[1] +  " " + values[2] + "\n"
                File.write(Full)

        File.close()
            
        self.update(reco,TName, event)


    #------------------------------------------------------------------------
    def update(self, reco,TName, event):
        MLI= "MLI_" + TName
        CHI= "Chi_" + TName
        f = ROOT.TFile.Open(self._outputpath, 'UPDATE')
        reco.MLImage().Write(MLI)
        reco.Chi2Image().Write(CHI)
        f.Close()

    #------------------------------------------------------------------------
    def PlotGraph(self,x,y,Name,CTitle,XTitle,YTitle,Mode):
        gr=ROOT.TGraph(len(x),x,y)
        Canvas=TCanvas(Name,CTitle,200,10,700,500)
        Canvas.SetFillColor(42)
        gr.SetLineColor(2)
        gr.SetLineWidth(4)
        gr.SetMarkerStyle(21)
        gr.GetXaxis().SetTitle(XTitle)
        gr.GetYaxis().SetTitle(YTitle)
        gr.Draw(Mode)
        f = ROOT.TFile(self._outputpath, 'UPDATE')
        Canvas.Write(Name)
        f.Close()


    def GetInfo(self):
        return self._Info()

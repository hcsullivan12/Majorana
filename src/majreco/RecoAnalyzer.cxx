/**
 * @file RecoAnalyzer.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Module to produce a ROOT analysis TTree.
 * @date 07-04-2019
 * 
 */

#include "majreco/RecoAnalyzer.h"
#include "majutil/OpDetPhotonTable.h"
#include "majutil/PixelTable.h"
#include "majreco/Reconstructor.h"
#include "majreco/Configuration.h"

#include "TFile.h"

namespace majreco 
{

//------------------------------------------------------------------------
RecoAnalyzer::RecoAnalyzer()
 : fAnaTree(nullptr)
{
  // Reset variables
  ResetVars();

  // Get config
  Configuration* config = Configuration::Instance();
  fRecoOutputPath = config->RecoOutputPath();
  
  fOutputFile = new TFile(fRecoOutputPath.c_str(), "RECREATE");
  fAnaTree    = new TTree("anatree", "analysis tree");
  fAnaTree->SetAutoSave(50);
  fAnaTree->SetAutoFlush(50);
  fAnaTree->SetDirectory(fOutputFile);


  fAnaTree->Branch("event",      &fEvent, "event/I");
  fAnaTree->Branch("nPixels",    &fNPixels, "nPixels/I");
  fAnaTree->Branch("diskRadius", &fDiskRadius, "diskRadius/F");
  fAnaTree->Branch("nPrimaries", &fNPrimaries, "nPrimaries/I");
  fAnaTree->Branch("sourcePosXYZ",  &fSourcePosXYZ);
  fAnaTree->Branch("sipmToLY",      &fSiPMToLY);
  fAnaTree->Branch("recoTotalLY",   &fRecoTotalLY, "recoTotalLY/I");
  fAnaTree->Branch("SimrecoPosXYZ",    &fSimRecoPosXYZ);
  fAnaTree->Branch("TruePosXYZ",    &fTruePosXYZ);
  fAnaTree->Branch("DataRecoPosXYZ",    &fDatarecoPosXYZ);
  fAnaTree->Branch("Time",    &fTime,"Time/F");
  fAnaTree->Branch("TotalPEDATA",    &fTotalPEDATA,"TotalPEDATA/F");
  fAnaTree->Branch("TotalPESim",    &fTotalPESim,"TotalPESim/F");
  fAnaTree->Branch("Chi2SquareProb",    &fChi2SquareProb,"Chi2SquareProb/F");
  fAnaTree->Branch("Chi2SquareResidues",    &fChi2SquareResidues);
  fAnaTree->Branch("RealData",    &fRealData);
  fAnaTree->Branch("SimData",    &fSimData);
  fAnaTree->Branch("PercentError",    &fPercentError);
  fAnaTree->Branch("DataRecoChi2Square",    &fDataRecoChi2Square,"DataRecoChi2Square/F");
  fAnaTree->Branch("DataRecoChi2ID",    &fDataRecoChi2ID,"DataRecoChi2ID/I");
  fAnaTree->Branch("DataRecoChi2Vertex",    &fDataRecoChi2Vertex);
  fAnaTree->Branch("SimRecoChi2Square",    &fSimRecoChi2Square,"SimRecoChi2Square/F");
  fAnaTree->Branch("SimRecoChi2ID",    &fSimRecoChi2ID,"SimRecoChi2ID/I");
  fAnaTree->Branch("SimRecoChi2Vertex",    &fSimRecoChi2Vertex);

}

//------------------------------------------------------------------------
RecoAnalyzer::~RecoAnalyzer()
{
  fAnaTree->AutoSave();

  if (fAnaTree)    delete fAnaTree;
  if (fOutputFile) delete fOutputFile;
}

//------------------------------------------------------------------------
void RecoAnalyzer::Fill(const size_t&             e,
                        const size_t&             nPixels,
                        const float&              diskRadius,
                        const size_t&             nPrimaries,
                        const std::vector<float>& sourcePosXYZ,
                        const std::vector<int>&   sipmToLY,
                        const size_t&             recoLY,
                        const std::vector<float>& SimrecoPosXYZ,
                        const std::vector<float>& truePosXYZ)
{
  ResetVars();
  /**
   * @todo nPixels showing large value. May not be filling in simulation.
   * 
   */
  fEvent        = e;
  fNPixels      = 0;//nPixels;
  fDiskRadius   = diskRadius;
  fNPrimaries   = nPrimaries;
  fSourcePosXYZ = sourcePosXYZ;
  fSiPMToLY     = sipmToLY;
  fRecoTotalLY  = recoLY;
  fSimRecoPosXYZ   = SimrecoPosXYZ;
  fTruePosXYZ   = truePosXYZ;

  fAnaTree->Fill();
}
//------------------------------------------------------------------------
    void RecoAnalyzer::Fill(const size_t&             e,
                            const size_t&             nPixels,
                            const float&              diskRadius,
                            const size_t&             nPrimaries,
                            const std::vector<float>& sourcePosXYZ,
                            const std::vector<int>&   sipmToLY,
                            const size_t&             recoLY,
                            const std::vector<float>& SimrecoPosXYZ,
                            const std::vector<float>& truePosXYZ,
                            const std::vector<float>& DataRecoPosXYZ
                           )

    {
        ResetVars();
        /**
         * @todo nPixels showing large value. May not be filling in simulation.
         *
         */
        fEvent        = e;
        fNPixels      = 0;//nPixels;
        fDiskRadius   = diskRadius;
        fNPrimaries   = nPrimaries;
        fSourcePosXYZ = sourcePosXYZ;
        fSiPMToLY     = sipmToLY;
        fRecoTotalLY  = recoLY;
        fSimRecoPosXYZ   = SimrecoPosXYZ;
        fTruePosXYZ   = truePosXYZ;
        fDatarecoPosXYZ=DataRecoPosXYZ;
        fAnaTree->Fill();


    }
//------------------------------------------------------------------------
    void RecoAnalyzer::Fill(const size_t&             e,
                            const size_t&             nPixels,
                            const float&              diskRadius,
                            const size_t&             nPrimaries,
                            const std::vector<float>& sourcePosXYZ,
                            const std::vector<int>&   sipmToLY,
                            const size_t&             recoLY,
                            const std::vector<float>& SimrecoPosXYZ,
                            const std::vector<float>& truePosXYZ,
                            const std::vector<float>& DataRecoPosXYZ,
                            const float & NoiseETime,
                            const float & GoodETime,
                            const float & TotalPE_GoodE,
                            const float & TotalPE_NoiseE,
                            const float & TotalRatio_GoodE,
                            const float & TotalRatio_NoiseE)

    {
        ResetVars();
        /**
         * @todo nPixels showing large value. May not be filling in simulation.
         *
         */
        fEvent        = e;
        fNPixels      = 0;//nPixels;
        fDiskRadius   = diskRadius;
        fNPrimaries   = nPrimaries;
        fSourcePosXYZ = sourcePosXYZ;
        fSiPMToLY     = sipmToLY;
        fRecoTotalLY  = recoLY;
        fSimRecoPosXYZ   = SimrecoPosXYZ;
        fTruePosXYZ   = truePosXYZ;
        fDatarecoPosXYZ=DataRecoPosXYZ;
        fNoiseETime =NoiseETime;
        fGoodETime =GoodETime;
        fTotalPE_GoodE=TotalPE_GoodE;
        fTotalPE_NoiseE=TotalPE_NoiseE;
        fTotalRatio_GoodE=TotalRatio_GoodE;
        fTotalRatio_NoiseE=TotalRatio_NoiseE;

        fAnaTree->Fill();
        if(e%50==0)
            fAnaTree->AutoSave();


    }

    void RecoAnalyzer::Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY,
              const std::vector<float>& SimrecoPosXYZ,
              const std::vector<float>& truePosXYZ,
              const std::vector<float>& DatarecoPosXYZ,
              const float & Time,
              const float & TotalPEDATA,
              const float & TotalPESim,
              const float & Chi2SquareProb,
              const std::vector<float> & Chi2SquareResidues,
              const std::vector<int> & RealData,
              const std::vector<int> & SimData,
              const std::vector<float> & PercentError
    )
    {


        ResetVars();
        /**
         * @todo nPixels showing large value. May not be filling in simulation.
         *
         */
        fEvent        = e;
        fNPixels      = 0;//nPixels;
        fDiskRadius   = diskRadius;
        fNPrimaries   = nPrimaries;
        fSourcePosXYZ = sourcePosXYZ;
        fSiPMToLY     = sipmToLY;
        fRecoTotalLY  = recoLY;
        fSimRecoPosXYZ   = SimrecoPosXYZ;
        fTruePosXYZ   = truePosXYZ;
        fDatarecoPosXYZ=DatarecoPosXYZ;
        fTime         = Time;
        fTotalPEDATA = TotalPEDATA;
        fTotalPESim = TotalPESim;
        fChi2SquareProb=Chi2SquareProb;
        fChi2SquareResidues= Chi2SquareResidues;
        fRealData = RealData;
        fSimData= SimData;
        fPercentError=PercentError;


        fAnaTree->Fill();
        if(e%50==0)
            fAnaTree->AutoSave();

    }
    /////////////////////////////////////////////////////////////////
    //////   This is filling with Reconstructed ChiSquare from data and Sim
    /////
    ////////////////////////////////////////////////////////////////

    void RecoAnalyzer::Fill(const size_t&             e,
                            const size_t&             nPixels,
                            const float&              diskRadius,
                            const size_t&             nPrimaries,
                            const std::vector<float>& sourcePosXYZ,
                            const std::vector<int>&   sipmToLY,
                            const size_t&             recoLY,
                            const std::vector<float>& SimrecoPosXYZ,
                            const std::vector<float>& truePosXYZ,
                            const std::vector<float>& DatarecoPosXYZ,
                            const float & Time,
                            const float & TotalPEDATA,
                            const float & TotalPESim,
                            const float & Chi2SquareProb,
                            const std::vector<float> & Chi2SquareResidues,
                            const std::vector<int> & RealData,
                            const std::vector<int> & SimData,
                            const std::vector<float> & PercentError,
                            const float & DataRecoChi2Square,
                            const int & DataRecoChi2ID,
                            const std::vector<float> & DataRecoChi2Vertex,
                            const float & SimRecoChi2Square,
                            const int & SimRecoChi2ID,
                            const std::vector<float> & SimRecoChi2Vertex

    ) {


        ResetVars();
        /**
         * @todo nPixels showing large value. May not be filling in simulation.
         *
         */
        fEvent        = e;
        fNPixels      = 0;//nPixels;
        fDiskRadius   = diskRadius;
        fNPrimaries   = nPrimaries;
        fSourcePosXYZ = sourcePosXYZ;
        fSiPMToLY     = sipmToLY;
        fRecoTotalLY  = recoLY;
        fSimRecoPosXYZ   = SimrecoPosXYZ;
        fTruePosXYZ   = truePosXYZ;
        fDatarecoPosXYZ=DatarecoPosXYZ;
        fTime         = Time;
        fTotalPEDATA = TotalPEDATA;
        fTotalPESim = TotalPESim;
        fChi2SquareProb=Chi2SquareProb;
        fChi2SquareResidues= Chi2SquareResidues;
        fRealData = RealData;
        fSimData= SimData;
        fPercentError=PercentError;
        fDataRecoChi2Square=DataRecoChi2Square;
        fDataRecoChi2Vertex=DataRecoChi2Vertex;
        fDataRecoChi2ID=DataRecoChi2ID;
        fSimRecoChi2Square=SimRecoChi2Square;
        fSimRecoChi2Vertex=SimRecoChi2Vertex;
        fSimRecoChi2ID=SimRecoChi2ID;


        fAnaTree->Fill();
        if(e%50==0)
            fAnaTree->AutoSave();

    }


//------------------------------------------------------------------------
void RecoAnalyzer::ResetVars()
{
  fEvent      = -99999;
  fNPixels    = -99999;
  fDiskRadius = -99999;
  fNPrimaries = -99999;
  fRecoTotalLY = -99999;
  fSourcePosXYZ.clear();
  fSiPMToLY.clear();
  fSimRecoPosXYZ.clear();
  fTruePosXYZ.clear();
  fDatarecoPosXYZ.clear();
  fGoodETime=0;
  fNoiseETime=0;
  fTotalPE_GoodE=0;
  fTotalPE_NoiseE=0;
  fTotalRatio_GoodE=0;
  fTotalRatio_NoiseE=0;
  fTime=0;
  fTotalPEDATA = 0;
  fTotalPESim = 0;
  fChi2SquareProb=99999;
  fChi2SquareResidues.clear();
  fRealData.clear();
  fSimData.clear();
  fPercentError.clear();
    fDataRecoChi2Square=99999;
    fDataRecoChi2Vertex.clear();
    fDataRecoChi2ID=99999;
    fSimRecoChi2Square=99999;
    fSimRecoChi2Vertex.clear();
    fSimRecoChi2ID=99999;




}

}

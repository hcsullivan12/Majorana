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
  
  fOutputFile = new TFile(fRecoOutputPath.c_str(), "UPDATE");
  fAnaTree    = new TTree("anatree", "analysis tree");
  fAnaTree->SetDirectory(fOutputFile);

  fAnaTree->Branch("event",      &fEvent, "event/I");
  fAnaTree->Branch("nPixels",    &fNPixels, "nPixels/I");
  fAnaTree->Branch("diskRadius", &fDiskRadius, "diskRadius/D");
  fAnaTree->Branch("nPrimaries", &fNPrimaries, "nPrimaries/I");
  fAnaTree->Branch("sourcePosXYZ",  &fSourcePosXYZ);
  fAnaTree->Branch("sipmToLY",      &fSiPMToLY);
  fAnaTree->Branch("recoTotalLY",   &fRecoTotalLY, "recoTotalLY/I");
  fAnaTree->Branch("recoPosXYZ",    &fRecoPosXYZ);
}

//------------------------------------------------------------------------
RecoAnalyzer::~RecoAnalyzer()
{
  fAnaTree->Write();

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
                        const std::vector<float>& recoPosXYZ)
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
  fRecoPosXYZ   = recoPosXYZ;

  fAnaTree->Fill();
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
  fRecoPosXYZ.clear();
}

}

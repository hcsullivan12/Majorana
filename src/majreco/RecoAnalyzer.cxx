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
  
  fAnaTree = new TTree("anatree", "analysis tree");

}

//------------------------------------------------------------------------
RecoAnalyzer::~RecoAnalyzer()
{
  TFile f(fRecoOutputPath.c_str(), "UPDATE");
  fAnaTree->Write();
  f.Close();

  if (fAnaTree) delete fAnaTree;
}

//------------------------------------------------------------------------
void RecoAnalyzer::Fill(const unsigned& e)
{
  ResetVars();


  
  fAnaTree->Fill();
}

//------------------------------------------------------------------------
void RecoAnalyzer::ResetVars()
{
 
}

}

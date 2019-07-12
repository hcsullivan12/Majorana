/**
 * @file RecoHelper.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Helper class to interface with simulation output and run reconstruction.
 * @date 07-04-2019
 * 
 */

#include "majreco/RecoHelper.h"
#include "majreco/Configuration.h"
#include "majutil/PixelTable.h"
#include "majreco/Reconstructor.h"
#include "majreco/RecoAnalyzer.h"

#include <assert.h>

#include "TFile.h"
#include "TTree.h"

namespace majreco
{

RecoHelper* RecoHelper::instance = 0;

//------------------------------------------------------------------------
RecoHelper* RecoHelper::CreateInstance()
{
  if (instance == 0)
  {
    static RecoHelper recoHelper;
    instance = &recoHelper;
  }
  return instance;
}

//------------------------------------------------------------------------
RecoHelper* RecoHelper::Instance()
{
  assert(instance);
  return instance;
}

//------------------------------------------------------------------------
RecoHelper::RecoHelper() 
{}

//------------------------------------------------------------------------
RecoHelper::~RecoHelper()
{}

//------------------------------------------------------------------------
void RecoHelper::Start()
{
  // Get config and pixels
  Configuration*       config     = Configuration::Instance();
  majutil::PixelTable* pixelTable = majutil::PixelTable::Instance();

  // Read the tree from simulation
  TFile simFile(config->SimulateOutputPath().c_str(), "READ");
  TTree* tree = (TTree*)simFile.Get("anatree");
  assert(tree);
  auto nentries = tree->GetEntries();

  // Variables to extract from tree
  int    event;
  int    nPixels;
  int    nSiPMs;
  double diskRadius;
  int    nPrimaries;
  std::vector<float>* sourcePosXYZ = 0;
  std::vector<int>*   sipmToLY     = 0;

  int errorInt = -90000;
  tree->SetBranchAddress("event",        &event);
  tree->SetBranchAddress("nPixels",      &nPixels);
  tree->SetBranchAddress("nMPPCs",       &nSiPMs);
  tree->SetBranchAddress("diskRadius",   &diskRadius);
  tree->SetBranchAddress("nPrimaries",   &nPrimaries);
  tree->SetBranchAddress("sourcePosXYZ", &sourcePosXYZ);
  tree->SetBranchAddress("mppcToLY",     &sipmToLY);

  // Create RecoAnatree object
  RecoAnalyzer analyzer;

  // Loop over all entries in the tree from simulation.
  for (auto entry = 0; entry < nentries; entry++)
  {
    std::cout << "\nReconstructing event " << entry << std::endl;
    tree->GetEntry(entry);

    // Make sure the values make sense first
    assert(nSiPMs > 0 && diskRadius > 0 && nPrimaries > 0);
    for (auto& p : *sourcePosXYZ) assert(p > errorInt);
    for (auto& p : *sipmToLY)     assert(p > errorInt);

    // Store data in more convienent format
    std::map<size_t, size_t> data;
    size_t sipmId(1);
    for (size_t d = 1; d <= sipmToLY->size(); d++) { data.emplace(d, (*sipmToLY)[d-1]); }

    // Initialize our reconstruction algorithm 
    Reconstructor reconstructor(data, pixelTable->GetPixels(), diskRadius);
    //reconstructor.DoEmMl(config->Gamma(),
    //                     config->UnpenalizedStopId(),
    //                     config->PenalizedStopId(),
    //                     config->DoPenalized());
 
    reconstructor.DoChi2(config->UnpenalizedStopId());
    reconstructor.Dump();

    analyzer.Fill(entry, 
                  nPixels,
                  diskRadius,
                  nPrimaries,
                  *sourcePosXYZ,
                  *sipmToLY,
                  reconstructor.TotalLight());
  }
}

}

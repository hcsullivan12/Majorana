// C++ includes
#include <iostream>
#include <string>

// majorana includes
#include "majreco/Configuration.h"
#include "majreco/RecoHelper.h"
#include "majutil/PixelTable.h"

// ROOT includes
#include "TFile.h"

// Prototypes
void HandleArgs(int argc, char **argv, majreco::Configuration* recoConfig);
void InitializeFiles(const majreco::Configuration*);
void DisplayHelp();

//------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // Initialize configuration
  majreco::Configuration* recoConfig = majreco::Configuration::CreateInstance();
  recoConfig->Initialize("tests/RecoConfiguration.ini");
  // Safety check
  recoConfig->CheckConfiguration();

  // Initialize output files
  InitializeFiles(recoConfig);
  // Initialize pixels 
  majutil::PixelTable* pixelTable = majutil::PixelTable::CreateInstance();
  pixelTable->Initialize(recoConfig->PixelizationPath());
  pixelTable->LoadReferenceTable(recoConfig->OpReferenceTablePath());

  // Initialize helper for interfacing with reconstruction alg
  majreco::RecoHelper* recoHelper = majreco::RecoHelper::CreateInstance();
  recoHelper->Start();

  return 0;
}

//------------------------------------------------------------------------
void InitializeFiles(const majreco::Configuration* recoConfig)
{
  TFile f1(recoConfig->RecoOutputPath().c_str(), "RECREATE");
  f1.Close();
}



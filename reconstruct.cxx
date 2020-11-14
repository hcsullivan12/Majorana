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
  // Handle runtime args
  HandleArgs(argc, argv, recoConfig); 
  // Initialize output files
  InitializeFiles(recoConfig);
  // Initialize pixels 
  majutil::PixelTable* pixelTable = majutil::PixelTable::CreateInstance();
  pixelTable->fRadius=recoConfig->diskRadius();
  pixelTable->Initialize(recoConfig->PixelizationPath());
  pixelTable->LoadReferenceTable(recoConfig->OpReferenceTablePath());

  // Initialize helper for interfacing with reconstruction alg
  majreco::RecoHelper* recoHelper = majreco::RecoHelper::CreateInstance();
  recoHelper->Start();

  return 0;
}

//------------------------------------------------------------------------
void HandleArgs(int argc, char **argv, majreco::Configuration* recoConfig)
{ 
  std::string configPath("");

  if (argc < 2) DisplayHelp();
  for (int arg = 0; arg < argc; arg++) if (std::string(argv[arg]) == "-h") DisplayHelp();
  for (int arg = 0; arg < (argc-1); arg++)
  {
    if (std::string(argv[arg]) == "-c") configPath = std::string(argv[arg+1]);
  }

  // Pass configuration
  assert(configPath != "");
  recoConfig->Initialize(configPath);

  // Safety check
  recoConfig->CheckConfiguration();
  // Output to terminal
  recoConfig->PrintConfiguration();
}

//------------------------------------------------------------------------
void DisplayHelp()
{
  std::cout << "\nUsage: ./simulate -c CONFIGPATH <Options>\n";
  std::cout << std::endl;
  std::exit(1);
}

//------------------------------------------------------------------------
void InitializeFiles(const majreco::Configuration* recoConfig)
{
  TFile f1(recoConfig->RecoOutputPath().c_str(), "RECREATE");
  f1.Close();
}



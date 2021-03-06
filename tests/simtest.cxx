// C++ includes
#include <iostream>
#include <string>

// majorana includes
#include "majsim/Configuration.h"
#include "majsim/G4Helper.h"
#include "majutil/PixelTable.h"

// ROOT includes
#include "TFile.h"


// Prototypes
void HandleArgs(int argc, char **argv, majsim::Configuration* simConfig);
void InitializeFiles(const majsim::Configuration*);
void DisplayHelp();

//------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // Initialize configuration
  majsim::Configuration* simConfig = majsim::Configuration::CreateInstance();

  // Pass configuration
  simConfig->SetVisualization(false);
  simConfig->SetEvdMode(false);
  simConfig->Initialize("tests/SimConfiguration.ini");

  // Safety check
  simConfig->CheckConfiguration();

  // Initialize output files
  InitializeFiles(simConfig);
  // Initialize source configuration
  if (simConfig->SourceMode() == "pixel")
  {
    // Initialize pixels so we can make the reference table
    majutil::PixelTable* pixelTable = majutil::PixelTable::CreateInstance();
    pixelTable->Initialize(simConfig->PixelizationPath());
  }
  
  // Start G4
  majsim::G4Helper* g4Helper = majsim::G4Helper::CreateInstance();
  g4Helper->StartG4();

  return 0;
}

//------------------------------------------------------------------------
void InitializeFiles(const majsim::Configuration* simConfig)
{
  TFile f1(simConfig->SimulateOutputPath().c_str(), "RECREATE");
  f1.Close();
}



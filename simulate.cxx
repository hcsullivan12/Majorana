// C++ includes
#include <iostream>
#include <string>

// Majorana includes
#include "Configuration.h"
#include "G4Helper.h"
#include "PixelTable.h"

// ROOT includes
#include "TFile.h"

// Visualization 
static bool showVis = false;

// Prototypes
void HandleArgs(int argc, char **argv);
void InitializeFiles(const majorana::Configuration*);

int main(int argc, char **argv)
{
  // Handle runtime args
  HandleArgs(argc, argv); 
  // Initialize configuration
  majorana::Configuration* config = majorana::Configuration::CreateInstance();
  // Pass visualization
  config->SetVisualization(showVis);
  config->Initialize(std::string(argv[1]));
  // Initialize output files
  InitializeFiles(config);
  // Initialize source configuration
  if (config->SourceMode() == "pixel")
  {
    // Initialize pixels so we can make the reference table
    majorana::PixelTable* pixelTable = majorana::PixelTable::CreateInstance();
    pixelTable->Initialize(config->PixelizationPath());
  }
  // If we're wanting to reconstruct in point mode, it's 
  // easier if we still initialize pixels to load reference table
  // in pixel table 
  if (config->Reconstruct())
  {
    majorana::PixelTable* pixelTable = majorana::PixelTable::CreateInstance();
    pixelTable->Initialize(config->PixelizationPath());
    pixelTable->LoadReferenceTable(config->OpReferenceTablePath());
  }
  // Start G4
  majorana::G4Helper* g4Helper = majorana::G4Helper::CreateInstance();
  g4Helper->StartG4();

  return 0;
}

void HandleArgs(int argc, char **argv)
{
  if (argc < 2 || argc > 4 || argc == 3) 
  {
    std::cout << "\nUsage: ./simulate PATH_TO_CONFIG <Options>\n";
    std::cout << "Options:\n"
              << "  --vis ON/OFF (If ON, render visualization. Default is OFF.)\n";
    std::cout << std::endl;
    std::exit(1);
  }
  if (argc == 4)
  {
    if (std::string(argv[2]) == "--vis" && std::string(argv[3]) == "ON") showVis = true;    
  }
}

void InitializeFiles(const majorana::Configuration* config)
{
  TFile f1(config->SimulateOutputPath().c_str(), "RECREATE");
  f1.Close();
  if (config->Reconstruct())
  {
    TFile f2(config->RecoAnaTreePath().c_str(), "RECREATE");
    f2.Close();
  }
}



// C++ includes
#include <iostream>
#include <string>

// Majorana includes
#include "Configuration.h"
#include "G4Helper.h"
#include "PixelTable.h"

// ROOT includes
#include "TFile.h"

// Prototypes
void HandleArgs(int argc, char **argv, majorana::Configuration* config);
void InitializeFiles(const majorana::Configuration*);
void DisplayHelp();

int main(int argc, char **argv)
{
  // Initialize configuration
  majorana::Configuration* config = majorana::Configuration::CreateInstance();
  // Handle runtime args
  HandleArgs(argc, argv, config); 
  // Initialize output files
  InitializeFiles(config);
  // Initialize source configuration
  if (config->SourceMode() == "pixel")
  {
    // Initialize pixels so we can make the reference table
    majorana::PixelTable* pixelTable = majorana::PixelTable::CreateInstance();
    pixelTable->Initialize(config->PixelizationPath());
  }
  else if (config->SourceMode() == "point" && config->Reconstruct())
  {   
    // If we're wanting to reconstruct in point mode
    majorana::PixelTable* pixelTable = majorana::PixelTable::CreateInstance();
    pixelTable->Initialize(config->PixelizationPath());
    pixelTable->LoadReferenceTable(config->OpReferenceTablePath());
  }
  // Start G4
  majorana::G4Helper* g4Helper = majorana::G4Helper::CreateInstance();
  g4Helper->StartG4();

  return 0;
}

void HandleArgs(int argc, char **argv, majorana::Configuration* config)
{
  bool showVis = false;
  bool evdMode = false;
  std::string configPath = "";
  std::string pixelPath  = "";
  std::string opRefTPath = "";
  std::string simOutputPath  = "";
  std::string recoAnaTPath   = "";
  
  unsigned nsipms(0);
  
  if (argc < 2) DisplayHelp();
  for (unsigned arg = 0; arg < argc; arg++) if (std::string(argv[arg]) == "-h") DisplayHelp();
  for (unsigned arg = 0; arg < (argc-1); arg++)
  {
    if (std::string(argv[arg]) == "-vis" && std::string(argv[arg+1]) == "ON") showVis = true;    
    if (std::string(argv[arg]) == "-c") configPath = std::string(argv[arg+1]);
    if (std::string(argv[arg]) == "-E" && std::string(argv[arg+1]) == "ON") evdMode = true;  
    if (std::string(argv[arg]) == "-ov" && (arg+5) < argc) 
    {
      nsipms        = std::stoi(argv[arg+1]);
      pixelPath     = std::string(argv[arg+2]);
      opRefTPath    = std::string(argv[arg+3]);
      simOutputPath = std::string(argv[arg+4]);
      recoAnaTPath  = std::string(argv[arg+5]);
    }
  }
  // Pass configuration
  config->SetVisualization(showVis);
  config->SetEvdMode(evdMode);
  assert(configPath != "");
  config->Initialize(configPath);

  // Overide 
  if (nsipms > 0       && 
      pixelPath  != "" && 
      opRefTPath != "" &&
      simOutputPath != "" &&
      recoAnaTPath  != "")
  {
    config->SetNSiPMs(nsipms);
    config->SetPixelPath(pixelPath);
    config->SetOpRefTablePath(opRefTPath);
    config->SetSimOutputPath(simOutputPath);
    config->SetRecoAnaPath(recoAnaTPath);
  }
  // Safety check
  config->CheckConfiguration();
  // Output to terminal
  config->PrintConfiguration();
}

void DisplayHelp()
{
  std::cout << "\nUsage: ./simulate -c CONFIGPATH <Options>\n";
  std::cout << "Options:\n"
            << "  -h for help\n"
            << "  -vis ON/OFF (If ON, render visualization. Default is OFF.)\n"
            << "  -ov  NSIPMS PIXELIZATIONPATH OPREFTABLEPATH SIMULATEOUTPUT RECOANATREE (If wanting to overide configuration.)";
  std::cout << std::endl;
  std::exit(1);
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



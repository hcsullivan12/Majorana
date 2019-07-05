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
void HandleArgs(int argc, char **argv, majsim::Configuration* genConfig);
void InitializeFiles(const majsim::Configuration*);
void DisplayHelp();

//------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // Initialize configuration
  majsim::Configuration* genConfig = majsim::Configuration::CreateInstance();
  // Handle runtime args
  HandleArgs(argc, argv, genConfig); 
  // Initialize output files
  InitializeFiles(genConfig);
  // Initialize source configuration
  if (genConfig->SourceMode() == "pixel")
  {
    // Initialize pixels so we can make the reference table
    majutil::PixelTable* pixelTable = majutil::PixelTable::CreateInstance();
    pixelTable->Initialize(genConfig->PixelizationPath());
  }
  else if (genConfig->SourceMode() == "point")
  {   
    // If we're wanting to reconstruct in point mode
    majutil::PixelTable* pixelTable = majutil::PixelTable::CreateInstance();
    pixelTable->Initialize(genConfig->PixelizationPath());
    pixelTable->LoadReferenceTable(genConfig->OpReferenceTablePath());
  }
  // Start G4
  majsim::G4Helper* g4Helper = majsim::G4Helper::CreateInstance();
  g4Helper->StartG4();

  return 0;
}

//------------------------------------------------------------------------
void HandleArgs(int argc, char **argv, majsim::Configuration* genConfig)
{
  bool showVis = false;
  bool evdMode = false;
  std::string configPath = "";
  std::string pixelPath  = "";
  std::string opRefTPath = "";
  std::string simOutputPath  = "";
  
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
    }
  }
  // Pass configuration
  genConfig->SetVisualization(showVis);
  genConfig->SetEvdMode(evdMode);
  assert(configPath != "");
  genConfig->Initialize(configPath);

  // Overide 
  if (nsipms > 0          && 
      pixelPath     != "" && 
      opRefTPath    != "" &&
      simOutputPath != "")
  {
    genConfig->SetNSiPMs(nsipms);
    genConfig->SetPixelPath(pixelPath);
    genConfig->SetOpRefTablePath(opRefTPath);
    genConfig->SetSimOutputPath(simOutputPath);
  }
  // Safety check
  genConfig->CheckConfiguration();
  // Output to terminal
  genConfig->PrintConfiguration();
}

//------------------------------------------------------------------------
void DisplayHelp()
{
  std::cout << "\nUsage: ./simulate -c CONFIGPATH <Options>\n";
  std::cout << "Options:\n"
            << "  -h for help\n"
            << "  -vis ON/OFF (If ON, render visualization. Default is OFF.)\n"
            << "  -ov  NSIPMS PIXELIZATIONPATH OPREFTABLEPATH SIMULATEOUTPUT (If wanting to overide configuration.)";
  std::cout << std::endl;
  std::exit(1);
}

//------------------------------------------------------------------------
void InitializeFiles(const majsim::Configuration* genConfig)
{
  TFile f1(genConfig->SimulateOutputPath().c_str(), "RECREATE");
  f1.Close();
}



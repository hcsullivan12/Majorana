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
  // Handle runtime args
  HandleArgs(argc, argv, simConfig); 
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
void HandleArgs(int argc, char **argv, majsim::Configuration* simConfig)
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
  simConfig->SetVisualization(showVis);
  simConfig->SetEvdMode(evdMode);
  assert(configPath != "");
  simConfig->Initialize(configPath);

  // Overide 
  if (nsipms > 0          && 
      pixelPath     != "" && 
      opRefTPath    != "" &&
      simOutputPath != "")
  {
    simConfig->SetNSiPMs(nsipms);
    simConfig->SetPixelPath(pixelPath);
    simConfig->SetOpRefTablePath(opRefTPath);
    simConfig->SetSimOutputPath(simOutputPath);
  }
  // Safety check
  simConfig->CheckConfiguration();
  // Output to terminal
  simConfig->PrintConfiguration();
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
void InitializeFiles(const majsim::Configuration* simConfig)
{
  TFile f1(simConfig->SimulateOutputPath().c_str(), "RECREATE");
  f1.Close();
}



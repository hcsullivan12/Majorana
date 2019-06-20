// 
// File: G4Helper.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to interface with G4
//

#include "G4Helper.h"
#include "Analyzer.h"
#include "OpDetPhotonTable.h"
#include "PixelTable.h"
#include "Configuration.h"
#include "Reconstructor.h"

#include <assert.h>

namespace majorana
{

G4Helper* G4Helper::instance = 0;

G4Helper* G4Helper::CreateInstance()
{
  if (instance == 0)
  {
    static G4Helper g4Helper;
    instance = &g4Helper;
  }
  return instance;
}

G4Helper* G4Helper::Instance()
{
  assert(instance);
  return instance;
}

G4Helper::G4Helper() 
 : fRunManager(NULL),
   fVisManager(NULL),
   fDetector(NULL),
   fGeneratorAction(NULL),
   fActionInitialization(NULL),
   fPhysicsList(NULL)
{
  // Get config
  Configuration* config = Configuration::Instance();

  // Visualization and outputs
  fShowVis            = config->ShowVis();
  fVisMacroPath       = config->VisMacroPath();
  fSimulateOutputPath = config->SimulateOutputPath();
  fRecoAnaTreePath    = config->RecoAnaTreePath();

  // Initialize managers
  fRunManager = new G4RunManager;
  fUIManager  = G4UImanager::GetUIpointer();

  // Load the steering file 
  config->ReadSteeringFile(); 
  
  // Construct detector
  fDetector = new DetectorConstruction();

  // Update the run manager
  fRunManager->SetUserInitialization(fDetector);

  // Initialize physics list
  fPhysicsList = new PhysicsList;
  fRunManager->SetUserInitialization(fPhysicsList);

  // Initialize action initialization 
  fActionInitialization = new ActionInitialization();

  // Get the pointer to the generator action
  fGeneratorAction = fActionInitialization->GetGeneratorAction();

  fRunManager->SetUserInitialization(fActionInitialization);
  fRunManager->Initialize();
}

G4Helper::~G4Helper()
{
  //if (fUIManager)  delete fUIManager;
  #ifdef G4VIS_USE
  if (fVisManager) delete fVisManager;
  #endif 
  if (fRunManager) delete fRunManager;
}

void G4Helper::StartG4()
{
  // Set verbosities
  HandleVerbosities();
  // Handle visualization
  HandleVisualization();
  // Start main G4 loop
  RunG4();
}

void G4Helper::HandleVerbosities()
{
  fUIManager->ApplyCommand("/run/verbose 0");      // max = 2
  fUIManager->ApplyCommand("/event/verbose 0");    // max = 2
  fUIManager->ApplyCommand("/tracking/verbose 0"); // max = 4
  G4HadronicProcessStore::Instance()->SetVerbose(0);
  fPhysicsList->GetOpticalPhysics()->GetBoundaryProcess()->SetVerboseLevel(0); // max 1
}

void G4Helper::RunG4()
{
  // Initialize photon table
  // This will help reduce overhead
  OpDetPhotonTable* photonTable = OpDetPhotonTable::CreateInstance();
  // Get config
  Configuration* config = Configuration::Instance();
  // Get steering table
  Configuration::SteeringTable steeringTable = config->GetSteeringTable();
  unsigned nEvents = steeringTable.size();
  // Initialize our anaylzer
  Analyzer analyzer(fSimulateOutputPath);
 
  //std::cout << "\nPress enter to start running G4...\n";
  //std::cin.get();
  for (unsigned e = 0; e < nEvents; e++)
  {
    G4cout << "\n****  EVENT #" << e << "  ****" << G4endl;
    // Reset the generator
    G4double r(0), thetaDeg(0), x(0), y(0), z(0), pixelSize(0);
    G4int    n(0);
    if (config->SourceMode() == "pixel")
    {
      // Get the pixel table
      PixelTable* pixelTable = PixelTable::Instance();
      const Pixel* pixel     = pixelTable->GetPixel(steeringTable[e].pixelID);

      r         = cm*pixel->R();
      thetaDeg  = deg*pixel->Theta();
      x         = cm*pixel->X();
      y         = cm*pixel->Y();
      z         = fDetector->WheelGeometry()->Thickness();
      n         = steeringTable[e].n;
      pixelSize = cm*pixel->Size(); 
      std::cout << "pixelID = " << pixel->ID() << std::endl;
    }
    else 
    {
      r         = steeringTable[e].r;
      thetaDeg  = steeringTable[e].thetaDeg; 
      x         = steeringTable[e].x;
      y         = steeringTable[e].y; 
      z         = fDetector->WheelGeometry()->Thickness();
      n         = steeringTable[e].n;
    }
    fGeneratorAction->Reset(r, thetaDeg, x, y, z, n, pixelSize);
  
    // Start run!
    fRunManager->BeamOn(1);
    //std::cin.get();

    // Reconstruct?
    if (Configuration::Instance()->Reconstruct())
    {
      std::cout << "\nReconstructing...\n";

      // Pass data and pixelization schema
      auto tempData = photonTable->GetPhotonsDetected();
      std::map<unsigned, unsigned> data;
      for (const auto& d : tempData) data.emplace(d.first, d.second.size());

      // Event display mode
      if (Configuration::Instance()->EvdMode())
      {
        std::ofstream outfile(Configuration::Instance()->DAQFilePath().c_str());
        outfile << x/cm << " " << y/cm << "\n";
        for (const auto& d : data) outfile << d.second << " ";
      }

      PixelTable* pixelTable = PixelTable::Instance();

      // Disk radius returns in mm, convert to cm
      fReconstructor.Initialize(data, pixelTable->GetPixels(), fDetector->WheelGeometry()->Radius()/10.);
      fReconstructor.Reconstruct(); 
      fReconstructor.MakePlots(fRecoAnaTreePath);
    }
    // Fill our ntuple
    analyzer.Fill(e);
    // Clear the photon table!
    photonTable->Reset();
  }
  //std::cout << "\nDone! Press enter to exit...\n";
  //std::cin.get();
}

void G4Helper::HandleVisualization()
{
  #ifdef G4VIS_USE
  if (fShowVis) 
  {
    fVisManager = new G4VisExecutive();
    fVisManager->SetVerboseLevel(0); // max = 6
    fVisManager->Initialize();
    std::string command = "/control/execute " + fVisMacroPath;
    fUIManager->ApplyCommand(command);
  }
  #endif
}

}

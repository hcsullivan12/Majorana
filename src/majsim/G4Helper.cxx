/**
 * @file G4Helper.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Helper class to interface with and run Geant4.
 * @date 07-04-2019
 * 
 */
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include "majsim/G4Helper.h"
#include "majutil/OpDetPhotonTable.h"
#include "majutil/PixelTable.h"
#include "majsim/Configuration.h"
#include "majsim/SimAnalyzer.h"
#include "G4VisManager.hh"
#include <assert.h>

#include <TFile.h>


namespace majsim
{

G4Helper* G4Helper::instance = 0;

//------------------------------------------------------------------------
G4Helper* G4Helper::CreateInstance()
{
  if (instance == 0)
  {
    static G4Helper g4Helper;
    instance = &g4Helper;
  }
  return instance;
}

//------------------------------------------------------------------------
G4Helper* G4Helper::Instance()
{
  assert(instance);
  return instance;
}

//------------------------------------------------------------------------
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

//------------------------------------------------------------------------
G4Helper::~G4Helper()
{
  //if (fUIManager)  delete fUIManager;
  //#ifdef G4VIS_USE

  if (fVisManager) delete fVisManager;
  //#endif
  if (fRunManager) delete fRunManager;
}

//------------------------------------------------------------------------
Int_t G4Helper::StartG4(G4UIExecutive* ui)
{
  // Set verbosities
    HandleVerbosities();
    fActionInitialization->DumpResults();
    //Handling Visualization
    HandleVisualization(ui);
  // Start main G4 loop
    return RunG4();



}

//------------------------------------------------------------------------
void G4Helper::HandleVerbosities()
{
  fUIManager->ApplyCommand("/run/verbose 0");      // max = 2
  fUIManager->ApplyCommand("/event/verbose 0");    // max = 2
  fUIManager->ApplyCommand("/tracking/verbose 0"); // max = 4
  G4HadronicProcessStore::Instance()->SetVerbose(0);
  //fPhysicsList->GetOpticalPhysics()->GetBoundaryProcess()->SetVerboseLevel(0); // max 1
}

//------------------------------------------------------------------------
Int_t G4Helper::RunG4()
{
    // Get config
    Configuration* config = Configuration::Instance();
    // Initialize photon table
    // Get steering table
    Configuration::SteeringTable steeringTable = config->GetSteeringTable();
    size_t nEvents = steeringTable.size();

    // Handle visualization
   // Run G4 nEvents times
    int StartEvent=0;

    size_t TotalEvents=nEvents;

    std::string PathForUpdate;

    int status=0;
    if (nEvents>100 and !fShowVis)
   {
       //Defining First Child
       pid_t f1 =fork();

       // Creating Second Child and GrandChild
       pid_t f2=fork();
       if(f1>0 && f2>0)
       {
           StartEvent=3*(TotalEvents/4);
           nEvents=TotalEvents;

           PathForUpdate=config->SimulateOutputPath()+"_p4.root";

           //wait(NULL);
           //wait(NULL);
           std::cout<<"Processing Parent ID "<<getppid() <<" "<<getpid()<<std::endl;

       }
       else if(f1==0 && f2>0) // 1st Child Process
       {
           StartEvent=(TotalEvents/2);
           nEvents=3*(TotalEvents/4);
           PathForUpdate=PathForUpdate;

           PathForUpdate=config->SimulateOutputPath()+"_p3.root";
           //wait(NULL);
           std::cout<<"Processing first Child Event "<<getppid() <<" "<<getpid()<<std::endl;
           status=1;
       }else if(f1>0 && f2==0) // Second Child Process
       {

           StartEvent=TotalEvents/4;
           nEvents=(TotalEvents/2);
           PathForUpdate=PathForUpdate;
           std::cout<<"Processing Second Child Event "<< getppid() <<" "<<getpid()<<std::endl;
           PathForUpdate=config->SimulateOutputPath()+"_p2.root";
           status=1;
       }else if(f1==0 && f2==0) // third Child Process
       {

           StartEvent=0;
           nEvents=TotalEvents/4;
           PathForUpdate=PathForUpdate;
           std::cout<<"Processing Third Child Event "<< getppid() <<" "<<getpid()<<std::endl;
           PathForUpdate=config->SimulateOutputPath()+"_p1.root";
           status=1;


       }
       else {std::cout<<"Fork Failed"<<std::endl; return 0;}

    } else
        {
            PathForUpdate=config->SimulateOutputPath()+".root";
        }


        // Make new analyzer
        SimAnalyzer analyzer(PathForUpdate.c_str());
        // This will help reduce overhead
        majutil::OpDetPhotonTable* photonTable = majutil::OpDetPhotonTable::CreateInstance(config->NMPPCs());


        for (size_t e = StartEvent; e < nEvents; e++)
  {

    std::cout << "\n****  EVENT #" << e << "  ****" << std::endl;

    // Reset the generator
    if (config->SourceMode() == "pixel")
    {
      // Get the pixel table
      majutil::PixelTable* pixelTable = majutil::PixelTable::Instance();
      const majutil::Pixel* pixel     = pixelTable->GetPixel(steeringTable[e].pixelID);

      G4double r         = CLHEP::cm*pixel->R();
      G4double thetaDeg  = CLHEP::deg*pixel->Theta();
      G4double x         = CLHEP::cm*pixel->X();
      G4double y         = CLHEP::cm*pixel->Y();
      G4double z         = fDetector->WheelGeometry()->Thickness();
      G4int    n         = steeringTable[e].n;
      G4double pixelSize = CLHEP::cm*pixel->Size(); 
      fGeneratorAction->Reset(r, thetaDeg, x, y, z, n, pixelSize);
    }
    else 
    {
      auto r         = steeringTable[e].r;
      auto thetaDeg  = steeringTable[e].thetaDeg; 
      auto x         = steeringTable[e].x;
      auto y         = steeringTable[e].y; 
      auto z         = fDetector->WheelGeometry()->Thickness();
      auto n         = steeringTable[e].n;
      fGeneratorAction->Reset(r, thetaDeg, x, y, z, n);
    }

      // Start run!
      if (!fShowVis) fRunManager->BeamOn(1);
    // Update DAQ file if in evd mode
    if (config->EvdMode())
    {
      std::ofstream outfile(config->DAQFilePath().c_str());
      auto data = photonTable->GetPhotonsDetected();
      for (const auto& d : data) outfile << d.second.size() << " ";
    }
    
    // Write true distribution
    //TFile f(config->SimulateOutputPath().c_str(), "UPDATE");
    //fGeneratorAction->GetPrimHist()->Write();
    //f.Close();

    // Fill our ntuple
      if (!fShowVis) analyzer.Fill(e);
    // Clear the photon table!
    photonTable->Reset();
  
    // Sleep if in evd mode
    if (config->EvdMode()) sleep(3);


  }

    return status;

}

//------------------------------------------------------------------------
void G4Helper::HandleVisualization(G4UIExecutive* ui)
{
  //#ifdef G4VIS_USE  Version 10.2

  if (fShowVis)
  {
      fVisManager = new G4VisExecutive();
    fVisManager->SetVerboseLevel(0); // max = 6
    fVisManager->Initialize();
    std::string command = "/control/execute " + fVisMacroPath;
    fUIManager->ApplyCommand(command);
    ui->SessionStart();
    //delete ui;

  }

    //#endif Version 10.2


}

}

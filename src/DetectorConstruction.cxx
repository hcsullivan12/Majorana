// 
// File: DetectorConstruction.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to construct our detector.
//

#include "DetectorConstruction.h"
#include "Configuration.h"
#include "MaterialManager.h"

#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"

namespace majorana
{

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fWheel(NULL),
  fVolWorld(NULL),
  fPVWorld(NULL)
{
  // Pass configuration to our other volumes
  Configuration* config = Configuration::Instance();
  fWheel = new Wheel(config->NMPPCs(),
                                 config->MPPCHalfLength(),
                                 config->DiskRadius(), 
                                 config->DiskThickness());
}

DetectorConstruction::~DetectorConstruction()
{
  if (fWheel) delete fWheel;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  InitializeMaterials();
  InitializeDetector();

  if (!fPVWorld) 
  {
    G4cerr << "DetectorConstruction::Construct() Error! WorldPV not initialized!\n";
  }

  return fPVWorld;
}

void DetectorConstruction::InitializeMaterials()
{
  // Construct materials
  MaterialManager* matMan = MaterialManager::Instance();

  if (!matMan) 
  {
    G4cerr << "DetectorConstruction::InitializeMaterials() Error! Material manager not initialized!\n";
  }
  matMan->ConstructMaterials();
}

void DetectorConstruction::InitializeDetector()
{
  if (!fWheel) 
  {
    G4cerr << "DetectorConstruction::InitializeDetector() Error! Disk volume not initialized!\n";
  }
  // Get instance of material manager
  MaterialManager* matMan = MaterialManager::Instance();

  //**** World
  G4Box* solWorld = new G4Box("solWorld", 
                               fWheel->Radius()*4, 
                               fWheel->Radius()*4, 
                               fWheel->Thickness()*4);
  fVolWorld    = new G4LogicalVolume(solWorld, 
                                     matMan->FindMaterial("G4_AIR"), 
                                     "volWorld");
  fPVWorld = new G4PVPlacement(0, 
                               G4ThreeVector(), 
                               fVolWorld, 
                               fVolWorld->GetName(), 
                               0, 
                               false, 
                               0);

  // vis
  G4Colour worldC(0,0,0);
  G4VisAttributes* worldVA = new G4VisAttributes(worldC);
  worldVA->SetForceWireframe(true);
  fVolWorld->SetVisAttributes(worldVA); 

  //**** Wheel
  fWheel->ConstructVolume();
}

void DetectorConstruction::ConstructSDandField()
{
  if (!fWheel) return;
 
  // MPPC SD
  if (!fMPPCSD.Get()) 
  {
    //G4cout << "Construction mppcSD..." << G4endl;
    MPPCSD* mppcSD = new MPPCSD("mppcSD");
    fMPPCSD.Put(mppcSD);
  }
  G4SDManager::GetSDMpointer()->AddNewDetector(fMPPCSD.Get());
  SetSensitiveDetector(fWheel->GetMPPCVolume(), fMPPCSD.Get());
}
}

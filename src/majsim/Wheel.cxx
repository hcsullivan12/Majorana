/**
 * @file Wheel.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief The SiPM wheel geometry
 * @date 07-04-2019
 * 
 */

#include "majsim/Wheel.h"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"

namespace majsim
{

Wheel::Wheel(const unsigned& nMPPCs, 
                         const G4double& mppcHalfL,
                         const G4double& diskRadius,
                         const G4double& diskThickness)
 : fDiskRadius(diskRadius), 
   fDiskThickness(diskThickness),
   fNMPPCs(nMPPCs),
   fMPPCHalfL(mppcHalfL),
   fVolDisk(NULL),
   fVolMPPC(NULL)
{
    config= Configuration::Instance();
}

Wheel::~Wheel()
{}

//***********************
// Construct Volume
//
void Wheel::ConstructVolume()
{
  MaterialManager* matMan = MaterialManager::Instance();


  // Construct the TPB volume
  G4Tubs *tpbLayerSolid = new G4Tubs("solTPB", 0, fDiskRadius, 0.5 * mm, 0, twopi);
  fTPBVolLogic = new G4LogicalVolume(tpbLayerSolid, matMan->FindMaterial("TPB"), "volTPB");

  //****
  // Construct disk volume
  //****
  G4Tubs* solDisk = new G4Tubs("solDisk", 0, fDiskRadius, (fDiskThickness/2.0), 0, twopi);
  fVolDisk        = new G4LogicalVolume(solDisk, matMan->FindMaterial("Acrylic"), "volDisk");

  //****
  // Construct mppcs
  //****
  G4double mppcThickness    = 1.0*mm; 
  G4Box* solMPPC = new G4Box("solMPPC", (mppcThickness/2.0), fMPPCHalfL, fMPPCHalfL);
  fVolMPPC       = new G4LogicalVolume(solMPPC, matMan->FindMaterial("G4_Al"), "volMPPC");

  G4cout<<"Before Placing Volumes"<<G4endl;
  // Place the volumes
  PlaceVolumes();
  // Handle surfaces
  HandleSurfaces();
  // Handle vis effects
  HandleVisAttributes();

  //G4LogicalBorderSurface::DumpInfo();
}

//***********************
// Place Volumes
//
void Wheel::PlaceVolumes()
{
    G4cout <<" before Getting Volumes"<<G4endl;
    // Get the world volume and mppc
  G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();
  G4LogicalVolume* volWorld = lvStore->GetVolume("volWorld");

    // Place disk
  // xy plane is the bottom surface
  G4ThreeVector transVec(0,0,(fDiskThickness/2.0));
  new G4PVPlacement(0, transVec, fVolDisk, fVolDisk->GetName(), volWorld, false, 0);


  //Placing TPB layer
  G4ThreeVector transVec2(0,0,0);
  if(config->NMPPCs()<32)
      transVec2[2]=fDiskThickness;

  new G4PVPlacement(0, transVec2, fTPBVolLogic, fTPBVolLogic->GetName(), volWorld, false, 0);


  // Place mppcs around the disk
  for (unsigned m = 1; m <= fNMPPCs; m++)
  {
    double angSepDeg = 360.0/fNMPPCs;
    double thetaDeg  = (m-1)*angSepDeg;
    G4double offsetR = fDiskRadius + ((G4Box*)fVolMPPC->GetSolid())->GetXHalfLength();

    // Convert to cartesian coordinates 
    G4double x(0), y(0), z(fDiskThickness/2.0);
    ConvertToCartesian(x, y, offsetR, thetaDeg);
    
    // Apply rotation
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(-1*thetaDeg*deg);
    // Apply translation
    G4ThreeVector xyzVec(x, y, z);
    G4ThreeVector rtzVec(offsetR, thetaDeg*deg, z);
    
    new G4PVPlacement(zRot, xyzVec, fVolMPPC, fVolMPPC->GetName()+std::to_string(m), volWorld, true, m-1);
    fMPPCPositions.push_back(rtzVec);
  }
}

//***********************
// Handle surfaces
//
void Wheel::HandleSurfaces()
{
  MaterialManager* matMan = MaterialManager::Instance();
  Configuration*   config = Configuration::Instance();

  // Get the world physical volume
  G4PhysicalVolumeStore* pvStore = G4PhysicalVolumeStore::GetInstance();
  G4VPhysicalVolume* pvWorld = pvStore->GetVolume("volWorld");
  G4VPhysicalVolume* pvDisk  = pvStore->GetVolume("volDisk");
  G4VPhysicalVolume* pvTPB  = pvStore->GetVolume("volTPB");


    //****
  // Argon surface
  //****
  G4double roughness = config->SurfaceRoughness();
    G4OpticalSurface* DiskSurface(NULL);
    G4OpticalSurface* diskToTPBSurface(NULL);
    G4OpticalSurface* TPBSurface(NULL);
    G4OpticalSurface* airSurface(NULL);


    airSurface = new G4OpticalSurface("AirSurface", glisur, ground, dielectric_dielectric, roughness);
    DiskSurface = new G4OpticalSurface("DiskTop", glisur,ground, dielectric_dielectric, roughness);
    diskToTPBSurface = new G4OpticalSurface("DiskBottom",glisur, ground ,dielectric_dielectric, roughness);
    TPBSurface = new G4OpticalSurface("TPBsurface", glisur,ground, dielectric_dielectric,roughness);
    if(config->NMPPCs()<32)
    {
        new G4LogicalBorderSurface("TPBLayerBack",pvTPB,pvWorld,TPBSurface);
        new G4LogicalBorderSurface("DiskBorderSurfaceOut", pvDisk,  pvTPB, airSurface);
        new G4LogicalBorderSurface("DiskBorderSurfaceIn",  pvWorld, pvDisk,  airSurface);
    }
    else{
        new G4LogicalBorderSurface("DiskBorderSurfaceOut", pvDisk,  pvWorld, DiskSurface);
        new G4LogicalBorderSurface("DiskBorderSurfacein",pvTPB,pvDisk,diskToTPBSurface);
        new G4LogicalBorderSurface("TPBLayerBack",pvWorld,pvTPB,TPBSurface);
    }

    //new G4LogicalBorderSurface("TPBLayerFront",pvWorld,,ArgonSurface);

    //new G4LogicalBorderSurface("TPBLayer",pvWorld,pvTPB,TPBSurface);

  //new G4LogicalBorderSurface("ArgonSurface",)
  //**** 
  // MPPC coupling surface
  //****
  G4OpticalSurface*          mppcCouplingSurface(NULL);
  G4MaterialPropertiesTable* mppcCouplingMPT(NULL);  
  mppcCouplingSurface = new G4OpticalSurface("DiskCouplingSurface", unified,polished, dielectric_metal, 1.0); // smooth
  mppcCouplingMPT = matMan->FindMaterial("G4_Al")->GetMaterialPropertiesTable();
  mppcCouplingSurface->SetMaterialPropertiesTable(mppcCouplingMPT);

  // Loop over the mppc PV to create border surfaces
  // Probably better than skin surface
  for (unsigned mppc = 1; mppc <= fNMPPCs; mppc++)
  {
    std::string mppcName      = fVolMPPC->GetName()+std::to_string(mppc);
    G4VPhysicalVolume* pvMPPC = pvStore->GetVolume(mppcName);
 
    // Disk MPPC surface
    std::string diskMPPCSurfaceNameOut = "DiskMPPCSurfaceOut" + std::to_string(mppc);
    std::string diskMPPCSurfaceNameIn  = "DiskMPPCSurfaceIn"  + std::to_string(mppc);

    new G4LogicalBorderSurface(diskMPPCSurfaceNameOut, pvDisk, pvMPPC, mppcCouplingSurface);
    new G4LogicalBorderSurface(diskMPPCSurfaceNameIn,  pvMPPC, pvDisk, mppcCouplingSurface);
  }
}

//***********************
// Handle Vis attributes
//
void Wheel::HandleVisAttributes()
{
    G4LogicalVolumeStore* pvStore = G4LogicalVolumeStore::GetInstance();
    G4LogicalVolume* pvWorld = pvStore->GetVolume("volWorld");
  // Disk
  G4VisAttributes* diskVA = new G4VisAttributes(G4Colour(1,1,1));
  diskVA->SetForceSolid(true);
  fVolDisk->SetVisAttributes(diskVA); 
  // MPPC
  G4VisAttributes* mppcVA = new G4VisAttributes(G4Colour(1,0,0));
  mppcVA->SetForceSolid(true);
  fVolMPPC->SetVisAttributes(mppcVA);
    G4VisAttributes* TPBLayer = new G4VisAttributes(G4Colour(0,0,1));
    TPBLayer->SetForceSolid(true);
    fTPBVolLogic->SetVisAttributes(TPBLayer);

    G4VisAttributes* World = new G4VisAttributes(G4Colour(1,1,1));
    World->SetForceWireframe(true);
    pvWorld->SetVisAttributes(World);

}

}

// 
// File: WheelVolume.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to construct disk volume.
//

#include "WheelVolume.h"
#include "Configuration.h"
#include "MaterialManager.h"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4PhysicalVolumeStore.hh"

namespace majorana
{

WheelVolume::WheelVolume(const unsigned& nMPPCs, 
                       const G4double& mppcArea,
                       const G4double& diskRadius,
                       const G4double& diskThickness)
 : m_diskRadius(diskRadius), 
   m_diskThickness(diskThickness),
   m_nMPPCs(nMPPCs),
   m_mppcArea(mppcArea),
   m_diskSolid(NULL),
   m_diskLV(NULL),
   m_diskPV(NULL),
   m_mppcSolid(NULL),
   m_mppcLV(NULL),
   m_couplingSolid(NULL),
   m_couplingLV(NULL)
{}

WheelVolume::~WheelVolume()
{}

void WheelVolume::ConstructVolume(G4VPhysicalVolume* worldPV,
                                  G4LogicalVolume*   worldLV)
{
  MaterialManager* materialManager = MaterialManager::Instance();

  //****
  // Construct disk volume
  //****
  m_diskSolid = new G4Tubs("DiskSolid", 0,
                           m_diskRadius*cm, (m_diskThickness/2.0)*cm, 0, twopi);
  m_diskLV    = new G4LogicalVolume(m_diskSolid, 
                                    materialManager->FindMaterial("Acrylic"), 
                                   "DiskLV");  
  // xy plane is the bottom surface
  G4ThreeVector transVec(0,0,(m_diskThickness/2.0)*cm);
  m_diskPV = new G4PVPlacement(0, transVec, m_diskLV, "Disk", worldLV, false, 0);

  //****
  // Construct mppcs
  //****
  m_mppcHalfL                = std::sqrt(m_mppcArea);
  G4double mppcThickness     = 0.15; // 1.5 mm
  G4double couplingThickness = mppcThickness + 0.1; // + 1 mm 
  G4double couplingHalfL     = m_mppcHalfL + 0.1;
  m_mppcSolid = new G4Box("MPPCSolid", 
                          (mppcThickness/2.0)*cm, m_mppcHalfL*cm, m_mppcHalfL*cm);
  m_mppcLV    = new G4LogicalVolume(m_mppcSolid, 
                                    materialManager->FindMaterial("G4_Si"), "MPPCLV");  

  // Place mppcs around the disk
  for (unsigned m = 1; m <= m_nMPPCs; m++)
  {
    G4double angSepDeg = 360.0/m_nMPPCs;
    G4double thetaDeg  = (m - 1)*angSepDeg;
    // Offset, small offset "connects" the volumes
    G4double offsetR = m_diskRadius + (mppcThickness/2.0) - 0.1;
    PlaceMPPC(worldLV, offsetR, thetaDeg, m);
  }

  // Handle surfaces
  HandleSurfaces(worldPV);
  // Handle vis effects
  HandleVisAttributes();

  //G4LogicalBorderSurface::DumpInfo();
}

void WheelVolume::PlaceMPPC(G4LogicalVolume* worldLV,
                            const G4double&  r,
                            const G4double&  thetaDeg,
                            const unsigned&  m)
{
  // Convert to cartesian coordinates 
  G4double x(0), y(0), z(m_diskThickness/2.0);
  ConvertToCartesian(x, y, r, thetaDeg);

  // Apply rotation
  G4RotationMatrix* zRot = new G4RotationMatrix;
  zRot->rotateZ(-1*thetaDeg*deg);  
  // Apply translation
  G4ThreeVector transVec(x*cm, y*cm, z*cm);
  // Name them
  std::string mppcName     = "mppcs/mppc" + std::to_string(m);    
  std::string couplingName = "couplings/coupling" + std::to_string(m);

  //G4PVPlacement* couplingPV = NULL;
  G4PVPlacement* mppcPV     = NULL; 
  mppcPV     = new G4PVPlacement(zRot,transVec,m_mppcLV,mppcName,worldLV,false,m);
 
  m_mppcPositions.push_back(transVec);
}

void WheelVolume::HandleSurfaces(G4VPhysicalVolume* worldPV)
{
  MaterialManager* materialManager = MaterialManager::Instance();
  Configuration*   config          = Configuration::Instance();

  //**** 
  // Air surface
  //****
  G4OpticalSurface* airSurface(NULL);
  float roughness = config->SurfaceRoughness();
  airSurface = new G4OpticalSurface("AirSurface", glisur, ground, dielectric_dielectric, roughness);
  new G4LogicalBorderSurface("DiskBorderSurfaceOut",
                             m_diskPV, worldPV, airSurface);
  new G4LogicalBorderSurface("DiskBorderSurfaceIn",
                             worldPV, m_diskPV, airSurface); 

  //**** 
  // MPPC coupling surface
  //****
  G4OpticalSurface*          mppcCouplingSurface(NULL);
  G4MaterialPropertiesTable* mppcCouplingMPT(NULL);  
  mppcCouplingSurface = new G4OpticalSurface("DiskCouplingSurface",
                                             glisur, polished, 
                                             dielectric_metal, 1.0); // smooth
  mppcCouplingMPT = materialManager->FindMaterial("G4_Si")->GetMaterialPropertiesTable();
  mppcCouplingSurface->SetMaterialPropertiesTable(mppcCouplingMPT);

  // Loop over the mppc PV to create border surfaces
  // Probably better than skin surface
  G4PhysicalVolumeStore* pvStore = G4PhysicalVolumeStore::GetInstance();
  for (unsigned mppc = 1; mppc <= m_nMPPCs; mppc++)
  {
    std::string mppcName     = "mppcs/mppc" + std::to_string(mppc);
    std::string couplingName = "couplings/coupling" + std::to_string(mppc);

    G4VPhysicalVolume* mppcPV     = pvStore->GetVolume(mppcName);
   // G4VPhysicalVolume* couplingPV = pvStore->GetVolume(couplingName);
 
    // Disk MPPC surface
    std::string diskMPPCSurfaceNameOut = "DiskMPPCSurfaceOut" + std::to_string(mppc);
    std::string diskMPPCSurfaceNameIn  = "DiskMPPCSurfaceIn"  + std::to_string(mppc);
    new G4LogicalBorderSurface(diskMPPCSurfaceNameOut,
                               m_diskPV, mppcPV, mppcCouplingSurface);
    new G4LogicalBorderSurface(diskMPPCSurfaceNameIn,
                               mppcPV, m_diskPV, mppcCouplingSurface); 

  }
}

void WheelVolume::HandleVisAttributes()
{
  // Disk
  G4VisAttributes* diskVA = new G4VisAttributes(G4Colour(1,1,1));
  diskVA->SetForceWireframe(true);
  m_diskLV->SetVisAttributes(diskVA); 
  // MPPC
  G4VisAttributes* mppcVA = new G4VisAttributes(G4Colour(1,0,0));
  mppcVA->SetForceSolid(true);
  m_mppcLV->SetVisAttributes(mppcVA); 
  /*// Coupling
  G4VisAttributes* couplingVA = new G4VisAttributes(G4Colour(1,0,0,0.2));
  couplingVA->SetForceWireframe(true);
  m_couplingLV->SetVisAttributes(couplingVA);*/
}

}

// 
// File: DiskVolume.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to construct disk volume.
//

#include "DiskVolume.h"

#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalBorderSurface.hh"

namespace majorana
{

DiskVolume::DiskVolume(const unsigned& nMPPCs, 
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
   m_mppcLV(NULL)
{}

DiskVolume::~DiskVolume()
{}

void DiskVolume::ConstructVolume(G4VPhysicalVolume* worldPV,
                                 G4LogicalVolume*   worldLV)
{
  MaterialManager* materialManager = MaterialManager::Instance();

  // Construct disk volume
  m_diskSolid      = new G4Tubs("DiskSolid", 
                                 0, 
                                 m_diskRadius*cm, 
                                 (m_diskThickness/2.0)*cm, 
                                 0, 
                                 twopi);
  m_diskLV = new G4LogicalVolume(m_diskSolid, 
                                 materialManager->FindMaterial("Acrylic"), 
                                "DiskLV");  
  G4ThreeVector transVec(0,0,(m_diskThickness/2.0)*cm);
  m_diskPV = new G4PVPlacement(0, 
                               transVec, 
                               m_diskLV, 
                              "Disk", 
                               worldLV, 
                               false, 
                               0);

  // Construct mppcs
  m_mppcSideLength = std::sqrt(m_mppcArea);  
  m_mppcSolid      = new G4Box("mppcSolid", 
                               m_mppcSideLength*cm, 
                               m_mppcSideLength*cm, 
                               (m_mppcSideLength/2.0)*cm);
  m_mppcLV = new G4LogicalVolume(m_mppcSolid, 
                                 materialManager->FindMaterial("G4_Si"), 
                                 "mppcLV"); 
  // Place mppcs around the disk
  for (unsigned m = 1; m <= m_nMPPCs; m++)
  {
    G4double angSepDeg = 360.0/m_nMPPCs;
    G4double r         = m_diskRadius;
    G4double thetaDeg  = (m - 1)*angSepDeg;
    // Offset 
    r = r + (m_mppcSideLength/2.0);
    PlaceMPPC(worldLV, r, thetaDeg, m-1);
  }

  // Handle surfaces
  HandleSurfaces(worldPV);
  // Handle vis effects
  HandleVisAttributes();

  //G4LogicalBorderSurface::DumpInfo();
}

void DiskVolume::PlaceMPPC(G4LogicalVolume* worldLV,
                           const G4double&  r,
                           const G4double&  thetaDeg,
                           const unsigned&  m)
{
  G4double x(0), y(0), z(m_diskThickness/2.0);
  ConvertToCartesian(x, y, r, thetaDeg);

  // Apply rotation
  G4RotationMatrix* zRot = new G4RotationMatrix;
  zRot->rotateZ(thetaDeg*deg);  
  // Apply translation
  G4ThreeVector transVec(x*cm, y*cm, z*cm);
  std::string name = "mppcs/mppc" + std::to_string(m);    
  G4PVPlacement* mppcPV = new G4PVPlacement(zRot, 
                                            transVec, 
                                            m_mppcLV, 
                                            name, 
                                            worldLV, 
                                            false, 
                                            m);
}


void DiskVolume::HandleSurfaces(G4VPhysicalVolume* worldPV)
{
  MaterialManager* materialManager = MaterialManager::Instance();

  //**** Simple mppc surface 
  G4OpticalSurface* mppcOS = new G4OpticalSurface("mppcOS", 
                                                  glisur,
                                                  ground, 
                                                  dielectric_dielectric);
  G4MaterialPropertiesTable* mppcMPT = 
     materialManager->FindMaterial("G4_Si")->GetMaterialPropertiesTable();
  mppcOS->SetMaterialPropertiesTable(mppcMPT);
  new G4LogicalSkinSurface("mppcSS", m_mppcLV, mppcOS);

  //**** Air surface
  // TODO: Surface roughness
  G4OpticalSurface* airOS = new G4OpticalSurface("airOS",
                                                 glisur,
                                                 ground,
                                                 dielectric_dielectric,
                                                 0.0);
  G4MaterialPropertiesTable* surfaceProp = 
    materialManager->FindMaterial("G4_AIR")->GetMaterialPropertiesTable();
  airOS->SetMaterialPropertiesTable(surfaceProp);
  new G4LogicalBorderSurface("airBS",
                              m_diskPV,
                              worldPV,
                              airOS);
}

void DiskVolume::HandleVisAttributes()
{
  // Disk
  G4VisAttributes* diskVA = new G4VisAttributes(G4Colour(1,1,1));
  diskVA->SetForceWireframe(true);
  m_diskLV->SetVisAttributes(diskVA); 
  // MPPC
  G4VisAttributes* mppcVA = new G4VisAttributes(G4Colour(1,0,0));
  mppcVA->SetForceSolid(true);
  m_mppcLV->SetVisAttributes(mppcVA); 
}

}

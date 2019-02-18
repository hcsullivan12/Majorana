// 
// File: Wheel.h
//
// Author: Hunter Sullivan
//
// Discription: Class to construct wheel volume.
//

#ifndef WHEELVOLUME_H
#define WHEELVOLUME_H

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"

namespace majorana
{

class Wheel 
{
  public:
    Wheel(const unsigned& nMPPCs,                                   
               const G4double&  mppcHalfL,
               const G4double&  diskRadius,
               const G4double&  diskThickness);
    ~Wheel();

    void ConstructVolume();
    void PlaceVolumes();

    const G4double   Radius()    const { return fDiskRadius; };
    const G4double   Thickness() const { return fDiskThickness; }; 
    const unsigned   NMPPCs()    const { return fNMPPCs; };

    G4LogicalVolume* GetMPPCVolume() const { return fVolMPPC; };
    const std::vector<G4ThreeVector> MPPCPositions() const { return fMPPCPositions; };

  private: 
    void HandleSurfaces();
    void HandleVisAttributes();
    inline void ConvertToCartesian(G4double&       x, 
                                   G4double&       y, 
                                   const G4double& r, 
                                   const double&   thetaDeg)
    {
      x = r*std::cos(thetaDeg*pi/180);
      y = r*std::sin(thetaDeg*pi/180);
    };

    G4double           fDiskRadius;
    G4double           fDiskThickness;
    G4double           fMPPCHalfL;
    unsigned           fNMPPCs;

    G4LogicalVolume*   fVolDisk;
    G4LogicalVolume*   fVolMPPC;

    std::vector<G4ThreeVector> fMPPCPositions;

};
}

#endif

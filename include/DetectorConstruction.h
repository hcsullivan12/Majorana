// 
// File: DetectorConstruction.h
//
// Author: Hunter Sullivan
//
// Discription: Class to construct our detector.
//

#ifndef DETECTORCONSTRUCTION_H
#define DETECTORCONSTRUCTION_H

#include "Wheel.h"
#include "MPPCSD.h"

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Cache.hh"

namespace majorana
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void               ConstructSDandField();
   
    const Wheel* WheelGeometry() const { return fWheel; };

  private:
    void InitializeMaterials();
    void InitializeDetector();
       
    G4LogicalVolume*   fVolWorld; 
    G4VPhysicalVolume* fPVWorld;
    Wheel*       fWheel;
    G4Cache<MPPCSD*>   fMPPCSD;
};
}

#endif

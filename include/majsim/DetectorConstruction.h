/**
 * @file DetectorConstruction.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Constructs the world and SiPM wheel geometries.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_DETECTORCONSTRUCTION_H
#define MAJSIM_DETECTORCONSTRUCTION_H

#include "majsim/Wheel.h"

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Cache.hh"
#include "majsim/Configuration.h"
#include "majsim/MaterialManager.h"

namespace majsim
{
class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void               ConstructSDandField();
   
    const Wheel* WheelGeometry() const { return fWheel; };
    const G4VPhysicalVolume* GetPVWorldGeometry() const { return fPVWorld; };
    const G4LogicalVolume* GetVolWorldGeometry() const { return fVolWorld; };


private:

    /**
     * @brief Initialize materials to be used for our geometries.
     * 
     */
    void InitializeMaterials();

    /**
     * @brief Initialize the individual geometries.
     * 
     */
    void InitializeDetector();
       
    G4LogicalVolume*   fVolWorld; ///< Pointer to the world logical volume
    G4VPhysicalVolume* fPVWorld;  ///< Pointer to world physical volume
    Wheel*             fWheel;    ///< The SiPM wheel geometry
    Configuration*     config;
};
}

#endif

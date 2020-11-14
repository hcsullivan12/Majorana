/**
 * @file PhysicsList.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to intialize the physics to be implemented.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_PHYSICSLIST_H
#define MAJSIM_PHYSICSLIST_H

//#include "majsim/OpticalPhysics.h"
#include "globals.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4UnitsTable.hh"
#include "G4ParticleTypes.hh"
#include "G4IonConstructor.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Radioactivation.hh"
#include "G4SystemOfUnits.hh"
#include "G4NuclideTable.hh"
#include "G4LossTableManager.hh"
#include "G4OpticalPhysics.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4NuclearLevelData.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4NuclideTable.hh"
#include <FTFP_BERT.hh>
#include "G4Scintillation.hh"
#include "G4VModularPhysicsList.hh"

class G4VPhysicsConstructor;

namespace majsim {

class PhysicsList : public G4VModularPhysicsList
{
  public:
    PhysicsList();
    ~PhysicsList();
    G4VModularPhysicsList * physicsList;


    virtual void ConstructParticle();
    virtual void ConstructProcess();
    virtual void SetCuts();

    //OpticalPhysics* GetOpticalPhysics() const { return fOpticalPhysics; };
 
private: 
    //OpticalPhysics* fOpticalPhysics;
};
}
#endif

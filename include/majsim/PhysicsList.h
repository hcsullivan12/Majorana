/**
 * @file PhysicsList.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to intialize the physics to be implemented.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_PHYSICSLIST_H
#define MAJSIM_PHYSICSLIST_H

#include "majsim/OpticalPhysics.h"

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include "FTFP_BERT.hh"

class G4VPhysicsConstructor;

namespace majsim {

class PhysicsList : public G4VModularPhysicsList
{
  public:
    PhysicsList();
    virtual ~PhysicsList();
 
    virtual void ConstructParticle();
    virtual void ConstructProcess();

    OpticalPhysics* GetOpticalPhysics() const { return fOpticalPhysics; };
 
private: 
    OpticalPhysics* fOpticalPhysics; 
};
}
#endif

// 
// File: PhysicsList.h
//
// Author: Hunter Sullivan
//
// Discription: Class to construct physics list.
//

#ifndef PHYSICSLIST_H
#define PHYSICSLIST_H

#include "OpticalPhysics.h"

#include "globals.hh"
#include "G4VModularPhysicsList.hh"
#include "FTFP_BERT.hh"

class G4VPhysicsConstructor;

namespace majorana {

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

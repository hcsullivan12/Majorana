// 
// File: PhysicsLists.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to construct physics list.
//

#include "PhysicsList.h"

namespace majorana {

PhysicsList::PhysicsList() 
 : G4VModularPhysicsList(),
    fOpticalPhysics(NULL)
{
  G4VModularPhysicsList* phys = new FTFP_BERT(0);
    
  fOpticalPhysics = new OpticalPhysics;
  RegisterPhysics(fOpticalPhysics);
}

PhysicsList::~PhysicsList()
{}

void PhysicsList::ConstructParticle()
{
  G4VModularPhysicsList::ConstructParticle(); 
}

void PhysicsList::ConstructProcess()
{
  G4VModularPhysicsList::ConstructProcess();
}

}

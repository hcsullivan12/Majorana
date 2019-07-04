/**
 * @file PhysicsList.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to intialize the physics to be implemented.
 * @date 07-04-2019
 * 
 */

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

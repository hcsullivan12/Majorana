/**
 * @file OpticalPhysics.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to initialize the optical physics to be implented. 
 * @date 07-04-2019
 * 
 */

#include "majsim/OpticalPhysics.h"

namespace majsim {

OpticalPhysics::OpticalPhysics()
 : G4VPhysicsConstructor("Optical"),
   fBoundaryProcess(NULL),
   fAbsorptionProcess(NULL)
{
  fAbsorptionProcess   = new G4OpAbsorption();
  fBoundaryProcess     = new OpBoundaryProcess();
}

OpticalPhysics::~OpticalPhysics() 
{}

void OpticalPhysics::ConstructParticle()
{
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void OpticalPhysics::ConstructProcess()
{  
  G4ProcessManager* pManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

  if (!pManager) 
  {
     std::ostringstream o;
     o << "Optical Photon without a Process Manager";
     G4Exception("OpticalPhysics::ConstructProcess()", "", FatalException,o.str().c_str());
  }

  pManager->AddDiscreteProcess(fAbsorptionProcess); 
  pManager->AddDiscreteProcess(fBoundaryProcess);
}

}

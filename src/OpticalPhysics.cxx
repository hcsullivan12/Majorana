// 
// File: OpticalPhysics.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to construct optical physics.
//

#include "OpticalPhysics.h"

namespace majorana {

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

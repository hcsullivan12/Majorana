// 
// File: OpticalPhysics.h
//
// Author: Hunter Sullivan
//
// Discription: Class for optical physics
//


#ifndef OPTICALPHYSICS_H
#define OPTICALPHYSICS_H

#include "OpBoundaryProcess.h"

#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4OpAbsorption.hh"
#include "G4VPhysicsConstructor.hh"

namespace majorana {

class OpticalPhysics : public G4VPhysicsConstructor
{
  public:
    OpticalPhysics();
    virtual ~OpticalPhysics();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    G4OpAbsorption*      GetAbsorptionProcess() const { return fAbsorptionProcess; };
    OpBoundaryProcess* GetBoundaryProcess()   const { return fBoundaryProcess;   };
 
private:

    G4OpAbsorption*      fAbsorptionProcess;
    OpBoundaryProcess*   fBoundaryProcess;
};
}
#endif

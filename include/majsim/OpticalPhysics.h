/**
 * @file OpticalPhysics.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to initialize the optical physics to be implented. 
 * @date 07-04-2019
 * 
 */


#ifndef MAJSIM_OPTICALPHYSICS_H
#define MAJSIM_OPTICALPHYSICS_H

#include "majsim/OpBoundaryProcessv2.h"

#include "G4OpticalPhoton.hh"
#include "G4ProcessManager.hh"
#include "G4OpAbsorption.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4eIonisation.hh"
#include "G4Scintillation.hh"
#include "G4Alpha.hh"
#include "G4OpMieHG.hh"
#include <G4OpRayleigh.hh>
#include "G4OpWLS.hh"
#include "G4LossTableManager.hh"
#include "G4EmSaturation.hh"
#include "G4VPhysicsConstructor.hh"
namespace majsim {

class OpticalPhysics : public G4VPhysicsConstructor
{
  public:
    OpticalPhysics();
    virtual ~OpticalPhysics();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    G4OpAbsorption*      GetAbsorptionProcess() const { return fAbsorptionProcess; };
    OpBoundaryProcessv2* GetBoundaryProcess()   const { return fBoundaryProcess;   };
 
private:

    G4OpAbsorption*      fAbsorptionProcess;
    OpBoundaryProcessv2*   fBoundaryProcess;
    G4OpWLS *fWLSProcess ;
    G4Scintillation *fScintProcess;

};
}
#endif

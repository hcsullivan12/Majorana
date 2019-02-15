// 
// File: SteppingAction.h
//
// Author: Hunter Sullivan
//
// Discription: Class for interface to stepping action.
//

#ifndef STEPPINGACTION_H
#define STEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4ProcessManager.hh"

namespace majorana {

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction();
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);
  
  private: 
    CLHEP::HepJamesRandom fRandomEngine;
};
}
#endif

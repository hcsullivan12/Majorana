/**
 * @file SteppingAction.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Stepping action.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_STEPPINGACTION_H
#define MAJSIM_STEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4ProcessManager.hh"
#include "G4OpticalPhoton.hh"
#include "G4UserSteppingAction.hh"
#include "G4ParticleTypes.hh"

#include "majutil/OpDetPhotonTable.h"



namespace majsim {

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction();
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);
    void PhDetectCount(){fPhDetecCount++;}
    void PhAbsCount(){fPhAbsCount++;}
    G4int GetPhDetectCount(){return fPhDetecCount;}
    G4int GetPhAbsCount(){return fPhAbsCount;}
    bool CheckDetection(const G4Track& theTrack, const G4Step& theStep);


private:
    CLHEP::HepJamesRandom fRandomEngine;
    G4int                    fPhAbsCount;
    G4int                    fPhDetecCount;
    majutil::OpDetPhotonTable* Counts ;

};
}
#endif

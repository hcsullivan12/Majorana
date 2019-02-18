
#ifndef OPDETECTIONPROCESS_H
#define OPDETECTIONPROCESS_H 

#include "globals.hh"
#include "G4OpticalPhoton.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpBoundaryProcess.hh"

class OpDetectionProcess : public G4VDiscreteProcess
{
  public:

    OpDetectionProcess(const G4String& processName = "OpDetectionProcess",
                       G4ProcessType type = fOptical);
    OpDetectionProcess(OpDetectionProcess &);

    virtual ~OpDetectionProcess();

    virtual G4bool IsApplicable(const G4ParticleDefinition&);

    //void SetStepMax(G4double);

    //G4double GetStepMax() {return fMaxChargedStep;};

    //virtual G4double PostStepGetPhysicalInteractionLength(const G4Track& track,
     //                                             G4double previousStepSize,
      //                                            G4ForceCondition* condition);

    virtual G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);

  protected:

    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*);

  private:

    // hide assignment operator as private
    OpDetectionProcess & operator=(const OpDetectionProcess &right);
    OpDetectionProcess(const OpDetectionProcess&);

  private:

    G4OpBoundaryProcessStatus theStatus;
    G4double kCarTolerance;
};

inline
G4bool OpDetectionProcess::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

#endif

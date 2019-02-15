
#include "G4Track.hh"
#include "G4VParticleChange.hh"
#include "G4OpProcessSubType.hh"
#include "G4GeometryTolerance.hh"
#include "G4ParallelWorldProcess.hh"

#include "old/OpDetectionProcess.h"


OpDetectionProcess::OpDetectionProcess(const G4String& aName,
                                       G4ProcessType type)
  : G4VDiscreteProcess(aName, type)
{
  G4cout << "\n\nCONSTRUCTOR\n\n";
  SetProcessSubType(fOpBoundary);

  theStatus = Undefined;
  kCarTolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
}

OpDetectionProcess::~OpDetectionProcess() { }

OpDetectionProcess::OpDetectionProcess(OpDetectionProcess& right) : G4VDiscreteProcess(right) { }

//void OpDetectionProcess::SetStepMax(G4double step) { fMaxChargedStep = step ; }
/*
G4double OpDetectionProcess::PostStepGetPhysicalInteractionLength(
                                              const G4Track&,
                                              G4double,
                                              G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;

  G4double ProposedStep = DBL_MAX;

  if ( fMaxChargedStep > 0.) ProposedStep = fMaxChargedStep;

   return ProposedStep;
}
*/

G4VParticleChange* OpDetectionProcess::PostStepDoIt(const G4Track& aTrack,
                                                    const G4Step&  aStep )
{
  G4cout << "\n\nHERERERE\n";

  theStatus = Undefined;

  const G4Step* pStep = &aStep;
  const G4Step* hStep = G4ParallelWorldProcess::GetHyperStep();
  if (hStep) pStep = hStep;
  G4bool isOnBoundary =
                (pStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary);

  if (isOnBoundary) 
  {
     //Material1 = pStep->GetPreStepPoint()->GetMaterial();
     //Material2 = pStep->GetPostStepPoint()->GetMaterial();
  } 
  else 
  {
     theStatus = NotAtBoundary;
     return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  if (aTrack.GetStepLength()<=kCarTolerance/2)
  {
	  theStatus = StepTooSmall;
	  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
	}

  // do nothing
  aParticleChange.Initialize(aTrack);
  return &aParticleChange;
}

G4double OpDetectionProcess::GetMeanFreePath(const G4Track&,G4double,G4ForceCondition*)
{
  return 0.;
}

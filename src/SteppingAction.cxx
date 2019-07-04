/**
 * @file SteppingAction.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Stepping action.
 * @date 07-04-2019
 * 
 */

#include "SteppingAction.h"
#include "Configuration.h"
#include "OpDetPhotonTable.h"

#include "G4OpticalPhoton.hh"

namespace majorana 
{

SteppingAction::SteppingAction()
{
  time_t seed = time( NULL );
  fRandomEngine.setSeed(static_cast<long>(seed+1));
}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track*           theTrack     = theStep->GetTrack();
  G4StepPoint*       thePrePoint  = theStep->GetPreStepPoint();
  G4StepPoint*       thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV    = thePostPoint->GetPhysicalVolume();
  G4VPhysicalVolume* thePrePV     = thePrePoint->GetPhysicalVolume();
  auto               thePosition  = thePrePoint->GetPosition();

  if (!thePostPV) return;
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
  if(particleType!=G4OpticalPhoton::OpticalPhotonDefinition()) return;

  //*************
  // We will handle surface absorption here
  // Make sure we're at the acrylic/air boundary
  G4String preName  = thePrePV->GetName();
  G4String postName = thePostPV->GetName();
  if (preName == "volDisk" && postName == "volWorld")
  {
    Configuration* config = Configuration::Instance();
    float surfaceAbs = config->SurfaceAbsorption();
    CLHEP::RandFlat   flat(fRandomEngine);
    float u = flat.fire();
    if (u < surfaceAbs) 
    {
      theTrack->SetTrackStatus(fStopAndKill);
      OpDetPhotonTable* photonTable = OpDetPhotonTable::Instance();
      photonTable->IncPhotonsAbs();
    }
  }
}
}

#include "MPPCSD.h"
#include "OpDetPhotonTable.h"
#include "G4Helper.h"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"

namespace majorana
{

MPPCSD::MPPCSD(G4String name)
  : G4VSensitiveDetector(name)
{}

MPPCSD::~MPPCSD() 
{}

void MPPCSD::Initialize(G4HCofThisEvent* hitsCE)
{}

G4bool MPPCSD::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
  G4Track* theTrack = aStep->GetTrack();

  // If this isn't an optical photon, exit
  if(theTrack->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;
 
  // Get mppc ID
  G4int mppcNumber = aStep->GetPostStepPoint()->GetTouchable()->GetReplicaNumber();

  // We want to store the information for this event
  G4Helper* g4Helper = G4Helper::Instance();
  PrimaryGeneratorAction* genAction = g4Helper->GetActionInitialization()->GetGeneratorAction();
  const std::vector<float> photonVertex = { genAction->GetSourcePositionRTZ()[0], 
                                            genAction->GetSourcePositionRTZ()[1],
                                            genAction->GetSourcePositionRTZ()[2] };

  // Create the photon
  Photon thePhoton(photonVertex);
  
  // Add to table 
  OpDetPhotonTable* thePhotonTable = OpDetPhotonTable::Instance();
  thePhotonTable->AddPhoton(mppcNumber, thePhoton);
 
  theTrack->SetTrackStatus(fStopAndKill);  
  return true;
}

void MPPCSD::EndOfEvent(G4HCofThisEvent* ) {}

void MPPCSD::clear() {}

void MPPCSD::DrawAll() {}

void MPPCSD::PrintAll() {}

}

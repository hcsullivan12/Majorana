// 
// File: PrimaryGeneratorAction.h
//
// Author: Hunter Sullivan
//
// Discription: 
//

#ifndef PRIMARYGENERATORACTION_H
#define PRIMARYGENERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"

#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

class G4Event;

namespace majorana {

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);         

    void Reset(const G4double& r,
               const G4double& thetaDeg,
               const G4double& x,
               const G4double& y,
               const G4double& z,
               const G4int&   n,
               const G4double& voxelSize = 0);
    void Append();
    std::vector<G4double> GetSourcePositionRTZ() const { return fSourcePositionRTZ; };
    std::vector<G4double> GetSourcePositionXYZ() const { return fSourcePositionXYZ; };
    unsigned           GetNPrimaries()           const { return fNPrimaries; };

  private:
    G4ParticleTable*      fParticleTable;
    unsigned              fNPrimaries;
    std::vector<G4double> fSourcePositionRTZ;
    std::vector<G4double> fSourcePositionXYZ;
    G4double              fSourcePosSigma;
    G4double              fSourcePeakE;
    G4double              fSourcePeakESigma;
    G4double              fVoxelSize;
    std::string           fSourceMode;
    CLHEP::HepJamesRandom fRandomEngine;
};

}
#endif

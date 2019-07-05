/**
 * @file PrimaryGeneratorAction.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Photon emission generator.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_PRIMARYGENERATORACTION_H
#define MAJSIM_PRIMARYGENERATORACTION_H

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

namespace majsim {

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
               const G4int&    n,
               G4double pixelSize = 5*CLHEP::mm);

    std::vector<G4double> GetSourcePositionRTZ() const { return fSourcePositionRTZ; };
    std::vector<G4double> GetSourcePositionXYZ() const { return fSourcePositionXYZ; };
    unsigned           GetNPrimaries()           const { return fNPrimaries; };

  private:
    G4ParticleTable*      fParticleTable;
    unsigned              fNPrimaries;
    std::vector<G4double> fSourcePositionRTZ; ///< Source position for this event in polar 
    std::vector<G4double> fSourcePositionXYZ; ///< Source position for this event in cartesian
    G4double              fSourcePosSigma;    ///< Sigma of point source distribution
    G4double              fSourcePeakE;       
    G4double              fSourcePeakESigma;  
    G4double              fBinSize;           ///< Bin size for histogram 
    std::string           fSourceMode;        ///< 
    CLHEP::HepJamesRandom fRandomEngine;      ///< 
};

}
#endif

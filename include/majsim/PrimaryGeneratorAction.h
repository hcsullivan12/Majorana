//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 




#ifndef MAJSIM_PRIMARYGENERATORACTION_H
#define MAJSIM_PRIMARYGENERATORACTION_H
#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4PrimaryVertex.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "majsim/Configuration.h"
#include "G4UImanager.hh"


#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"


#include "TH2.h"
class G4Event;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
namespace majsim {
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
   ~PrimaryGeneratorAction();
    //virtual void GeneratePrimaries(G4Event*);

    void Reset(const G4double& r,
               const G4double& thetaDeg,
               const G4double& x,
               const G4double& y,
               const G4double& z,
               const G4int&    n,
               G4double pixelSize = 5*CLHEP::mm);

    std::vector<G4double> GetSourcePositionRTZ() const { return fSourcePositionRTZ; };
    TH2I*                 GetPrimHist()                { return fPrimHist; };
    std::vector<G4double> GetSourcePositionXYZ() const { return fSourcePositionXYZ; };
    unsigned           GetNPrimaries()           const { return fNPrimaries; };




  public:
    virtual void GeneratePrimaries(G4Event*);
    G4ParticleGun* GetParticleGun() { return fParticleGun;} ;
    void               PrintSourceInfo();
            
  private:
    G4ParticleGun*  fParticleGun;
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
    TH2I*                 fPrimHist;
    Configuration*            fConfig;
    G4int                  fEventCount=0;
    G4UImanager*            fUIManager;
    G4bool                  fOldStyle=false;
    G4PrimaryParticle *     g4Particle;


};


}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif

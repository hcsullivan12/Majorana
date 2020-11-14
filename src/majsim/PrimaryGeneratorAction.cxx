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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "majsim/PrimaryGeneratorAction.h"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Geantino.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "TVector3.h"
#include "TH2.h"
#include "TFile.h"
#include "majsim/Configuration.h"


namespace majsim {
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0),fPrimHist(nullptr)
{
    fConfig = Configuration::Instance();
    fSourcePosSigma   = fConfig->SourcePosSigma();
    fSourcePeakE      = fConfig->SourcePeakE();
    fSourcePeakESigma = fConfig->SourcePeakESigma();
    fSourceMode       = fConfig->SourceMode();

    // Initialize rnd generator
    time_t seed = time( NULL );
    fRandomEngine.setSeed(static_cast<long>(seed));
    fNPrimaries = 1;

    if (!fOldStyle) {
        G4int n_particle = fNPrimaries;
        fParticleGun = new G4ParticleGun(n_particle);
        //fParticleGun->SetParticleEnergy(0 * eV);
    }else
        fParticleTable=0;







}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//------------------------------------------------------------------------
void PrimaryGeneratorAction::Reset(const G4double& r,
                                   const G4double& thetaDeg,
                                   const G4double& x,
                                   const G4double& y,
                                   const G4double& z,
                                   const G4int&    n,
                                   G4double binSize)
{
    fSourcePositionRTZ.clear();
    fSourcePositionXYZ.clear();

    fSourcePositionRTZ.push_back(r);
    fSourcePositionRTZ.push_back(thetaDeg);
    fSourcePositionRTZ.push_back(z);
    fSourcePositionXYZ.push_back(x);
    fSourcePositionXYZ.push_back(y);
    fSourcePositionXYZ.push_back(z);
    fUIManager=G4UImanager::GetUIpointer();

    fNPrimaries = n;
    fBinSize    = binSize;

    // Initialize true distribution
    double diskRadius = Configuration::Instance()->DiskRadius()/CLHEP::cm;
    size_t nBins = 2*std::floor(diskRadius/(binSize/CLHEP::cm)) + 1; // this assumes there is a pixel at the origin

    //if (!fPrimHist) fPrimHist = new TH2I("primHist", "primHist", nBins, -diskRadius, diskRadius, nBins, -diskRadius, diskRadius);
}
//------------------------------------------------------------------------
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    std::cout << "Source fConfiguration: "
              << "  nPrimaries = "        << fNPrimaries
              << "  r = "                 << fSourcePositionRTZ[0]/CLHEP::cm
              << "  theta = "             << fSourcePositionRTZ[1]/CLHEP::deg
              << "  x = "                 << fSourcePositionXYZ[0]/CLHEP::cm
              << "  y = "                 << fSourcePositionXYZ[1]/CLHEP::cm
              << "\n";

    CLHEP::RandGaussQ gauss(fRandomEngine);
    CLHEP::RandFlat   flat(fRandomEngine);

    if (fParticleGun->GetParticleDefinition() == G4Geantino::Geantino()) {


        std::cout<<"No Source is assined.. "<<std::endl;
        std::cout<<"Assigning the source... "<<std::endl;
        std::cout <<"--Setting the Source--"<<std::endl;
        G4bool IsCaseGeantino =false;
        switch(fConfig->GetSourceType()){
            case 0:
                std::cout<<"Setting the Particle to Cf252"<<std::endl;
                fUIManager->ApplyCommand("/control/execute Cf252.mac");//
                fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,0));


                break;
            case 1:
                std::cout<<"Setting the Particle to a opticalPhoton"<<std::endl;
                fUIManager->ApplyCommand("/control/execute opticalphoton.mac");
                fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));

                break;
            case 2:
                std::cout<<"Setting the Particle to a Alpha Particle"<<std::endl;
                fUIManager->ApplyCommand("/control/execute alpha.mac");
                fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));
                break;
            case 3:
                IsCaseGeantino=true;
                break;
            default:
                std::cout<<"This is the old Style"<<std::endl;
                fOldStyle=true;
        }
        if(IsCaseGeantino){
            G4int Z = 10, A = 24;
            G4double ionCharge   = 0.*eplus;
            G4double excitEnergy = 0.*keV;
            G4ParticleDefinition* ion= G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
            fParticleGun->SetParticleDefinition(ion);
            fParticleGun->SetParticleCharge(ionCharge);
        }
    }

    auto diskRadius = Configuration::Instance()->DiskRadius()/CLHEP::cm;
    /*for (unsigned xbin = 1; xbin <= fPrimHist->GetXaxis()->GetNbins(); xbin++)
    {
      for (unsigned ybin = 1; ybin <= fPrimHist->GetYaxis()->GetNbins(); ybin++)
      {
        float xV = fPrimHist->GetXaxis()->GetBinCenter(xbin);
        float yV = fPrimHist->GetYaxis()->GetBinCenter(ybin);
        if ((xV*xV + yV*yV) > diskRadius*diskRadius) continue;
        fPrimHist->SetBinContent(xbin, ybin, 1); // this gives a nicer plot
      }
    }*/

    if(!fOldStyle){

        G4double SourcePosZ=-56.5;
        fSourcePositionXYZ[2]=SourcePosZ;
        auto PosX=fSourcePositionXYZ[0];
        auto PosY=fSourcePositionXYZ[1];
        auto PosZ=fSourcePositionXYZ[2];

        fParticleGun->SetParticlePosition(G4ThreeVector(PosX,PosY,PosZ));
    }



    // Loop over the primaries
    if(fOldStyle) {
        for (unsigned primary = 0; primary < fNPrimaries; primary++) {
            // Smear position of this photon
            // If source mode is point, gaussian smear
            // If source mode is pixel, uniform smear
            // Initial z will be slightly below top
            G4double x(0), y(0);
            if (fSourceMode == "point") {
                x = gauss.fire(fSourcePositionXYZ[0], fSourcePosSigma);
                y = gauss.fire(fSourcePositionXYZ[1], fSourcePosSigma);

                float xTemp = x / 10;
                float yTemp = y / 10;
                //if((xTemp*xTemp+yTemp*yTemp) < diskRadius*diskRadius) fPrimHist->Fill(xTemp, yTemp);
            } else {
                G4double a1 = fSourcePositionXYZ[0] - fBinSize / 2;
                G4double b1 = fSourcePositionXYZ[0] + fBinSize / 2;
                G4double a2 = fSourcePositionXYZ[1] - fBinSize / 2;
                G4double b2 = fSourcePositionXYZ[1] + fBinSize / 2;

                // smear
                //x = flat.fire(a1, b1);
                //y = flat.fire(a2, b2);
                x = fSourcePositionXYZ[0];
                y = fSourcePositionXYZ[1];
            }
            // Just under the top surface
            G4double z = fSourcePositionXYZ[2] - 0.1 * CLHEP::cm;
            // Sample the momentum
            G4double p = gauss.fire(fSourcePeakE, fSourcePeakESigma);
            // Keep generating until pZ < 0
            G4double pX(0), pY(0), pZ(1);
            while (pZ >= 0) {
                double cosTheta = 2 * flat.fire() - 1;
                double sinTheta = pow(1 - pow(cosTheta, 2), 0.5);
                double phi = 2 * M_PI * flat.fire();

                pX = p * sinTheta * cos(phi);
                pY = p * sinTheta * sin(phi);
                pZ = p * cosTheta;
            }

            //*******************
            // TEMPORARY
            //pX = p;
            //pY = 0;
            //pZ = 0;
            //x  = 0;
            //y  = -0.01*CLHEP::cm;
            //*******************

            // For the polarization vector, sample a new random direction
            // and cross it with the momentum direction
            G4double u1 = flat.fire();
            G4double u2 = flat.fire();
            G4double u3 = flat.fire();
            TVector3 momentum(pX, pY, pZ);
            TVector3 temp(u1, u2, u3);
            TVector3 polarization = momentum.Cross(temp);
            G4double mag = polarization * polarization;
            polarization = polarization * std::sqrt(1 / mag);

            // Add this vertex
            G4PrimaryVertex *vertex = new G4PrimaryVertex(x, y, z, 0);
            event->AddPrimaryVertex(vertex);

            if (fParticleTable == 0) fParticleTable = G4ParticleTable::GetParticleTable();
            G4ParticleDefinition *particleDefinition = fParticleTable->FindParticle("opticalphoton");
            g4Particle = new G4PrimaryParticle(particleDefinition,
                                                                  momentum[0],
                                                                  momentum[1],
                                                                  momentum[2]);
            g4Particle->SetCharge(0);
            g4Particle->SetPolarization(polarization[0],
                                        polarization[1],
                                        polarization[2]);

            vertex->SetPrimary(g4Particle);

        }
        PrintSourceInfo();
    }



    if(!fOldStyle){
        fParticleGun->SetNumberOfParticles(fNPrimaries);
        PrintSourceInfo();
        fParticleGun->GeneratePrimaryVertex(event);
    }



    //create vertex




}
    void PrimaryGeneratorAction::PrintSourceInfo()
    {
        if(!fOldStyle){
            std::cout<<"-----------Particle Gun Information--------" << std::endl;
            std::cout<<"Source Name: " << fParticleGun->GetParticleDefinition()->GetParticleName()<<std::endl;
            std::cout<<"AtomicNumber: " << fParticleGun->GetParticleDefinition()->GetAtomicNumber() <<std::endl;
            std::cout<<"Type: " << fParticleGun->GetParticleDefinition()->GetParticleType() <<std::endl;
            std::cout<<"NumberofParticle: "<< fParticleGun->GetNumberOfParticles()<<std::endl;
            std::cout<<"Position: "<< fParticleGun->GetParticlePosition()<<std::endl;
            std::cout<<"Momenutum: "<< fParticleGun->GetParticleMomentumDirection()<<std::endl;
            std::cout<<"Energy(eV): "<< fParticleGun->GetParticleEnergy()<<std::endl;
            }else{

            std::cout<<"-----------Particle Gun Information--------" << std::endl;
            std::cout<<"Source Name: " << g4Particle->GetParticleDefinition()->GetParticleName()<<std::endl;
            std::cout<<"AtomicNumber: " << g4Particle->GetParticleDefinition()->GetAtomicNumber() <<std::endl;
            std::cout<<"Type: " << g4Particle->GetParticleDefinition()->GetParticleType() <<std::endl;
            std::cout<<"NumberofParticle: "<< fNPrimaries<<std::endl;
            std::cout<<"Polarization: "<< g4Particle->GetPolarization()<<std::endl;
            std::cout<<"MomenutumDirection: "<< g4Particle->GetMomentumDirection()<<std::endl;
            std::cout<<"TotalEnergy(eV): "<< g4Particle->GetTotalEnergy()<<std::endl;
        }
    }

}
// 
// File: PrimaryGeneratorAction.cxx
//
// Author: Hunter Sullivan
//
// Discription: 
//

#include "PrimaryGeneratorAction.h"
#include "Configuration.h"

#include "TVector3.h"
#include "TGraph.h"
#include "TFile.h"

namespace majorana {

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction()
{
  fParticleTable = 0;

  Configuration* config = Configuration::Instance();
  fSourcePosSigma   = config->SourcePosSigma();
  fSourcePeakE      = config->SourcePeakE();
  fSourcePeakESigma = config->SourcePeakESigma();
  fSourceMode       = config->SourceMode();

  // Initialize rnd generator
  time_t seed = time( NULL );
  fRandomEngine.setSeed(static_cast<long>(seed)); 
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{}

void PrimaryGeneratorAction::Reset(const G4double& r,
                                   const G4double& thetaDeg,
                                   const G4double& x,
                                   const G4double& y,
                                   const G4double& z,
                                   const G4int&    n,
                                   const G4double& voxelSize)
{
  fSourcePositionRTZ.clear();
  fSourcePositionXYZ.clear();

  fSourcePositionRTZ.push_back(r);
  fSourcePositionRTZ.push_back(thetaDeg);
  fSourcePositionRTZ.push_back(z);
  fSourcePositionXYZ.push_back(x);
  fSourcePositionXYZ.push_back(y);
  fSourcePositionXYZ.push_back(z);
  
  fNPrimaries = n;
  fVoxelSize  = voxelSize; 
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  G4cout << "Source configuration: "
         << "  nPrimaries = "        << fNPrimaries
         << "  r = "                 << fSourcePositionRTZ[0]/cm 
         << "  theta = "             << fSourcePositionRTZ[1]/deg 
         << "  x = "                 << fSourcePositionXYZ[0]/cm 
         << "  y = "                 << fSourcePositionXYZ[1]/cm
         << G4endl;
  
  CLHEP::RandGaussQ gauss(fRandomEngine);
  CLHEP::RandFlat   flat(fRandomEngine);

  // Loop over the primaries  
  for (unsigned primary = 0; primary < fNPrimaries; primary++)
  {
    // Smear position of this photon
    // If source mode is point, gaussian smear
    // If source mode is voxel, uniform smear
    // Initial z will be slightly below top
    G4double x(0), y(0);
    if (fSourceMode == "point")
    {
      x = gauss.fire(fSourcePositionXYZ[0], fSourcePosSigma);
      y = gauss.fire(fSourcePositionXYZ[1], fSourcePosSigma);
    }
    else
    {
      G4double a1 = fSourcePositionXYZ[0] - fVoxelSize/2;
      G4double b1 = fSourcePositionXYZ[0] + fVoxelSize/2;
      G4double a2 = fSourcePositionXYZ[1] - fVoxelSize/2;
      G4double b2 = fSourcePositionXYZ[1] + fVoxelSize/2;

      // smear
      //x = flat.fire(a1, b1);
      //y = flat.fire(a2, b2);
      x = fSourcePositionXYZ[0];
      y = fSourcePositionXYZ[1];
    }
    G4double z = fSourcePositionXYZ[2]*0.5;// - 0.1;
    // Sample the momentum
    G4double p = gauss.fire(fSourcePeakE, fSourcePeakESigma);
    // Keep generating until pZ < 0
    G4double pX(0), pY(0), pZ(1);
    while (pZ >= 0)
    {
      double cosTheta = 2*flat.fire() - 1;
      double sinTheta = pow(1-pow(cosTheta,2),0.5);
      double phi      = 2*M_PI*flat.fire();

      pX = p*sinTheta*cos(phi);
      pY = p*sinTheta*sin(phi);
      pZ = p*cosTheta;
    }
   
    //*******************
    // TEMPORARY
    //pX = p;
    //pY = 0;
    //pZ = 0;
    //x  = 0;
    //y  = -0.01*cm;
    //*******************
    
    // For the polarization vector, sample a new random direction
    // and cross it with the momentum direction
    G4double u1 = flat.fire();
    G4double u2 = flat.fire();
    G4double u3 = flat.fire();
    TVector3 momentum(pX, pY, pZ);
    TVector3 temp(u1, u2, u3);
    TVector3 polarization = momentum.Cross(temp);
    G4double mag = polarization*polarization;
    polarization = polarization*std::sqrt(1/mag);

    // Add this vertex
    G4PrimaryVertex* vertex = new G4PrimaryVertex(x, y, z, 0);
    event->AddPrimaryVertex(vertex);
    //g.SetPoint(primary, x, y);

    if (fParticleTable == 0)
    {
      fParticleTable = G4ParticleTable::GetParticleTable();
    }

    G4ParticleDefinition* particleDefinition = fParticleTable->FindParticle("opticalphoton");
    G4PrimaryParticle* g4Particle = new G4PrimaryParticle(particleDefinition,
                                                          momentum[0],
                                                          momentum[1],
                                                          momentum[2]);
    g4Particle->SetCharge(0);
    g4Particle->SetPolarization(polarization[0],
                                polarization[1],
                                polarization[2]);
    vertex->SetPrimary(g4Particle);
  }
}
}

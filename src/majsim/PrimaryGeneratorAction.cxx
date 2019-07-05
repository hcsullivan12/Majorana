/**
 * @file PrimaryGeneratorAction.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Photon emission generator.
 * @date 07-04-2019
 * 
 */

#include "majsim/PrimaryGeneratorAction.h"
#include "majsim/Configuration.h"
#include "majsim/PixelTable.h"

#include "TVector3.h"
#include "TH2.h"
#include "TFile.h"

namespace majsim {

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
                                   const G4double& pixelSize)
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
  fPixelSize  = pixelSize; 
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

  // Initialize true distribution
  auto config = Configuration::Instance();
  auto pixelTable = PixelTable::Instance();
  double diskRadius = config->DiskRadius()/CLHEP::cm;
  double pixelSpacing = pixelTable->GetSpacing();
  size_t nBins = 2*std::floor(diskRadius/pixelSpacing) + 1; // this assumes there is a pixel at the origin

  TH2I primHist("primHist", "primHist", nBins, -diskRadius, diskRadius, nBins, -diskRadius, diskRadius);
  for (unsigned xbin = 1; xbin <= primHist.GetXaxis()->GetNbins(); xbin++)
  {
    for (unsigned ybin = 1; ybin <= primHist.GetYaxis()->GetNbins(); ybin++) 
    {
      float xV = primHist.GetXaxis()->GetBinCenter(xbin);
      float yV = primHist.GetYaxis()->GetBinCenter(ybin);
      if ((xV*xV + yV*yV) > diskRadius*diskRadius) continue;
      primHist.SetBinContent(xbin, ybin, 1); // this gives a nicer plot
    }
  }

  // Loop over the primaries  
  for (unsigned primary = 0; primary < fNPrimaries; primary++)
  {
    // Smear position of this photon
    // If source mode is point, gaussian smear
    // If source mode is pixel, uniform smear
    // Initial z will be slightly below top
    G4double x(0), y(0);
    if (fSourceMode == "point")
    {
      x = gauss.fire(fSourcePositionXYZ[0], fSourcePosSigma);
      y = gauss.fire(fSourcePositionXYZ[1], fSourcePosSigma);

      float xTemp = x/10;
      float yTemp = y/10;
      if((xTemp*xTemp+yTemp*yTemp) < diskRadius*diskRadius) primHist.Fill(xTemp, yTemp);
    }
    else
    {
      G4double a1 = fSourcePositionXYZ[0] - fPixelSize/2;
      G4double b1 = fSourcePositionXYZ[0] + fPixelSize/2;
      G4double a2 = fSourcePositionXYZ[1] - fPixelSize/2;
      G4double b2 = fSourcePositionXYZ[1] + fPixelSize/2;

      // smear
      //x = flat.fire(a1, b1);
      //y = flat.fire(a2, b2);
      x = fSourcePositionXYZ[0];
      y = fSourcePositionXYZ[1];
    }
    // Just under the top surface
    G4double z = fSourcePositionXYZ[2] - 0.1*cm;
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

    if (fParticleTable == 0) fParticleTable = G4ParticleTable::GetParticleTable();

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
  if (config->EvdMode())
  {
    std::string outfile = "../evd/trueDist.root";
    TFile f(outfile.c_str(), "RECREATE");
    primHist.Write();
    f.Close();
  }
}
}

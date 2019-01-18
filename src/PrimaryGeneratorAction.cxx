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
  m_particleTable = 0;

  Configuration* config = Configuration::Instance();
  m_sourcePosSigma   = config->SourcePosSigma();
  m_sourcePeakE      = config->SourcePeakE();
  m_sourcePeakESigma = config->SourcePeakESigma();
  m_sourceMode       = config->SourceMode();

  // Initialize rnd generator
  time_t seed = time( NULL );
  m_randomEngine.setSeed(static_cast<long>(seed)); 
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{}

void PrimaryGeneratorAction::Reset(const G4float& r,
                                   const G4float& thetaDeg,
                                   const G4float& x,
                                   const G4float& y,
                                   const G4float& z,
                                   const G4int&   n,
                                   const G4float& voxelSize)
{
  m_sourcePositionRTZ.clear();
  m_sourcePositionXYZ.clear();

  m_sourcePositionRTZ.push_back(r);
  m_sourcePositionRTZ.push_back(thetaDeg);
  m_sourcePositionRTZ.push_back(z);
  m_sourcePositionXYZ.push_back(x);
  m_sourcePositionXYZ.push_back(y);
  m_sourcePositionXYZ.push_back(z);
  
  m_nPrimaries = n;
  m_voxelSize  = voxelSize; 
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  G4cout << "Source configuration: "
         << "  nPrimaries = "        << m_nPrimaries
         << "  r = "                 << m_sourcePositionRTZ[0] 
         << "  theta = "             << m_sourcePositionRTZ[1] 
         << "  x = "                 << m_sourcePositionXYZ[0] 
         << "  y = "                 << m_sourcePositionXYZ[1]
         << std::endl;
  
  CLHEP::RandGaussQ gauss(m_randomEngine);
  CLHEP::RandFlat   flat(m_randomEngine);

  // Source scatter plot
  //TGraph g(m_nPrimaries);

  // Loop over the primaries  
  for (unsigned primary = 0; primary < m_nPrimaries; primary++)
  {
    // Smear position of this photon
    // If source mode is point, gaussian smear
    // If source mode is voxel, uniform smear
    // Initial z will be slightly below top
    G4float x(0), y(0);
    if (m_sourceMode == "point")
    {
      x = gauss.fire(m_sourcePositionXYZ[0], m_sourcePosSigma);
      y = gauss.fire(m_sourcePositionXYZ[1], m_sourcePosSigma);
    }
    else
    {
      float a1 = m_sourcePositionXYZ[0] - m_voxelSize/2;
      float b1 = m_sourcePositionXYZ[0] + m_voxelSize/2;
      float a2 = m_sourcePositionXYZ[1] - m_voxelSize/2;
      float b2 = m_sourcePositionXYZ[1] + m_voxelSize/2;

      // smear
      //x = flat.fire(a1, b1);
      //y = flat.fire(a2, b2);
      x = m_sourcePositionXYZ[0];
      y = m_sourcePositionXYZ[1];
    }
    G4float z = m_sourcePositionXYZ[2] - 0.1;
    // Sample the momentum
    float p = gauss.fire(m_sourcePeakE, m_sourcePeakESigma);
    // Keep generating until pZ < 0
    float pX(0), pY(0), pZ(1);
    while (pZ >= 0)
    {
      float cosTheta = 2*flat.fire() - 1;
      float sinTheta = pow(1-pow(cosTheta,2),0.5);
      float phi      = 2*M_PI*flat.fire();

      pX = p*sinTheta*cos(phi);
      pY = p*sinTheta*sin(phi);
      pZ = p*cosTheta;
    }
   
    // For the polarization vector, sample a new random direction
    // and cross it with the momentum direction
    float u1 = flat.fire();
    float u2 = flat.fire();
    float u3 = flat.fire();
    TVector3 momentum(pX, pY, pZ);
    TVector3 temp(u1, u2, u3);
    TVector3 polarization = momentum.Cross(temp);
    float mag = polarization*polarization;
    polarization = polarization*std::sqrt(1/mag);

    // Add this vertex
    G4PrimaryVertex* vertex = new G4PrimaryVertex(x*cm, y*cm, z*cm, 0);
    event->AddPrimaryVertex(vertex);
    //g.SetPoint(primary, x, y);

    if (m_particleTable == 0)
    {
      m_particleTable = G4ParticleTable::GetParticleTable();
    }

    G4ParticleDefinition* particleDefinition = m_particleTable->FindParticle("opticalphoton");
    G4PrimaryParticle* g4Particle = new G4PrimaryParticle(particleDefinition,
                                                          momentum[0]*eV,
                                                          momentum[1]*eV,
                                                          momentum[2]*eV);
    g4Particle->SetCharge(0);
    g4Particle->SetPolarization(polarization[0],
                                polarization[1],
                                polarization[2]);
    vertex->SetPrimary(g4Particle);
  }

  
  //TFile f("/home/hunter/projects/Majorana/output/simulateOutput.root", "UPDATE");
  //g.Write();
  //f.Close();
}
}

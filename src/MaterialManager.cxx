/**
 * @file MaterialManager.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton for handling geometry materials.
 * @date 07-04-2019
 * 
 */

#include "MaterialManager.h"

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

#include <assert.h>

namespace majorana
{

MaterialManager* MaterialManager::instance = 0;

MaterialManager* MaterialManager::CreateInstance()
{
  if (instance == 0) 
  {
    static MaterialManager manager;
    instance = &manager;
  }
  return instance;
}

MaterialManager* MaterialManager::Instance()
{
  assert(instance);
  return instance;
}

MaterialManager::MaterialManager()
{}

MaterialManager::~MaterialManager()
{}

//**************************
// Construct materials
//
void MaterialManager::ConstructMaterials()
{
  // Define the tpb emission spectrum
  fTPBEmissionE.resize(15, 0);
  fTPBEmissionE = {  0.05*eV,    1.0*eV,   1.5*eV,  2.25*eV, 2.481*eV, 
                    2.819*eV,  2.952*eV, 2.988*eV, 3.024*eV,   3.1*eV, 
                     3.14*eV, 3.1807*eV,  3.54*eV,   5.5*eV, 50.39*eV};

  fTPBEmissionSpect.resize(15, 0);
  fTPBEmissionSpect = {    0.0*eV,   0.0*eV, 0.0*eV, 0.0588*eV, 0.235*eV, 
                         0.853*eV,   1.0*eV, 1.0*eV, 0.9259*eV, 0.704*eV, 
                        0.0296*eV, 0.011*eV, 0.0*eV,    0.0*eV,   0.0*eV};

  // Seperate the implentation here
  DefineAir();
  DefineMPPCMaterial();
  DefineAcrylic();
}

//**************************
// Define MPPC material
//
void MaterialManager::DefineMPPCMaterial()
{
  G4NistManager* nistMan  = G4NistManager::Instance();
  G4Material*    al       = nistMan->FindOrBuildMaterial("G4_Al");

  // Optical properties 
  // For now, just assume 0% reflectivity (all absorbed) 
  // and 100 % effeciency
  const G4int nEntries = fTPBEmissionE.size();
  G4double photonEnergy[nEntries];
  G4double alRef[nEntries];
  G4double alEff[nEntries];
  for (int i = 0; i < nEntries; i++) 
  {
    photonEnergy[i] = fTPBEmissionE[i];
    alRef[i]        = 0.0;
    alEff[i]        = 1.0;
  }

  G4MaterialPropertiesTable *siMPT = new G4MaterialPropertiesTable();
  siMPT->AddProperty("REFLECTIVITY",      photonEnergy, alRef,       nEntries);
  siMPT->AddProperty("EFFICIENCY",        photonEnergy, alEff,       nEntries);

  al->SetMaterialPropertiesTable(siMPT);
}

//**************************
// Define air
//
void MaterialManager::DefineAir()
{ 
  G4NistManager* nistMan = G4NistManager::Instance();
  G4Material*      air   = nistMan->FindOrBuildMaterial("G4_AIR");
 
  // Optical properties 
  const G4int nEntries = fTPBEmissionE.size();
  G4double photonEnergy[nEntries];
  G4double airRIndex[nEntries];
  G4double airSpect[nEntries];
  G4double airAbs[nEntries];
  G4double airRIndexImg[nEntries];
  G4double airRefl[nEntries];
  for (int i = 0; i < nEntries; i++) 
  {
    photonEnergy[i] = fTPBEmissionE[i];
    airRIndex[i]    = 1.0;
    airRIndexImg[i] = 0.0;
    airRefl[i]       = 0.97;
  }

  G4MaterialPropertiesTable *airMPT = new G4MaterialPropertiesTable();
  airMPT->AddProperty("RINDEX", photonEnergy, airRIndex, nEntries);
  airMPT->AddProperty("REALRINDEX", photonEnergy, airRIndex, nEntries);
  airMPT->AddProperty("IMAGINARYRINDEX", photonEnergy, airRIndexImg, nEntries);
  // For border surface
  // Assuming 97% reflectivity
  //airMPT->AddProperty("REFLECTIVITY", photonEnergy, airRefl, nEntries);
  air->SetMaterialPropertiesTable(airMPT);
}

//**************************
// Define acrylic
//
void MaterialManager::DefineAcrylic()
{
  //Define elements 
  G4int nel;
  G4double a, density, z;
  G4String name, symbol;
  a = 1.01*g/mole;
  G4Element* elH  = new G4Element(name="Hydrogen", symbol="H", z=1., a); 
  a = 12.01*g/mole;
  G4Element* elC  = new G4Element(name="Carbon",   symbol="C", z=6., a);
  a = 14.01*g/mole;
  G4Element* elN  = new G4Element(name="Nitrogen", symbol="N", z=7., a);
  a = 16.00*g/mole;
  G4Element* elO  = new G4Element(name="Oxygen",   symbol="O", z=8., a);

  density = 1.19*g/cm3;
  G4Material* acrylic = new G4Material(name="Acrylic", density, nel=3);
  acrylic->AddElement(elC, 5);
  acrylic->AddElement(elH, 8);
  acrylic->AddElement(elO, 2);

  // Optical properties 
  // TPB Emission: LArSoft package (lardataalg)
  // Refractive index: arXiv:1101.3013v1
  // Absorption length --> infinity: ref therein arXiv:1307.6906v2
  const G4int nEntries = fTPBEmissionE.size();
  G4double photonEnergy[nEntries];
  G4double acrylicRIndex[nEntries];
  G4double acrylicRIndexImg[nEntries];
  G4double acrylicSpect[nEntries];
  G4double acrylicAbs[nEntries];
  for (int i = 0; i < nEntries; i++) 
  {
    photonEnergy[i]     = fTPBEmissionE[i];
    acrylicRIndex[i]    = 1.49;
    acrylicRIndexImg[i] = 0.0;
    acrylicSpect[i]     = fTPBEmissionSpect[i]; 
    acrylicAbs[i]       = 100*m;
  }

  G4MaterialPropertiesTable *acrylicMPT = new G4MaterialPropertiesTable();
  acrylicMPT->AddProperty("RINDEX",          photonEnergy, acrylicRIndex,    nEntries);
  acrylicMPT->AddProperty("REALRINDEX",      photonEnergy, acrylicRIndex,    nEntries);
  acrylicMPT->AddProperty("IMAGINARYRINDEX", photonEnergy, acrylicRIndexImg, nEntries);
  acrylicMPT->AddProperty("ABSLENGTH",       photonEnergy, acrylicAbs,       nEntries);
  acrylicMPT->AddProperty("EMISSIONSPECT",   photonEnergy, acrylicSpect,     nEntries);

  acrylic->SetMaterialPropertiesTable(acrylicMPT);
}

//**************************
// Find material
//
G4Material* MaterialManager::FindMaterial(const G4String& name)
{
  G4Material* material = G4Material::GetMaterial(name, true);
  return material;
}

}

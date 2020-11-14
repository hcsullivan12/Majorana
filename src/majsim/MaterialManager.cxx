/**
 * @file MaterialManager.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton for handling geometry materials.
 * @date 07-04-2019
 * 
 */
 #include <cmath>
#include "majsim/MaterialManager.h"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include <assert.h>
#include "globals.hh"

namespace majsim
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
  fTPBEmissionSpect = {    0.0,   0.0, 0.0, 0.0588, 0.235,
                         0.853,   1.0, 1.0, 0.9259, 0.704,
                        0.0296, 0.011, 0.0,    0.0,   0.0};

  //Absorption
  fTPBAbsE.resize(10,0);
  fTPBAbsE={0.05*eV,1.77*eV,2.0675*eV, 7.42*eV, 7.75*eV, 8.16*eV, 8.73*eV, 9.78*eV,10.69*eV, 50.39*eV};
  fTPBAbsSpect.resize(10,0);
  fTPBAbsSpect={100000.0,100000.0, 100000.0,0.001,0.00000000001,0.00000000001, 0.00000000001, 0.00000000001, 0.00000000001, 0.00000000001};


  // Seperate the implentation here
  config = Configuration::CreateInstance();


  DefineAir();
  DefineArgon();
  DefineAcrylic();
  DefineMPPCMaterial();
  DefineTPB();
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
  G4double abs[nEntries];

    for (int i = 0; i < nEntries; i++)
  {
    photonEnergy[i] = fTPBEmissionE[i];
    alRef[i]        = 0.0;
    alEff[i]        = 0.5;
    abs[i]          =0;
  }

  G4MaterialPropertiesTable *siMPT = new G4MaterialPropertiesTable();
  siMPT->AddProperty("REFLECTIVITY",      photonEnergy, alRef,       nEntries);
  siMPT->AddProperty("EFFICIENCY",        photonEnergy, alEff,       nEntries);
  siMPT->AddProperty("ABSLENGTH",photonEnergy,abs,nEntries);

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
// Define Argon
void MaterialManager::DefineArgon()
    {
        Configuration * config=Configuration::Instance();
        G4double pressure=config->GetPressure();
        //std::cout<<"Pressure is set to "<<pressure<<std::endl;
        G4NistManager* nistMan = G4NistManager::Instance();
        G4Material*      GasArgon   = nistMan->ConstructNewGasMaterial("GasAr","G4_Ar",293.15*kelvin,pressure*atmosphere);
        //std::cout<<"Pressure is set to "<< GasArgon->GetPressure()/atmosphere<<std::endl;

        G4double Abs=4*config->DiskRadius();
        const G4int nEntries = 17;
        G4double Rind_LAr[nEntries] = {1.00,  1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,  1.00, 1.00};
        G4double Absl_LAr[nEntries] = {0.55*m,  0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m,  0.55*m, 0.55*m };
        G4double lR_lAr[nEntries] = {0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m,0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m,0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m, 0.55*m};

        // distribution of produced optical photons
        //https://github.com/LArSoft/lardataalg/blob/develop/lardataalg/DetectorInfo/larproperties.fcl
        G4double LAr_phE[nEntries] ={ 6.0*eV,  6.7*eV,  7.1*eV,  7.4*eV,  7.7*eV, 7.9*eV,  8.1*eV,  8.4*eV,  8.5*eV,  8.6*eV,  8.8*eV,  9.0*eV,  9.1*eV,  9.4*eV,  9.8*eV,  10.4*eV,  10.7*eV};
        G4double LAr_SCINT[nEntries] ={0.0,  0.04, 0.12, 0.27, 0.44, 0.62, 0.80, 0.91, 0.92, 0.85, 0.70, 0.50, 0.31, 0.13, 0.04,  0.01, 0.0};

        G4MaterialPropertiesTable* LAr_MPT = new G4MaterialPropertiesTable();
        LAr_MPT ->AddProperty("FASTCOMPONENT",LAr_phE,LAr_SCINT,nEntries);
        LAr_MPT ->AddProperty("SLOWCOMPONENT",LAr_phE,LAr_SCINT,nEntries);
        //LAr_MPT->AddProperty("RAYLEIGH", LAr_phE, lR_lAr, nEntries);
        LAr_MPT -> AddProperty("RINDEX",LAr_phE,Rind_LAr,nEntries);
        LAr_MPT -> AddProperty("ABSLENGTH",LAr_phE,Absl_LAr,nEntries);
        LAr_MPT -> AddConstProperty ("SCINTILLATIONYIELD", 1/MeV);
        LAr_MPT -> AddConstProperty("RESOLUTIONSCALE",1);
        LAr_MPT -> AddConstProperty("FASTTIMECONSTANT",6.8*ns);
        LAr_MPT -> AddConstProperty("SLOWTIMECONSTANT",1590.*ns);
        LAr_MPT -> AddConstProperty("YIELDRATIO",1);

        GasArgon -> SetMaterialPropertiesTable(LAr_MPT);
        //GasArgon->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

        // Optical properties
        /*
        const G4int nEntries = fTPBEmissionE.size();
        G4double photonEnergy[nEntries];
        G4double ArgonRIndex[nEntries];
        G4double ArgonSpect[nEntries];
        G4double ArgonAbs[nEntries];
        G4double ArgonRIndexImg[nEntries];
        G4double ArgonRefl[nEntries];
        for (int i = 0; i < nEntries; i++)
        {
            photonEnergy[i] = fTPBEmissionE[i];
            ArgonRIndex[i]    = 1.00026735;
            ArgonRIndexImg[i] = 0.0;
            ArgonRefl[i]       = 0.97;
        }

        G4MaterialPropertiesTable *ArgonMPT = new G4MaterialPropertiesTable();
        ArgonMPT->AddProperty("RINDEX", photonEnergy, ArgonRIndex, nEntries);
        ArgonMPT->AddProperty("REALRINDEX", photonEnergy, ArgonRIndex, nEntries);
        ArgonMPT->AddProperty("IMAGINARYRINDEX", photonEnergy, ArgonRIndexImg, nEntries);
        // For border surface
        // Assuming 97% reflectivity
        //ArgonMPT->AddProperty("REFLECTIVITY", photonEnergy, ArgonRefl, nEntries);
        GasArgon->SetMaterialPropertiesTable(ArgonMPT);
        */
    }

void MaterialManager::DefineTPB()
{

    G4int netries=15;
    G4int netriesArg=17;
    //G4double Rind_LAr[netriesArg] = {1.49,  1.49,1.49,  1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49};
    G4double LAr_phE[netriesArg] ={ 6.0*eV,  6.7*eV,  7.1*eV,  7.4*eV,  7.7*eV, 7.9*eV,  8.1*eV,  8.4*eV,  8.5*eV,  8.6*eV,  8.8*eV,  9.0*eV,  9.1*eV,  9.4*eV,  9.68*eV,  10.4*eV,  10.7*eV};
    /*G4double tpb_e_energy[netries] = {3.125*eV, 3*eV, 2.875*eV, 2.75*eV, 2.625*eV, 2.5*eV,2.375*eV, 2.25*eV, 2.125*eV, 2*eV};
    G4double tpb_e[netries] = {0.120479, 0.718092, 1.00175, 0.640985, 0.389969, 0.157005,0.0509217, 0.0080711, 0.000595335, 2.04258e-05};
    G4double tpb_a_energy[netries] = {10.9729*eV, 7.07431*eV, 5.22486*eV, 4.2614*eV,3.71444*eV, 3.36*eV, 3.16*eV, 2.96*eV, 2.9064*eV, 2.901*eV};
    G4double tpb_l[netries] = {0.0594703*um, 0.0594703*um, 0.0573079*um, 0.106937*um,0.0323873*um, 0.0608373*um, 0.581187*um, 13.2291*um, 87.4751*um, 950.347*um};
    */
    G4double tpb_e_energy[netries] ={  0.05*eV,    1.0*eV,   1.5*eV,  2.25*eV, 2.481*eV,
                                       2.819*eV,  2.952*eV, 2.988*eV, 3.024*eV,   3.1*eV,
                                       3.14*eV, 3.1807*eV,  3.54*eV,   5.5*eV, 50.39*eV};
    G4double tpb_e[netries]= {    0.0,   0.0, 0.0, 0.0588, 0.235,
                                          0.853,   1.0, 1.0, 0.9259, 0.704,
                                          0.0296, 0.011, 0.0,    0.0,   0.0};

    G4int nentriesAbs=11;
    G4double tpb_a_energy[nentriesAbs]={0.05*eV,1.77*eV,2.0675*eV,2.98*eV, 7.42*eV, 7.75*eV, 8.16*eV, 8.73*eV, 9.78*eV,10.69*eV, 50.39*eV};
    G4double tpb_l[nentriesAbs]={100000.0,100000.0, 100000.0,1.0,0.001,0.00000000001,0.00000000001, 0.00000000001, 0.00000000001, 0.00000000001, 0.00000000001};
    G4double Rind_LAr[nentriesAbs] = {  1.49,1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,  1.49, 1.49};


    G4Material *tpb_mat=new G4Material("TPB",1.079*g/cm3,2);

    G4Element* H = new G4Element ("Hydrogen","H",1.,1.01*g/mole);
    G4Element* C = new G4Element ("Carbon","C",6.,12.01*g/mole);

    tpb_mat->AddElement (C, 28);
    tpb_mat->AddElement (H, 22);


    G4MaterialPropertiesTable * tpb = new G4MaterialPropertiesTable();
    tpb->AddProperty("RINDEX",tpb_a_energy,Rind_LAr,nentriesAbs);
    tpb->AddProperty("WLSABSLENGTH",tpb_a_energy,tpb_l,nentriesAbs);
    tpb->AddProperty("WLSCOMPONENT",tpb_e_energy,tpb_e,netries);
    tpb->AddConstProperty("WLSTIMECONSTANT", 2.5*ns);
    tpb_mat->SetMaterialPropertiesTable(tpb);

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
  G4double Abs=2*config->DiskThickness();

  a = 1.01*g/mole;
  G4Element* elH  = new G4Element(name="Hydrogen", symbol="H", z=1., a); 
  a = 12.01*g/mole;
  G4Element* elC  = new G4Element(name="Carbon",   symbol="C", z=6., a);
  a = 14.01*g/mole;
  G4Element* elN  = new G4Element(name="Nitrogen", symbol="N", z=7., a);
  a = 16.00*g/mole;
  G4Element* elO  = new G4Element(name="Oxygen",   symbol="O", z=8., a);

  //Polystyrene
    //acrylic->AddElement(elC, 8);
    //acrylic->AddElement(elH, 8);

  //Acrylic
  density = 1.19*g/cm3;
  G4Material* acrylic = new G4Material(name="Acrylic", density, 3);
  acrylic->AddElement(elC, 5);
  acrylic->AddElement(elH, 8);
  acrylic->AddElement(elO, 2);
    G4MaterialPropertiesTable *acrylicMPT = new G4MaterialPropertiesTable();
   /* G4int nEntries=50;
    G4double photonEnergy[] =
            {2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
             2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
             2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
             2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
             2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
             2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
             2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
             3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
             3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
             3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};
    G4double refractiveIndexWLSfiber[] =
            { 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
              1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60};

    assert(sizeof(refractiveIndexWLSfiber) == sizeof(photonEnergy));

    G4double absWLSfiber[] =
            {5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
             5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
             5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,1.10*m,
             1.10*m,1.10*m,1.10*m,1.10*m,1.10*m,1.10*m, 1.*mm, 1.*mm, 1.*mm, 1.*mm,
             1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm};

    assert(sizeof(absWLSfiber) == sizeof(photonEnergy));

    G4double emissionFib[] =
            {0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
             3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
             12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
             15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
             0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};

    assert(sizeof(emissionFib) == sizeof(photonEnergy));

    // Add entries into properties table
    acrylicMPT->AddProperty("RINDEX",photonEnergy,refractiveIndexWLSfiber,nEntries);
     //acrylicMPT->AddProperty("ABSLENGTH",photonEnergy,absWLSfiber,nEntries);
    acrylicMPT->AddProperty("WLSABSLENGTH",photonEnergy,absWLSfiber,nEntries);
    acrylicMPT->AddProperty("WLSCOMPONENT",photonEnergy,emissionFib,nEntries);
    acrylicMPT->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);

    */

  // Optical properties 
  // TPB Emission: LArSoft package (lardataalg)
  // Refractive index: arXiv:1101.3013v1
  // Absorption length --> infinity: ref therein arXiv:1307.6906v2
  const G4int nEntries = fTPBEmissionE.size();
    //const G4int nEntries =10;
  G4double EmphotonEnergy[nEntries];  //= {3.125*eV, 3*eV, 2.875*eV, 2.75*eV, 2.625*eV, 2.5*eV,2.375*eV, 2.25*eV, 2.125*eV, 2*eV};;
  G4double acrylicRIndex[nEntries];
  G4double acrylicRIndexImg[nEntries];
  G4double acrylicSpect[nEntries];  //={0.120479, 0.718092, 1.00175, 0.640985, 0.389969, 0.157005,0.0509217, 0.0080711, 0.000595335, 2.04258e-05};;
  G4double acrylicAbs[nEntries];
  G4double acrylicESpect[nEntries];


    for (int i = 0; i < nEntries; i++)
  {
    EmphotonEnergy[i]     = fTPBEmissionE[i];
    acrylicRIndex[i]    = 1.49;
    acrylicRIndexImg[i] = 0.0;
    acrylicSpect[i]     = fTPBEmissionSpect[i];
    acrylicESpect[i]    = 1.0;
    acrylicAbs[i]       = 100*m;
  }

  acrylicMPT->AddProperty("RINDEX",          EmphotonEnergy, acrylicRIndex,    nEntries);
  acrylicMPT->AddProperty("REALRINDEX",      EmphotonEnergy, acrylicRIndex,    nEntries);
  acrylicMPT->AddProperty("IMAGINARYRINDEX", EmphotonEnergy, acrylicRIndexImg, nEntries);
  acrylicMPT->AddProperty("ABSLENGTH",       EmphotonEnergy, acrylicAbs,       nEntries);
  acrylicMPT->AddProperty("EMISSIONSPECT",   EmphotonEnergy, acrylicSpect,     nEntries);


    // Set the Birks Constant for the Polystyrene scintillator
    acrylic->SetMaterialPropertiesTable(acrylicMPT);

    //acrylic->GetIonisation()->SetBirksConstant(0.126*mm/MeV);

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

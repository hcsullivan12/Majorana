/**
 * @file PhysicsList.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to intialize the physics to be implemented.
 * @date 07-04-2019
 * 
 */

#include "majsim/PhysicsList.h"
#include "majsim/OpticalPhysics.h"
#include "G4EmStandardPhysics_option1.hh"
namespace majsim {

PhysicsList::PhysicsList() 
 : G4VModularPhysicsList()
{
  /* // Old stufss
  G4VModularPhysicsList* phys = new FTFP_BERT(0);
  fOpticalPhysics = new OpticalPhysics;
  RegisterPhysics(fOpticalPhysics);
  */

    //add new units for radioActive decays
    //
    const G4double minute = 60*second;
    const G4double hour   = 60*minute;
    const G4double day    = 24*hour;
    const G4double year   = 365*day;
    new G4UnitDefinition("minute", "min", "Time", minute);
    new G4UnitDefinition("hour",   "h",   "Time", hour);
    new G4UnitDefinition("day",    "d",   "Time", day);
    new G4UnitDefinition("year",   "y",   "Time", year);

    // mandatory for G4NuclideTable
    //
    G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(0.1*picosecond);
    G4NuclideTable::GetInstance()->SetLevelTolerance(1.0*eV);

    //read new PhotonEvaporation data set
    //
    G4DeexPrecoParameters* deex = G4NuclearLevelData::GetInstance()->GetParameters();
    deex->SetCorrelatedGamma(false);
    deex->SetStoreAllLevels(true);
    deex->SetMaxLifeTime(G4NuclideTable::GetInstance()->GetThresholdOfHalfLife()/std::log(2.));
    ReplacePhysics(new G4EmStandardPhysics_option4);
    /*G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
    opticalPhysics->SetWLSTimeProfile("delta"); //Wavelength shifting
    opticalPhysics -> SetScintillationYieldFactor(1.0);
    opticalPhysics -> SetScintillationExcitationRatio(0.0);
    opticalPhysics->SetTrackSecondariesFirst(kScintillation,true);

     RegisterPhysics(opticalPhysics);
     */
    RegisterPhysics(new OpticalPhysics);

}

PhysicsList::~PhysicsList()
{}

void PhysicsList::ConstructParticle()
{

// Optical
    G4VModularPhysicsList::ConstructParticle();

    // pseudo-particles

     G4Geantino::GeantinoDefinition();

     // gamma
     G4Gamma::GammaDefinition();


     // leptons
     G4Electron::ElectronDefinition();
     G4Positron::PositronDefinition();

     G4NeutrinoE::NeutrinoEDefinition();
     G4AntiNeutrinoE::AntiNeutrinoEDefinition();

     // baryons
     G4Proton::ProtonDefinition();
     G4Neutron::NeutronDefinition();

     G4Alpha::AlphaDefinition();
     // ions
     G4IonConstructor iConstructor;
     iConstructor.ConstructParticle();





}

void PhysicsList::ConstructProcess()
{
    //AddTransportation();

    G4Radioactivation* radioactiveDecay = new G4Radioactivation();

    G4bool ARMflag = false;
    radioactiveDecay->SetARM(ARMflag);        //Atomic Rearangement

    // need to initialize atomic deexcitation
    //
    G4LossTableManager* man = G4LossTableManager::Instance();
    G4VAtomDeexcitation* deex = man->AtomDeexcitation();
    if (!deex) {
        ///G4EmParameters::Instance()->SetFluo(true);
        G4EmParameters::Instance()->SetAugerCascade(ARMflag);
        deex = new G4UAtomicDeexcitation();
        deex->InitialiseAtomicDeexcitation();
        man->SetAtomDeexcitation(deex);
    }

    // register radioactiveDecay
    //
    //G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    //ph->RegisterProcess(radioactiveDecay, G4GenericIon::GenericIon());
    G4VModularPhysicsList::RegisterProcess(radioactiveDecay,G4GenericIon::GenericIon());
    //printout
    //
    G4cout << "\n  Set atomic relaxation mode " << ARMflag << G4endl;
    //optical
    G4VModularPhysicsList::ConstructProcess();


}
    void PhysicsList::SetCuts(){}


}

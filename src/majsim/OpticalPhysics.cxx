/**
 * @file OpticalPhysics.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Class to initialize the optical physics to be implented. 
 * @date 07-04-2019
 * 
 */

#include "majsim/OpticalPhysics.h"
#include "G4LossTableManager.hh"
 #include "G4EmSaturation.hh"
#include "G4VPhysicsConstructor.hh"
//#include "majsim/MaterialManager.h"


 // factory
 #include "G4PhysicsConstructorFactory.hh"
namespace majsim {

OpticalPhysics::OpticalPhysics()
 : G4VPhysicsConstructor("Optical"),
   fBoundaryProcess(NULL),
   fAbsorptionProcess(NULL)
{
  fAbsorptionProcess   = new G4OpAbsorption();
  fBoundaryProcess     = new OpBoundaryProcessv2();
}

OpticalPhysics::~OpticalPhysics() 
{}

void OpticalPhysics::ConstructParticle()
{
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void OpticalPhysics::ConstructProcess()
{  
  G4ProcessManager* pManager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();

  fScintProcess=new G4Scintillation();
  fWLSProcess =new G4OpWLS();

    if (!pManager  )
  {
     std::ostringstream o;
     o << "Optical Photon without a Process Manager";
     G4Exception("OpticalPhysics::ConstructProcess()", "", FatalException,o.str().c_str());
  }
    fWLSProcess->UseTimeProfile("delta"); //Wavelength shifting
    fScintProcess -> SetScintillationYieldFactor(1.0);
    fScintProcess -> SetScintillationExcitationRatio(0.0);
    //fScintProcess->SetScintillationByParticleType(true);
    fScintProcess ->SetTrackSecondariesFirst(true);


    // Use Birks Correction in the Scintillation process
       G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
       fScintProcess->AddSaturation(emSaturation);

    pManager->AddDiscreteProcess(fAbsorptionProcess);
    pManager->AddDiscreteProcess(fWLSProcess);
    pManager->AddDiscreteProcess(fBoundaryProcess);
    auto myIt=GetParticleIterator();
    myIt->reset();

    while((*myIt)()){
        G4ParticleDefinition* particle = myIt->value();
        G4String particleName = particle->GetParticleName();
        pManager=particle->GetProcessManager();
        if (!pManager) {
            std::ostringstream o;
            o << "Particle " << particleName << "without a Process Manager";
            G4Exception("G4OpticalPhysics::ConstructProcess()","",
                        FatalException,o.str().c_str());
            return;                 // else coverity complains for pManager use below
        }
        if( fScintProcess->IsApplicable(*particle) ){
            std::cout<<"Particle Name that Scintilation Applices "<<particle->GetParticleName()<<std::endl;
            pManager->AddProcess(fScintProcess);
            pManager->SetProcessOrderingToLast(fScintProcess,idxAtRest);
            if(fScintProcess->isAlongStepDoItIsEnabled())
                pManager->SetProcessOrdering(fScintProcess,idxAlongStep);
            pManager->SetProcessOrderingToLast(fScintProcess,idxPostStep);
            //pManager->DumpInfo();



        }



    }



}

}

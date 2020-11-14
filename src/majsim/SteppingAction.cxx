/**
 * @file SteppingAction.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Stepping action.
 * @date 07-04-2019
 * 
 */

#include "majsim/SteppingAction.h"
#include "majsim/Configuration.h"
#include "G4OpBoundaryProcess.hh"
#include "G4Scintillation.hh"



namespace majsim 
{

SteppingAction::SteppingAction()
{
  time_t seed = time( NULL );
  fRandomEngine.setSeed(static_cast<long>(seed+1));
  fPhAbsCount=0;
  fPhDetecCount=0;


}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* theStep)
{
  G4Track*           theTrack     = theStep->GetTrack();
  G4VUserTrackInformation *TrackInfo =theTrack->GetUserInformation();
  G4StepPoint*       thePrePoint  = theStep->GetPreStepPoint();
  G4StepPoint*       thePostPoint = theStep->GetPostStepPoint();
  G4VPhysicalVolume* thePostPV    = thePostPoint->GetPhysicalVolume();
  G4VPhysicalVolume* thePrePV     = thePrePoint->GetPhysicalVolume();
  auto               thePosition  = thePrePoint->GetPosition();


    if (!thePostPV) return;
  G4ParticleDefinition* particleType = theTrack->GetDefinition();
    Counts = majutil::OpDetPhotonTable::Instance();
    G4double E=theTrack->GetTotalEnergy()/eV;


    if(particleType!=G4OpticalPhoton::OpticalPhotonDefinition()){
      /*if(theTrack->GetParentID()>0){
      G4ProcessManager * pManager=  particleType->GetProcessManager();
      G4Scintillation * pr= new G4Scintillation();
        pManager->AddProcess(pr);
        pManager->SetProcessOrderingToLast(pr,idxAtRest);
        pManager->SetProcessOrderingToLast(pr,idxPostStep);
      }*/
      if(particleType==G4Electron::ElectronDefinition() && theTrack->GetCurrentStepNumber()==1 )
          Counts->AddElectronE(E);
      if(particleType==G4Neutron::NeutronDefinition() && theTrack->GetCurrentStepNumber()==1)
          Counts->AddNeutronE(E);

      if(particleType==G4Alpha::AlphaDefinition() && theTrack->GetCurrentStepNumber()==1)
          Counts->AddAlphaE(E);


  }
  else{
    if(theTrack->GetCurrentStepNumber()==1)
      Counts->AddPhotonE(E);
    if(theTrack->GetParentID()==0) {
        //This is a primary track

        if ( theTrack->GetCurrentStepNumber() == 1 ) {
//        G4double x  = theTrack->GetVertexPosition().x();
//        G4double y  = theTrack->GetVertexPosition().y();
//        G4double z  = theTrack->GetVertexPosition().z();
//        G4double pz = theTrack->GetVertexMomentumDirection().z();
//        G4double fInitTheta =
//                         theTrack->GetVertexMomentumDirection().angle(ZHat);
        }
    }

          //*************
  // We will handle surface absorption here
  // Make sure we're at the acrylic/air boundary
  G4String preName  = thePrePV->GetName();
  G4String postName = thePostPV->GetName();

    G4OpBoundaryProcessStatus boundaryStatus=Undefined;
    static G4ThreadLocal G4OpBoundaryProcess* boundary = nullptr;

    //find the boundary process only once
    if(!boundary){
        G4ProcessManager* pm
                = theStep->GetTrack()->GetDefinition()->GetProcessManager();
        G4int nprocesses = pm->GetProcessListLength();
        G4ProcessVector* pv = pm->GetProcessList();
        G4int i;
        for( i=0;i<nprocesses;i++){
            if((*pv)[i]->GetProcessName()=="Transport" && postName=="volMPPC"){
                PhAbsCount();
                G4cout<<"PhAbsCount = " <<GetPhAbsCount()<<G4endl;
            }
        }
    }



    //if ( (preName == "volWorld" || preName == "volDisk" || preName == "volTPB") && postName.find(SIPM,SIPM.size())!=SIPM.size())
    if  ((preName == "volWorld" && postName == "volDisk")) //|| preName == "volDisk" && postName == "volTPB" )
  {
    Configuration* config = Configuration::Instance();
    float surfaceAbs = config->SurfaceAbsorption();
    CLHEP::RandFlat   flat(fRandomEngine);
    float u = flat.fire();
    if (u < surfaceAbs) 
    {
      theTrack->SetTrackStatus(fStopAndKill);
      majutil::OpDetPhotonTable* photonTable = majutil::OpDetPhotonTable::Instance();
      photonTable->IncPhotonsAbs();
        //std::cout<<"Detected Photons"<<std::endl;

    }
  }
}
}
}

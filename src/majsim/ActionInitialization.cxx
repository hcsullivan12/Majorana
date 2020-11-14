/**
 * @file ActionInitialization.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Action intialization.
 * @date 07-04-2019
 * 
 */

#include "majsim/ActionInitialization.h"
#include "majsim/PrimaryGeneratorAction.h"
//#include "RunAction.hh"
//#include "EventAction.hh"
//#include "TrackingAction.hh"
//#include "SteppingVerbose.hh"


namespace majsim
{

ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization(),
   fGeneratorAction(NULL),
   fSteppingAction(NULL)
{
  fGeneratorAction = new PrimaryGeneratorAction();
  fSteppingAction  = new SteppingAction;
}

    ActionInitialization::~ActionInitialization()
    {}
    void ActionInitialization::DumpResults()
    {
        G4cout<<"--------Dumping the Results-----"<<G4endl;
        G4cout<<"Abs Photons= "<<fSteppingAction->GetPhAbsCount()<<G4endl;
        G4cout<<"Detected= "<<fSteppingAction->GetPhDetectCount()<<G4endl;

    }

    void ActionInitialization::Build() const
    {
      SetUserAction(fGeneratorAction);
      SetUserAction(fSteppingAction);
        G4cout<<"--------Dumping the Results-----"<<G4endl;
        G4cout<<"Abs Photons= "<<fSteppingAction->GetPhAbsCount()<<G4endl;
        G4cout<<"Detected= "<<fSteppingAction->GetPhDetectCount()<<G4endl;
        /*RunAction* runAction = new RunAction(primary);
        SetUserAction(runAction);

        EventAction* eventAction = new EventAction();
        SetUserAction(eventAction);

        TrackingAction* trackingAction = new TrackingAction(eventAction);
        SetUserAction(trackingAction);
        */
    }

    /*G4VSteppingVerbose* ActionInitialization::InitializeSteppingVerbose() const
    {
        //return new SteppingVerbose();

    }
     */


}

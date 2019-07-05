/**
 * @file ActionInitialization.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Action intialization.
 * @date 07-04-2019
 * 
 */

#include "majsim/ActionInitialization.h"

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

void ActionInitialization::Build() const
{
  SetUserAction(fGeneratorAction);
  SetUserAction(fSteppingAction);
}

}

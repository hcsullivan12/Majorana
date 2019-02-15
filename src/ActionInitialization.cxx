// 
// File: ActionInitialization.cxx
//
// Author: Hunter Sullivan
//
// Discription: 
//

#include "ActionInitialization.h"

namespace majorana
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

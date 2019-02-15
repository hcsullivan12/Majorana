// 
// File: ActionInitialization.h
//
// Author: Hunter Sullivan
//
// Discription: 
//


#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H 

#include "PrimaryGeneratorAction.h"
#include "SteppingAction.h"
 
#include "G4VUserActionInitialization.hh"

namespace majorana
{

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    virtual ~ActionInitialization();

    virtual void Build() const;

    PrimaryGeneratorAction* GetGeneratorAction() const { return fGeneratorAction; };

  private:
    PrimaryGeneratorAction*  fGeneratorAction;
    SteppingAction*          fSteppingAction;
};

}

#endif

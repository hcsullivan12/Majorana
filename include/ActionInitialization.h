/**
 * @file ActionInitialization.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Action intialization.
 * @date 07-04-2019
 * 
 */


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
    PrimaryGeneratorAction*  fGeneratorAction; ///< Pointer to generator action 
    SteppingAction*          fSteppingAction;  ///< Pointer to stepping action
};

}

#endif

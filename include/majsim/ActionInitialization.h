/**
 * @file ActionInitialization.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Action intialization.
 * @date 07-04-2019
 * 
 */


#ifndef MAJSIM_ACTIONINITIALIZATION_H
#define MAJSIM_ACTIONINITIALIZATION_H 

#include "majsim/PrimaryGeneratorAction.h"
#include "majsim/SteppingAction.h"
 
#include "G4VUserActionInitialization.hh"

namespace majsim
{

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    virtual ~ActionInitialization();

    virtual void Build() const;
    //virtual void BuildForMaster() const;
    //virtual G4VSteppingVerbose* InitializeSteppingVerbose() const;

    PrimaryGeneratorAction* GetGeneratorAction() const { return fGeneratorAction; };
    void DumpResults();


private:
    PrimaryGeneratorAction*  fGeneratorAction; ///< Pointer to generator action 
    SteppingAction*          fSteppingAction;  ///< Pointer to stepping action
    G4int                    fScintCount;
    G4int                    fCerenkovCount;
    G4int                    fOWLSCount;

};

}

#endif

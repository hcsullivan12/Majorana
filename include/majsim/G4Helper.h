/**
 * @file G4Helper.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Helper class to interface with and run Geant4.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_G4HELPER_H
#define MAJSIM_G4HELPER_H

#include "majsim/DetectorConstruction.h"
#include "majsim/PrimaryGeneratorAction.h"
#include "majsim/ActionInitialization.h"
#include "majsim/PhysicsList.h"

// Geant4 includes
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4StateManager.hh"
#include "G4HadronicProcessStore.hh"
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif


namespace majsim
{

class G4Helper 
{
  public:
    static G4Helper* Instance();
    static G4Helper* CreateInstance();
    ~G4Helper();

    /**
     * @brief Container for light source positions for all events.
     * @todo Consider the case when the steering file becomes large. 
     *       May need to find a better solution than just storing all
     *       events in memory at runtime. 
     * 
     */
    using SteeringTable = std::vector<std::vector<float>>;

    /**
     * @brief Begin main geant4 simulation.
     * 
     */
    void StartG4();

    ActionInitialization* GetActionInitialization() const { return fActionInitialization; };
    DetectorConstruction* GetDetectorConstruction() const { return fDetector; };

  private:
    G4Helper();
    static G4Helper* instance;

    /**
     * @brief Initialize the visualization manager.
     * 
     */
    void HandleVisualization();

    /**
     * @brief Initializes event generator and tells geant4 to run.
     * 
     */
    void RunG4();

    /**
     * @brief Set verbosities for simulation.
     * 
     */
    void HandleVerbosities();

    G4RunManager*           fRunManager;            
    G4UImanager*            fUIManager;             
    G4VisExecutive*         fVisManager;           
    ActionInitialization*   fActionInitialization;   
    PrimaryGeneratorAction* fGeneratorAction;       ///< Generator that handles emission 
    DetectorConstruction*   fDetector;              ///< Pointer to geometries
    PhysicsList*            fPhysicsList;           
    SteeringTable           fSteeringTable;         
    std::string             fSteeringFilePath;       
    bool                    fShowVis;                
    std::string             fVisMacroPath;           
    std::string             fSimulateOutputPath;     
};
}

#endif

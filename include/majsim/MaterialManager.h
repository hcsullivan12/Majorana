/**
 * @file MaterialManager.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton for handling geometry materials.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_MATERIALMANAGER_H
#define MAJSIM_MATERIALMANAGER_H

#include "G4Material.hh"
#include "Configuration.h"

namespace majsim
{

class MaterialManager
{
  public:
    static MaterialManager* Instance();
    static MaterialManager* CreateInstance();
    ~MaterialManager();

    void        ConstructMaterials();
    G4Material* FindMaterial(const G4String& name);
    Configuration * config;
    std::vector<float> GetPhotonEnergies() const { return fTPBEmissionE; };

  private:
    MaterialManager();
    static MaterialManager* instance;

    void DefineAir();
    void DefineAcrylic();

    void DefineMPPCMaterial();
    void DefineArgon();
    void DefineTPB();

 
    std::vector<float> fTPBEmissionE;
    std::vector<float> fTPBEmissionSpect;

    std::vector<float> fTPBAbsE;
    std::vector<float> fTPBAbsSpect;
};
}

#endif

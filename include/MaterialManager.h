/**
 * @file MaterialManager.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton for handling geometry materials.
 * @date 07-04-2019
 * 
 */

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "G4Material.hh"

namespace majorana
{

class MaterialManager
{
  public:
    static MaterialManager* Instance();
    static MaterialManager* CreateInstance();
    ~MaterialManager();

    void        ConstructMaterials();
    G4Material* FindMaterial(const G4String& name);

    std::vector<float> GetPhotonEnergies() const { return fTPBEmissionE; };

  private:
    MaterialManager();
    static MaterialManager* instance;

    void DefineAir();
    void DefineAcrylic();
    void DefineMPPCMaterial();
 
    std::vector<float> fTPBEmissionE;
    std::vector<float> fTPBEmissionSpect; 
};
}

#endif

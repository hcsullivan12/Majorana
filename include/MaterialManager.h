// 
// File: MatieralManager.h
//
// Author: Hunter Sullivan
//
// Discription: Class to construct detector materials.
//

#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "G4Material.hh"

namespace majorana
{

class MaterialManager
{
  public:
    static MaterialManager* Instance();
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

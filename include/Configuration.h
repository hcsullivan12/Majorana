// 
// File: Configuration.h
//
// Author: Hunter Sullivan
//
// Discription: Class to initalize configuration.
//

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <array>
#include <vector>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/rapidjson.h"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Types.hh"

namespace majorana
{

class Configuration
{
  public:
    static Configuration* Instance();
    static Configuration* CreateInstance();
    ~Configuration();

    struct SteeringTableIndex
    {
      G4double x;
      G4double y;
      G4double r;
      G4double thetaDeg;

      G4int    voxelID;
      G4int    n;
    };

    using SteeringTable = std::vector<SteeringTableIndex>; 
 
    void Initialize(const std::string& configPath);
    void SetVisualization(const bool& b) { fShowVis = b; };
    void ReadSteeringFile();

    unsigned    NMPPCs()               const { return fNMPPCs; };
    G4double    MPPCHalfLength()       const { return fMPPCHalfLength; };
    G4double    DiskRadius()           const { return fDiskRadius; };
    G4double    DiskThickness()        const { return fDiskThickness; };
    bool        ShowVis()              const { return fShowVis; };
    bool        Reconstruct()          const { return fReconstruct; };
    std::string VisMacroPath()         const { return fVisMacroPath; };
    std::string SteeringFilePath()     const { return fSteeringFilePath; };
    std::string SimulateOutputPath()   const { return fSimulateOutputPath; };
    std::string SourceMode()           const { return fSourceMode; };
    std::string VoxelizationPath()     const { return fVoxelizationPath; };
    std::string OpReferenceTablePath() const { return fOpReferenceTablePath; };
    std::string RecoAnaTreePath()      const { return fRecoAnaTreePath; };
    G4double    SourcePosSigma()       const { return fSourcePosSigma; };
    G4double    SourcePeakE()          const { return fSourcePeakE; };
    G4double    SourcePeakESigma()     const { return fSourcePeakESigma; };
    G4double    SurfaceRoughness()     const { return fSurfaceRoughness; };
    G4double    SurfaceAbsorption()    const { return fSurfaceAbsorption; };
    const SteeringTable& GetSteeringTable()   const { return fSteeringTable; };
    
  private:
    Configuration();
    static Configuration* instance;

    void ReadJSONFile();
    const rapidjson::Value& GetJSONMember(const std::string&     memberName,
                                          rapidjson::Type        memberType,
                                          const unsigned&        arraySize = 0,
                                          const rapidjson::Type& arrayType = rapidjson::kNullType);
    void CheckConfiguration();
    void PrintConfiguration();

    rapidjson::Document              fJSONDoc;
    const std::array<std::string, 7> fJSONTypes = {{"Null", "False", "True", "Object", "Array", "String", "Number"}};

    std::string fConfigPath;
    std::string fSimulateOutputPath;
    std::string fRecoAnaTreePath;
    std::string fSteeringFilePath;
    std::string fVoxelizationPath;
    std::string fVisMacroPath;
    std::string fSourceMode;
    std::string fOpReferenceTablePath;

    unsigned    fNMPPCs;
    G4double    fMPPCHalfLength;
    G4double    fDiskRadius;
    G4double    fDiskThickness;
    G4double    fSourcePosSigma;
    G4double    fSourcePeakE;
    G4double    fSourcePeakESigma;
    G4double    fSurfaceRoughness;
    G4double    fSurfaceAbsorption;

    bool        fReconstruct;
    bool        fShowVis;

    std::vector<SteeringTableIndex> fSteeringTable;
};
}

#endif

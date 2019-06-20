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

      G4int    pixelID;
      G4int    n;
    };

    using SteeringTable = std::vector<SteeringTableIndex>; 
 
    void Initialize(const std::string& configPath);
    void SetVisualization(const bool& b) { fShowVis = b; };
    void SetNSiPMs(const unsigned& n) { fNMPPCs = n; };
    void SetPixelPath(const std::string& p) { fPixelizationPath = p; };
    void SetOpRefTablePath(const std::string& p) { fOpReferenceTablePath = p; };
    void SetSimOutputPath(const std::string& p) { fSimulateOutputPath = p; };
    void SetRecoAnaPath(const std::string& p) { fRecoAnaTreePath = p; };
    void SetEvdMode(const bool& b) { fEvdMode = b; };

    void ReadSteeringFile();
    void CheckConfiguration();
    void PrintConfiguration();

    unsigned    NMPPCs()               const { return fNMPPCs; };
    G4double    MPPCHalfLength()       const { return fMPPCHalfLength; };
    G4double    DiskRadius()           const { return fDiskRadius; };
    G4double    DiskThickness()        const { return fDiskThickness; };
    bool        ShowVis()              const { return fShowVis; };
    bool        Reconstruct()          const { return fReconstruct; };
    bool        EvdMode()              const { return fEvdMode; };
    std::string VisMacroPath()         const { return fVisMacroPath; };
    std::string SteeringFilePath()     const { return fSteeringFilePath; };
    std::string SimulateOutputPath()   const { return fSimulateOutputPath; };
    std::string SourceMode()           const { return fSourceMode; };
    std::string PixelizationPath()     const { return fPixelizationPath; };
    std::string OpReferenceTablePath() const { return fOpReferenceTablePath; };
    std::string RecoAnaTreePath()      const { return fRecoAnaTreePath; };
    std::string DAQFilePath()          const { return fDAQFilePath; };
    G4double    SourcePosSigma()       const { return fSourcePosSigma; };
    G4double    SourcePeakE()          const { return fSourcePeakE; };
    G4double    SourcePeakESigma()     const { return fSourcePeakESigma; };
    G4double    SurfaceRoughness()     const { return fSurfaceRoughness; };
    G4double    SurfaceAbsorption()    const { return fSurfaceAbsorption; };
    const SteeringTable& GetSteeringTable()   const { return fSteeringTable; };
    
  private:
    Configuration();
    static Configuration* instance;

    void ReadConfigFile();

    std::string fConfigPath;
    std::string fSimulateOutputPath;
    std::string fRecoAnaTreePath;
    std::string fSteeringFilePath;
    std::string fPixelizationPath;
    std::string fVisMacroPath;
    std::string fSourceMode;
    std::string fOpReferenceTablePath;
    std::string fDAQFilePath;

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
    bool        fEvdMode;

    std::vector<SteeringTableIndex> fSteeringTable;
};
}

#endif

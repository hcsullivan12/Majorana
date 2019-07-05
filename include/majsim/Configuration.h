/**
 * @file Configuration.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton to hold user configuration for simulation.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_CONFIGURATION_H
#define MAJSIM_CONFIGURATION_H

#include <string>
#include <array>
#include <vector>
#include <iostream>

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Types.hh"

namespace majsim
{

class Configuration
{
  public:

    static Configuration* Instance();
    static Configuration* CreateInstance();
    ~Configuration();

    /**
     * @brief Container for storing light source information.
     * 
     */
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
 
    /**
     * @brief Initialize our configuration.
     * 
     * @param configPath The path to the configuration file.
     */
    void Initialize(const std::string& configPath);
    
    void SetVisualization(const bool& b) { fShowVis = b; };
    void SetNSiPMs(const size_t& n) { fNMPPCs = n; };
    void SetPixelPath(const std::string& p) { fPixelizationPath = p; };
    void SetOpRefTablePath(const std::string& p) { fOpReferenceTablePath = p; };
    void SetSimOutputPath(const std::string& p) { fSimulateOutputPath = p; };
    void SetEvdMode(const bool& b) { fEvdMode = b; };

    /**
     * @brief Reads the light steering file and stores the light source information
     *        for each event
     * 
     */
    void ReadSteeringFile();

    /**
     * @brief Validate the configuration.
     * 
     */
    void CheckConfiguration();
    void PrintConfiguration();

    size_t      NMPPCs()               const { return fNMPPCs; };
    G4double    MPPCHalfLength()       const { return fMPPCHalfLength; };
    G4double    DiskRadius()           const { return fDiskRadius; };
    G4double    DiskThickness()        const { return fDiskThickness; };
    bool        ShowVis()              const { return fShowVis; };
    bool        EvdMode()              const { return fEvdMode; };
    std::string VisMacroPath()         const { return fVisMacroPath; };
    std::string SteeringFilePath()     const { return fSteeringFilePath; };
    std::string SimulateOutputPath()   const { return fSimulateOutputPath; };
    std::string SourceMode()           const { return fSourceMode; };
    std::string PixelizationPath()     const { return fPixelizationPath; };
    std::string OpReferenceTablePath() const { return fOpReferenceTablePath; };
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

    /**
     * @brief Read the configuration file that's passed at runtime
     * 
     */
    void ReadConfigFile();

    std::string fConfigPath;             ///< Path to configuration file
    std::string fSimulateOutputPath;     ///< Output path for simulation
    std::string fSteeringFilePath;       ///< Path to light steering file
    std::string fPixelizationPath;       ///< Path to pixelization scheme
    std::string fVisMacroPath;           ///< Path to g4 visual macro
    std::string fSourceMode;             ///< Source mode: pixel mode is true point source while point mode has a spread
    std::string fOpReferenceTablePath;   ///< Path to optical lookup table
    std::string fDAQFilePath;            ///< Path to DAQ file, used for event display
    size_t      fNMPPCs;                 ///< Number of sipms
    G4double    fMPPCHalfLength;         ///< Half length of sipm
    G4double    fDiskRadius;             ///< Disk radius
    G4double    fDiskThickness;          ///< Disk thickness
    G4double    fSourcePosSigma;         ///< The spread about the mean position
    G4double    fSourcePeakE;            ///< TPB emission peak energy
    G4double    fSourcePeakESigma;       ///< TPB emission peak energy sigma
    G4double    fSurfaceRoughness;       ///< Surface roughness, valid range is [0,1]
    G4double    fSurfaceAbsorption;      ///< Surface absorption coefficient, valid range is [0,1]
    bool        fShowVis;                ///< Option to render visualization
    bool        fEvdMode;                ///< Option for running with event display
    std::vector<SteeringTableIndex> fSteeringTable; ///< Container for information on each event to be simulated 
};
}

#endif

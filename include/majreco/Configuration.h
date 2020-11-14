/**
 * @file Configuration.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton to hold user configuration for simulation 
 *        and reconstruction.
 * @date 07-04-2019
 * 
 */

#ifndef MAJRECO_CONFIGURATION_H
#define MAJRECO_CONFIGURATION_H

#include <string>
#include <array>
#include <vector>
#include <iostream>

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Types.hh"

namespace majreco
{

class Configuration
{
  public:

    static Configuration* Instance();
    static Configuration* CreateInstance();
    ~Configuration();
 
    /**
     * @brief Initialize our configuration.
     * 
     * @param configPath The path to the configuration file.
     */
    void Initialize(const std::string& configPath);

    /**
     * @brief Validate the configuration.
     * 
     */
    void CheckConfiguration();
    void PrintConfiguration();

    size_t      PenalizedStopId()      const { return fPenalizedStopId; }
    size_t      UnpenalizedStopId()    const { return fUnpenalizedStopId; }
    float       Gamma()                const { return fGamma; }
    bool        DoPenalized()          const { return fDoPenalized; };
    std::string SimulateOutputPath()   const { return fSimulateOutputPath; };
    std::string PixelizationPath()     const { return fPixelizationPath; };
    std::string OpReferenceTablePath() const { return fOpReferenceTablePath; };
    std::string RecoOutputPath()       const { return fRecoOutputPath; };
    float diskRadius()                 const { return fdiskRadius; };
    std::string TrueFilePath()         const { return fTrueFilePath; };
    std::string FileName()             const { return fFileName; };
    std::string GetRecoDataFilePath()  const {return fRecoDataFilePath;};
    std::string GetPathToSavePics()    const {return fPathToSavePics;};
    std::string GetPathToDeadChannels()  const{return fPathToDeadChanels;};




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
    std::string fRecoOutputPath;         ///< Output path for reconstruction anatree
    std::string fPixelizationPath;       ///< Path to pixelization scheme
    std::string fOpReferenceTablePath;   ///< Path to optical lookup table
    std::string fTrueFilePath;           ///< Path to True Data points
    std::string fFileName;               ///< FileName
    size_t      fPenalizedStopId;        ///< Iteration number to stop penalized reconstruction
    size_t      fUnpenalizedStopId;      ///< Iteration number to stop unpenalized reconstruction
    float       fGamma;                  ///< Strength parameter used for penalized reconstruction
    bool        fDoPenalized;            ///< Option to run penalized reconstruction
    float       fdiskRadius;
    std::string fRecoDataFilePath;
    std::string fPathToSavePics;
    std::string fPathToDeadChanels;
};
}

#endif

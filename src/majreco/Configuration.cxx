/**
 * @file Configuration.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton to hold user configuration for simulation 
 *        and reconstruction.
 * @date 07-04-2019
 * 
 */

#include "majreco/Configuration.h"

#include <iomanip>
#include <math.h>
#include <fstream>
#include <assert.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>

// Preprocessing variables
#ifdef VERSION
#define majrecoVersion VERSION
#endif

#ifdef PROJECTDIR
#define majrecoDir PROJECTDIR
#endif

namespace majreco
{

Configuration* Configuration::instance = 0;

//------------------------------------------------------------------------
Configuration* Configuration::CreateInstance()
{
  if (instance == 0)
  {
    static Configuration config;
    instance = &config;
  }
  return instance;
}

//------------------------------------------------------------------------
Configuration* Configuration::Instance()
{
  assert(instance);
  return instance;
}

//------------------------------------------------------------------------
Configuration::Configuration()
{}

//------------------------------------------------------------------------
Configuration::~Configuration()
{}

//------------------------------------------------------------------------
void Configuration::Initialize(const std::string& configPath)
{
  fConfigPath = configPath;
  // Save the configuration
  ReadConfigFile();
}

//------------------------------------------------------------------------
void Configuration::ReadConfigFile()
{
  using boost::property_tree::ptree;
  ptree pt;
  read_ini(fConfigPath.c_str(), pt);

  // Grab the values 
  ptree& filesSect = pt.get_child("Files");
  ptree& recoSect  = pt.get_child("Reconstruction");

  // Reconstruction
  fDoPenalized        = "true" == recoSect.get_child("doPenalized").get_value("") ? true : false;
  fPenalizedStopId    = boost::lexical_cast<size_t>(recoSect.get_child("penalizedStopId").get_value(""));
  fUnpenalizedStopId  = boost::lexical_cast<size_t>(recoSect.get_child("unpenalizedStopId").get_value(""));
  fGamma              = boost::lexical_cast<float>(recoSect.get_child("gamma").get_value(""));
  // Files
  fSimulateOutputPath   = std::string(majrecoDir)+"/"+filesSect.get_child("simulateOutputPath").get_value("");
  fRecoOutputPath       = std::string(majrecoDir)+"/"+filesSect.get_child("recoOutputPath").get_value("");
  fOpReferenceTablePath = std::string(majrecoDir)+"/"+filesSect.get_child("opReferenceTablePath").get_value("");
  fPixelizationPath     = std::string(majrecoDir)+"/"+filesSect.get_child("pixelizationPath").get_value(""); 
}

//------------------------------------------------------------------------
void Configuration::CheckConfiguration()
{
  
}

//------------------------------------------------------------------------
void Configuration::PrintConfiguration()
{
  // Hello there!
  std::cout << std::setfill('-') << std::setw(60) << "-" << std::setfill(' ')  << std::endl;
  std::cout << "              Majorana " << majrecoVersion                     << std::endl;
  //std::cout << "       Simulation software for SiPM Wheel           "        << std::endl;
  std::cout                                                                    << std::endl;
  std::cout << "Majorana Configuration:\n"
            << "SimulateOutputPath:        " << fSimulateOutputPath            << std::endl        
            << "RecoAnaTreePath:           " << fRecoOutputPath                << std::endl        
            << "OpReferenceTablePath:      " << fOpReferenceTablePath          << std::endl        
            << "PixelizationPath:          " << fPixelizationPath              << std::endl        
            << "DoPenalized:               " << fDoPenalized                   << std::endl        
            << "PenalizedStopId:           " << fPenalizedStopId               << std::endl        
            << "UnpenalizedStopId:         " << fUnpenalizedStopId             << std::endl        
            << "Gamma:                     " << fGamma                         << std::endl;
  std::cout << std::setfill('-') << std::setw(60) << "-" << std::setfill(' ')  << std::endl;
}  

}


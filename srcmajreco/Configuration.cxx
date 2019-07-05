/**
 * @file Configuration.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton to hold user configuration for simulation 
 *        and reconstruction.
 * @date 07-04-2019
 * 
 */

#include "Configuration.h"

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

Configuration* Configuration::CreateInstance()
{
  if (instance == 0)
  {
    static Configuration config;
    instance = &config;
  }
  return instance;
}

Configuration* Configuration::Instance()
{
  assert(instance);
  return instance;
}

Configuration::Configuration()
{}

Configuration::~Configuration()
{}

void Configuration::Initialize(const std::string& configPath)
{
  fConfigPath = configPath;
  // Save the configuration
  ReadConfigFile();
}

void Configuration::ReadConfigFile()
{
  using boost::property_tree::ptree;
  ptree pt;
  read_ini(fConfigPath.c_str(), pt);

  // Grab the values 
  ptree& modeSect  = pt.get_child("Mode");
  ptree& filesSect = pt.get_child("Files");
  ptree& simSect   = pt.get_child("Simulation");
  ptree& recoSect  = pt.get_child("Reconstruction");

  // Mode
  fSourceMode         = modeSect.get_child("sourceMode").get_value("");
  // Reconstruction
  fReconstruct        = "true" == recoSect.get_child("reconstruct").get_value("") ? true : false;
  fDoPenalized        = "true" == recoSect.get_child("doPenalized").get_value("") ? true : false;
  fPenalizedStopId    = boost::lexical_cast<size_t>(recoSect.get_child("penalizedStopId").get_value(""));
  fUnpenalizedStopId  = boost::lexical_cast<size_t>(recoSect.get_child("unpenalizedStopId").get_value(""));
  fGamma              = boost::lexical_cast<float>(recoSect.get_child("gamma").get_value(""));
  // Files
  fSimulateOutputPath   = std::string(majrecoDir)+"/"+filesSect.get_child("simulateOutputPath").get_value("");
  fSteeringFilePath     = std::string(majrecoDir)+"/"+filesSect.get_child("steeringFilePath").get_value("");
  fDAQFilePath          = std::string(majrecoDir)+"/"+filesSect.get_child("daqFilePath").get_value("");
  if (fReconstruct) fRecoAnaTreePath = std::string(majrecoDir)+"/"+filesSect.get_child("recoAnaTreePath").get_value("");
  if (fReconstruct) fOpReferenceTablePath = std::string(majrecoDir)+"/"+filesSect.get_child("opReferenceTablePath").get_value("");
  if (fShowVis) fVisMacroPath = std::string(majrecoDir)+"/"+filesSect.get_child("visMacroPath").get_value("");
  if (fReconstruct || fSourceMode == "pixel") fPixelizationPath = std::string(majrecoDir)+"/"+filesSect.get_child("pixelizationPath").get_value(""); 
  // Simulation
  // CONVERT TO PROPER UNITS HERE
  fNMPPCs               =    boost::lexical_cast<unsigned>(simSect.get_child("nMPPCs").get_value(""));
  fMPPCHalfLength       = cm*boost::lexical_cast<double>(simSect.get_child("mppcHalfLength").get_value(""));
  fDiskRadius           = cm*boost::lexical_cast<double>(simSect.get_child("diskRadius").get_value(""));
  fDiskThickness        = cm*boost::lexical_cast<double>(simSect.get_child("diskThickness").get_value(""));
  fSourcePeakE          = eV*boost::lexical_cast<double>(simSect.get_child("sourcePeakE").get_value(""));
  fSourcePeakESigma     = eV*boost::lexical_cast<double>(simSect.get_child("sourcePeakESigma").get_value(""));
  fSurfaceRoughness     =    boost::lexical_cast<double>(simSect.get_child("surfaceRoughness").get_value(""));
  fSurfaceAbsorption    =    boost::lexical_cast<double>(simSect.get_child("surfaceAbsorption").get_value(""));
  fSourcePosSigma       = cm*boost::lexical_cast<double>(simSect.get_child("sourcePosSigma").get_value(""));
}

void Configuration::CheckConfiguration()
{
  // Make sure the configuration makes sense
  if (fNMPPCs     <= 0) { std::cerr << "ERROR. Number of MPPCs < 0." << std::endl; exit(1); }
  if (fMPPCHalfLength < 0) { std::cerr << "ERROR. MPPC areas < 0."      << std::endl; exit(1); }
  if (fDiskRadius <= 0) { std::cerr << "ERROR. Disk radius < 0." << std::endl; exit(1); }
  if (fDiskThickness <= 0) { std::cerr << "ERROR. Disk thickness < 0." << std::endl; exit(1); }
  if (fSourceMode != "pixel" &&
      fSourceMode != "point") { std::cerr << "ERROR. Source mode listed as \'" << fSourceMode << "\'." << std::endl; exit(1); }
  if (fSurfaceRoughness < 0 || fSurfaceRoughness > 1) { std::cerr << "ERROR. 0 < Surface roughness < 1." << std::endl; exit(1); }
  if (fSurfaceAbsorption < 0 || fSurfaceAbsorption > 1) { std::cerr << "ERROR. 0 < Surface absorption < 1." << std::endl; exit(1); }
}

void Configuration::PrintConfiguration()
{
  // Hello there!
  std::cout << std::setfill('-') << std::setw(60) << "-" << std::setfill(' ')  << std::endl;
  std::cout << "              Majorana " << majrecoVersion                    << std::endl;
  //std::cout << "       Simulation software for SiPM Wheel           "          << std::endl;
  std::cout                                                                    << std::endl;
  std::cout << "Majorana Configuration:\n";
  std::cout << "SimulateOutputPath " << fSimulateOutputPath << std::endl
            << "SteeringFilePath   " << fSteeringFilePath   << std::endl
            << "SourceMode         " << fSourceMode         << std::endl;
            if (fSourceMode == "pixel" || fReconstruct) std::cout << "PixelizationPath   " << fPixelizationPath << std::endl;
  std::cout << "Reconstruct        "; 
            if (fReconstruct) 
            { 
              std::cout << "true\n"; 
              std::cout << "RecoAnaTreePath    " << fRecoAnaTreePath << std::endl; 
              std::cout << "OpReferenceTablePath " << fOpReferenceTablePath << std::endl;
            }
            else std::cout << "false\n";
  std::cout << "SourcePosSigma     " << fSourcePosSigma/cm     << " cm"  << std::endl
            << "SourcePeakE        " << fSourcePeakE/eV        << " eV"  << std::endl
            << "SourcePeakESigma   " << fSourcePeakESigma/eV   << " eV"  << std::endl
            << "SurfaceRoughness   " << fSurfaceRoughness   << std::endl
            << "SurfaceAbsorption  " << fSurfaceAbsorption  << std::endl
            << "NumberOfMPPCs      " << fNMPPCs             << std::endl
            << "SipmHalfLength     " << fMPPCHalfLength/cm     << " cm" << std::endl
            << "DiskRadius         " << fDiskRadius/cm         << " cm"  << std::endl
            << "DiskThickness      " << fDiskThickness/cm      << " cm"  << std::endl;
  std::cout << std::setfill('-') << std::setw(60) << "-" << std::setfill(' ')  << std::endl;
}  

void Configuration::ReadSteeringFile()
{
  std::ifstream f(fSteeringFilePath.c_str()); 
  if (!f.is_open())
  {
    std::cerr << "Configuration::ReadSteeringFile() Error! Cannot open steering file!\n";
    exit(1);
  }
  std::cout << "\nReading light steering file..." << std::endl;
  
  // We have different modes here:
  //
  //   SteeringFile in pixel mode
  //        pixelID n
  //   SteeringFile in point mode
  //        r theta n or x y n
  if (fSourceMode == "pixel")
  {
    // First read top line 
    std::string string1, string2;
    std::getline(f, string1, ' ');
    std::getline(f, string2);
    if (string1 != "pixelID" || string2 != "n")
    {
      std::cout << "Error! LightSteeringFile in pixel mode must have " 
             << "\'pixelID n\' on the top row.\n"
             << std::endl;
      exit(1);
    }
    // Read the rest of the file
    while (std::getline(f, string1, ' '))
    {
      std::getline(f, string2);
      G4int pixelID = std::stof(string1);
      G4int n       = std::stof(string2);

      SteeringTableIndex s;
      s.pixelID = pixelID;
      s.n       = n;
      fSteeringTable.push_back(s);
    }
    return;
  }

  // We are in point mode
  // First read top line for r,theta mode or x,y mode
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);
  
  bool sourcePosXY;
  if (string1 == "r" && string2 == "theta")  sourcePosXY = false;
  else if (string1 == "x" && string2 == "y") sourcePosXY = true;
  else 
  {
    std::cout << "Error! LightSteeringFile in point mode must have "
           << "\"r theta n\" or \"x y n\" on top row.\n" 
           << std::endl;
    exit(1);
  }
  if (string3 != "n")
  {
    std::cout << "Error! LightSteeringFile in point mode must have "
           << "\"r theta n\" or \"x y n\" on top row.\n" 
           << std::endl;
    exit(1);
  }
   
  // Read the rest of the file
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);
    float value1 = std::stof(string1);
    float value2 = std::stof(string2);
    unsigned n   = std::stoi(string3);

    // We have primaries and some coordinates
    float x(0), y(0), r(0), thetaDeg(0);
    if (sourcePosXY)
    {
      x = value1;
      y = value2;
      r = std::sqrt(x*x + y*y);
    
      if (r > 0.01) thetaDeg = std::asin(std::abs(y/r))*180/pi;
      // Handle theta convention
      if (x <  0 && y >= 0) thetaDeg = 180 - thetaDeg;
      if (x <  0 && y <  0) thetaDeg = 180 + thetaDeg;
      if (x >= 0 && y <  0) thetaDeg = 360 - thetaDeg;
    }
    else 
    {
      r        = value1;
      thetaDeg = value2;
      x = r*std::cos(thetaDeg*pi/180);
      y = r*std::sin(thetaDeg*pi/180);
    }

    SteeringTableIndex s;
    s.r        = r*cm;        
    s.thetaDeg = thetaDeg*deg;
    s.x        = x*cm;
    s.y        = y*cm;
    s.n        = n;
    fSteeringTable.push_back(s);
  }
}

}


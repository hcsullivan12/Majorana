// 
// File: Configuration.cxx
//
// Author: Hunter Sullivan
//
// Discription: Class to initalize configuration.
//

#include "Configuration.h"

#include <iomanip>
#include <math.h>
#include <fstream>
#include <assert.h>

// Preprocessing variables
#ifdef VERSION
#define majoranaVersion VERSION
#endif

#ifdef PROJECTDIR
#define majoranaDir PROJECTDIR
#endif

namespace majorana
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
  // Read json file
  FILE *configFile = fopen(fConfigPath.c_str(), "r");
  char readBuffer[65536];
  rapidjson::FileReadStream jsonStream(configFile, readBuffer, sizeof(readBuffer));

  fJSONDoc.ParseStream(jsonStream);
  fclose(configFile);

  if (fJSONDoc.HasParseError() || !fJSONDoc.IsObject())
  {
    std::cerr << "Error. Failed to parse config file " << fConfigPath << "!" << std::endl;
    std::exit(1);
  }

  // Save the configuration
  ReadJSONFile();
  // Safety checks
  CheckConfiguration();
  // Output to terminal
  PrintConfiguration();
}

void Configuration::ReadJSONFile()
{
  // Specific order 
  // CONVERT TO PROPER UNITS HERE
  // Prepend project directory
  fSimulateOutputPath =    std::string(majoranaDir)+"/"+GetJSONMember("simulateOutputPath", rapidjson::kStringType).GetString();
  fSteeringFilePath   =    std::string(majoranaDir)+"/"+GetJSONMember("steeringFilePath", rapidjson::kStringType).GetString();
  fSourceMode         =    GetJSONMember("sourceMode", rapidjson::kStringType).GetString();

  fNMPPCs             =    GetJSONMember("nMPPCs", rapidjson::kNumberType).GetUint();

  fMPPCHalfLength     =    cm*GetJSONMember("mppcHalfLength", rapidjson::kNumberType).GetDouble();
  fDiskRadius         =    cm*GetJSONMember("diskRadius", rapidjson::kNumberType).GetDouble();
  fDiskThickness      =    cm*GetJSONMember("diskThickness", rapidjson::kNumberType).GetDouble();
  fSourcePeakE        =    eV*GetJSONMember("sourcePeakE", rapidjson::kNumberType).GetDouble(); 
  fSourcePeakESigma   =    eV*GetJSONMember("sourcePeakESigma", rapidjson::kNumberType).GetDouble(); 
  fSurfaceRoughness   =       GetJSONMember("surfaceRoughness", rapidjson::kNumberType).GetDouble(); 
  fSurfaceAbsorption  =       GetJSONMember("surfaceAbsorption", rapidjson::kNumberType).GetDouble();

  fReconstruct        =    GetJSONMember("reconstruct", rapidjson::kFalseType).GetBool(); 

  if (fReconstruct)  
  {
    fRecoAnaTreePath      = std::string(majoranaDir)+"/"+GetJSONMember("recoAnaTreePath", rapidjson::kStringType).GetString();
    fOpReferenceTablePath = std::string(majoranaDir)+"/"+GetJSONMember("opReferenceTablePath", rapidjson::kStringType).GetString();
  }
  if (fShowVis)                               fVisMacroPath     = std::string(majoranaDir)+"/"+GetJSONMember("visMacroPath", rapidjson::kStringType).GetString();
  if (fSourceMode == "point")                 fSourcePosSigma   = cm*GetJSONMember("sourcePosSigma", rapidjson::kNumberType).GetDouble(); 
  if (fSourceMode == "point" && fReconstruct) fPixelizationPath = std::string(majoranaDir)+"/"+GetJSONMember("pixelizationPath", rapidjson::kStringType).GetString();
  if (fSourceMode == "pixel") fPixelizationPath = std::string(majoranaDir)+"/"+GetJSONMember("pixelizationPath", rapidjson::kStringType).GetString(); 
}

const rapidjson::Value& Configuration::GetJSONMember(const std::string&     memberName,
                                                     rapidjson::Type        memberType,
                                                     const unsigned&        arraySize,
                                                     const rapidjson::Type& arrayType)
{
   // Check to see if the document has memberName
   if (!fJSONDoc.HasMember(memberName.c_str())) 
   {
     std::cerr << "ERROR: \"" << memberName << "\" in config file not found!" << std::endl;
     exit(1);
   }

   // Get the value specified for memberName
   rapidjson::Value& member = fJSONDoc[memberName.c_str()];

   // Make sure the types match
   if ( ((memberType       == rapidjson::kTrueType) || (memberType       == rapidjson::kFalseType)) &&
       !((member.GetType() == rapidjson::kTrueType) || (member.GetType() == rapidjson::kFalseType)) ) 
   {
     std::cerr << "ERROR: \"" << memberName << "\" in config file has wrong type!"<< std::endl;
     std::cerr << "Expected " << fJSONTypes.at(rapidjson::kTrueType)
               << " or " << fJSONTypes.at(rapidjson::kFalseType)
               << ", got " << fJSONTypes.at(member.GetType()) << "." << std::endl;
     exit(1);
   }
   // Handle boolean
   if ( (memberType == rapidjson::kTrueType) || (memberType == rapidjson::kFalseType) )
   {
     memberType = member.GetType();
   }

   if (member.GetType() != memberType) 
   {
     std::cerr << "ERROR: \"" << memberName << "\" in run config file has wrong type!"<< std::endl;
     std::cerr << "Expected " << fJSONTypes.at(memberType) << ", got " << fJSONTypes.at(member.GetType())
               << "." << std::endl;
     exit(1);
   }

   if (member.GetType() == rapidjson::kArrayType) 
   {
     if (member.Size() != arraySize) 
     {
       std::cerr << "ERROR: Size mismatch for array \"" << memberName << "\" in config file!" << std::endl;
       std::cerr << "Expected " << arraySize << ", got " << member.Size() << "." << std::endl;
       exit(1);
     }
     for (const auto& value : member.GetArray()) 
     {
       if (value.GetType() != arrayType) 
       {
         std::cerr << "ERROR: Type mismatch in array \"" << memberName << "\" in config file!" << std::endl;
         std::cerr << "Expected " << fJSONTypes.at(arrayType) << ", got "
                   << fJSONTypes.at(value.GetType()) << "." << std::endl;
         exit(1);
       }
     }
   }
   return member;
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
  std::cout << "              Majorana " << majoranaVersion                    << std::endl;
  std::cout << "       Simulation software for SiPM Wheel           "          << std::endl;
  std::cout                                                                    << std::endl;
  std::cout << "Majorana Configuration:\n";
  std::cout << "SimulateOutputPath " << fSimulateOutputPath << std::endl
            << "SteeringFilePath   " << fSteeringFilePath   << std::endl
            << "SourceMode         " << fSourceMode         << std::endl;
            if (fSourceMode == "pixel" || fReconstruct) std::cout << "PixelizationPath " << fPixelizationPath << std::endl;
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


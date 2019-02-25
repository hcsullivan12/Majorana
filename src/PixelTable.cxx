// 
// File: PixelTable.cxx
//
// Author: Hunter Sullivan
//
// Discription: 
//

#include "PixelTable.h"

#include <fstream>
#include <assert.h>

#include "TMath.h"

#include "G4ios.hh"

namespace majorana 
{

PixelTable* PixelTable::instance = 0;

PixelTable* PixelTable::CreateInstance()
{
  if (instance == 0)
  {
    static PixelTable pixelTable;
    instance = &pixelTable;
  }
  return instance;
}

PixelTable* PixelTable::Instance()
{
  assert(instance);
  return instance;
}

PixelTable::PixelTable()
{
  fPixelList.clear();
}

Pixel* PixelTable::GetPixel(const unsigned& id) 
{
  auto it = std::find_if(fPixelList.begin(), fPixelList.end(), [id](const Pixel& pixel){ return pixel.ID() == id; }); 
  if (it == fPixelList.end())
  {
    G4cout << "Pixel::GetPixel() Error. Pixel #" << id << " not initialized!" << G4endl;
  }
  return &*it;
}

void PixelTable::LoadReferenceTable(const std::string& path)
{
  // Make sure pixels have been initialized
  if (fPixelList.size() == 0)
  {
    G4cerr << "Error! Pixels have not been initialized!\n" << G4endl;
    exit(1);
  }

  // Read in reference table
  std::ifstream f(path.c_str());
  if (!f.is_open())
  { 
    G4cerr << "PixelTable::LoadReferenceTable() Error! Cannot open reference table file!\n";
    exit(1);
  }
  G4cout << "Reading reference table file..." << G4endl;
 
  // Table must be:
  //
  //    pixelID mppcID probability
  //
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);

  if (string1 != "pixelID" || string2 != "mppcID" || string3 != "probability")
  { 
    G4cout << "Error! ReferenceTableFile must have "
           << "\'pixelID mppcID probability\' on the top row.\n"
           << G4endl;
    exit(1);
  } 
  
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);
    unsigned mppcID  = std::stof(string2);
    float    prob    = std::stof(string3);

    Pixel* pixel = GetPixel(pixelID);
    pixel->AddReference(mppcID, prob); 
  }
  f.close();
}

void PixelTable::Initialize(const std::string& pixelizationPath)
{
  // Make pixels for each position
  std::ifstream f(pixelizationPath.c_str());
  if (!f.is_open())
  { 
    G4cerr << "PixelTable::Initialize() Error! Cannot open pixelization file!\n";
    exit(1);
  }
  G4cout << "Reading pixelization file..." << G4endl;

  // Table must be:
  //
  //   pixelID x y
  // 

  // First read top line 
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);
  if (string1 != "pixelID" || string2 != "x" || string3 != "y")
  { 
    G4cout << "Error! PixelizationFile must have "
           << "\'pixelID x y\' on the top row.\n"
           << G4endl;
    exit(1);
  }

  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);
    float    x       = std::stof(string2);
    float    y       = std::stof(string3);
        
    // Get r and theta just in case we need it
    float r     = std::sqrt(x*x + y*y);
    float thetaDeg(0);
    if (r > 0.01) thetaDeg = TMath::ASin(std::abs(y/r))*180/TMath::Pi();
    // Handle theta convention
    if (x <  0 && y >= 0) thetaDeg = 180 - thetaDeg;
    if (x <  0 && y <  0) thetaDeg = 180 + thetaDeg;
    if (x >= 0 && y <  0) thetaDeg = 360 - thetaDeg; 
 
    Pixel v(pixelID, x, y, r, thetaDeg);
    fPixelList.emplace_back(v);
  }
  f.close();

  // Compute size 
  float min = std::numeric_limits<float>::max(); 
  if (fPixelList.size() > 0)
  {
    float x1  = fPixelList.front().X();
    for (const auto& v : fPixelList)
    {
      float diff = std::abs(x1 - v.X());
      if (diff < min && diff > 0) min = diff;
    }
    for (auto& v : fPixelList) v.SetSize(min);
  }

  G4cout << "Initialized " << fPixelList.size() << " " << min << "x" << min << "cm2 pixels..." << G4endl;
}

PixelTable::~PixelTable()
{}

}

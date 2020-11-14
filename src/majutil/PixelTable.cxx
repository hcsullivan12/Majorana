/**
 * @file PixelTable.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton table for holding the pixelization scheme.
 * @date 07-04-2019
 * 
 */

#include "majutil/PixelTable.h"

#include <fstream>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <math.h>

namespace majutil 
{

PixelTable* PixelTable::instance = 0;

//------------------------------------------------------------------------
PixelTable* PixelTable::CreateInstance()
{
  if (instance == 0)
  {
    static PixelTable pixelTable;
    instance = &pixelTable;
  }
  return instance;
}

//------------------------------------------------------------------------
PixelTable* PixelTable::Instance()
{
  assert(instance);
  return instance;
}

//------------------------------------------------------------------------
PixelTable::PixelTable()
{
  fPixelVec = std::make_shared<std::vector<Pixel>>();
  fPixelVec->clear();

  fSpacing = 0;
}

//------------------------------------------------------------------------
PixelTable::~PixelTable()
{}

//------------------------------------------------------------------------
Pixel* PixelTable::GetPixel(const unsigned& id) 
{
  auto it = std::find_if(fPixelVec->begin(), fPixelVec->end(), [id](const Pixel& pixel){ return pixel.ID() == id; }); 
  if (it == fPixelVec->end())
  {
    it = it--;
    std::cout << "Pixel::GetPixel() WARNING... "
              << "Pixel #" << id << " not initialized! "
              << "Returning Pixel #" << it->ID() << std::endl;
  }
  return &*it;
}

//------------------------------------------------------------------------
void PixelTable::LoadReferenceTable(const std::string& path)
{
  // Make sure pixels have been initialized
  assert(fPixelVec->size() != 0 && "Pixels have not been initialized!");

  // Read in reference table
  std::ifstream f(path.c_str());
  if (!f.is_open())
  { 
    std::cout << "PixelTable::LoadReferenceTable() Error! Cannot open reference table file!\n";
    exit(1);
  }
  std::cout << "Reading reference table file...\n";
 
  // Table must be:
  //
  //    pixelID mppcID probability
  //
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);

  if (string1 != "pixelID" || 
      string2 != "mppcID"  || 
      string3 != "probability")
  { 
    std::cout << "PixelTable::LoadReferenceTable() Error! ReferenceTable must have "
              << "\'pixelID mppcID probability\' as header.\n";
    exit(1);
  } 
  
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);
    unsigned mppcID  = std::stof(string2);
    float    prob    = std::stof(string3);

    // This assumes the pixels have been ordered
    (*fPixelVec)[pixelID-1].AddReference(mppcID, prob);
  }
  f.close();
}

//------------------------------------------------------------------------
void PixelTable::Initialize(const std::string& pixelizationPath)
{
  // Make pixels for each position
  std::ifstream f(pixelizationPath.c_str());
  if (!f.is_open())
  { 
    std::cout << "PixelTable::Initialize() Error! Cannot open pixelization file!\n";
    exit(1);
  }
  
  std::cout << "Reading pixelization file...\n";

  // Table must be:
  //
  //   pixelID x y
  // 

  // First read top line 
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);
  if(string1 != "pixelID" || 
     string2 != "x"       || 
     string3 != "y")
  {
    std::cout << "PixelTable::Initialize() Error! ReferenceTable must have "
              << "\'pixelID mppcID probability\' as header.\n";
    exit(1);
  }

  // For computing the size
  unsigned thePixelCount(0);
  float aPixelPos(0);
  float min = std::numeric_limits<float>::max(); 
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);

    float    x=stof(string2);       ;

    if(fRadius>16)
        x=stof(string2)*cos(3.14);
    else
        x=stof(string2);

    float    y       = std::stof(string3);

     thePixelCount++;
    if (thePixelCount == 1) aPixelPos = x; 
    else min = std::abs(aPixelPos-x) < min && std::abs(aPixelPos-x) > 0 ? std::abs(aPixelPos-x) : min;
        
    // Get r and theta just in case we need it
    float r     = std::sqrt(x*x + y*y);
    float thetaDeg(0);
    if (r > 0.01) thetaDeg = std::asin(std::abs(y/r))*180/M_PI;
    // Handle theta convention
    if (x <  0 && y >= 0) thetaDeg = 180 - thetaDeg;
    if (x <  0 && y <  0) thetaDeg = 180 + thetaDeg;
    if (x >= 0 && y <  0) thetaDeg = 360 - thetaDeg; 
 
    fPixelVec->emplace_back(pixelID, x, y, r, thetaDeg);
  }
  for (auto& p : *fPixelVec) p.SetSize(min);
  f.close();

  // Sort 
  std::sort( (*fPixelVec).begin(), (*fPixelVec).end(), [](const auto& left, const auto& right) { return left.ID() < right.ID(); } );

  std::cout << "Initialized " << fPixelVec->size() << " " << min << "x" << min << "cm2 pixels...\n";
  fSpacing = min;
}

}

// 
// File: VoxelTable.cxx
//
// Author: Hunter Sullivan
//
// Discription: 
//

#include "VoxelTable.h"

#include <fstream>

#include "TMath.h"

#include "G4ios.hh"

namespace majorana 
{

VoxelTable* VoxelTable::instance = 0;

VoxelTable* VoxelTable::Instance()
{
  if (instance == 0)
  {
    static VoxelTable voxelTable;
    instance = &voxelTable;
  }
  return instance;
}

VoxelTable::VoxelTable()
{
  m_voxelList.clear();
}

Voxel* VoxelTable::GetVoxel(const unsigned& id) 
{
  auto it = std::find_if(m_voxelList.begin(), m_voxelList.end(), [id](const Voxel& voxel){ return voxel.ID() == id; }); 
  if (it == m_voxelList.end())
  {
    G4cout << "Voxel::GetVoxel() Error. Voxel #" << id << " not initialized!" << G4endl;
  }
  return &*it;
}

void VoxelTable::LoadReferenceTable(const std::string& path)
{
  // Make sure voxels have been initialized
  if (m_voxelList.size() == 0)
  {
    G4cerr << "Error! Voxels have not been initialized!\n" << G4endl;
    exit(1);
  }

  // Read in reference table
  std::ifstream f(path.c_str());
  if (!f.is_open())
  { 
    G4cerr << "VoxelTable::LoadReferenceTable() Error! Cannot open reference table file!\n";
    exit(1);
  }
  G4cout << "Reading reference table file..." << G4endl;
 
  // Table must be:
  //
  //    voxelID mppcID probability
  //
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);

  if (string1 != "voxelID" || string2 != "mppcID" || string3 != "probability")
  { 
    G4cout << "Error! ReferenceTableFile must have "
           << "\'voxelID mppcID probability\' on the top row.\n"
           << G4endl;
    exit(1);
  } 
  
  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned voxelID = std::stoi(string1);
    unsigned mppcID  = std::stof(string2);
    float    prob    = std::stof(string3);

    Voxel* voxel = GetVoxel(voxelID);
    voxel->AddReference(mppcID, prob); 
  }
  f.close();
}

void VoxelTable::Initialize(const std::string& voxelizationPath)
{
  // Make voxels for each position
  std::ifstream f(voxelizationPath.c_str());
  if (!f.is_open())
  { 
    G4cerr << "VoxelTable::Initialize() Error! Cannot open voxelization file!\n";
    exit(1);
  }
  G4cout << "Reading voxelization file..." << G4endl;

  // Table must be:
  //
  //   voxelID x y
  // 

  // First read top line 
  std::string string1, string2, string3;
  std::getline(f, string1, ' ');
  std::getline(f, string2, ' ');
  std::getline(f, string3);
  if (string1 != "voxelID" || string2 != "x" || string3 != "y")
  { 
    G4cout << "Error! VoxelizationFile must have "
           << "\'voxelID x y\' on the top row.\n"
           << G4endl;
    exit(1);
  }

  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned voxelID = std::stoi(string1);
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
 
    Voxel v(voxelID, x, y, r, thetaDeg);
    m_voxelList.emplace_back(v);
  }
  f.close();

  // Compute size 
  float min = std::numeric_limits<float>::max(); 
  if (m_voxelList.size() > 0)
  {
    float x1  = m_voxelList.front().X();
    for (const auto& v : m_voxelList)
    {
      float diff = std::abs(x1 - v.X());
      if (diff < min && diff > 0) min = diff;
    }
    for (auto& v : m_voxelList) v.SetSize(min);
  }

  G4cout << "Initialized " << m_voxelList.size() << " " << min << "x" << min << "cm2 voxels..." << G4endl;
}

VoxelTable::~VoxelTable()
{}

}

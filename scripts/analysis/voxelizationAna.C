#define voxelizationAna_cxx
#include "voxelizationAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void ReadVoxelFile();
void Ana();
unsigned FindVoxelID(const float&, const float&);

std::string voxelizationPath = "/home/hunter/projects/Majorana/config/voxels/voxelization.txt";
class Voxel;
std::list<Voxel> voxelList;
// <voxelID, <sipmID, LY>>
using VoxelIDToSiPMMap = std::map<unsigned, std::map<unsigned, double>>;
VoxelIDToSiPMMap moneyMap;
unsigned nPrim(1);

class Voxel 
{

public:
	Voxel(const unsigned& id,
        const float& x, 
        const float& y, 
        const float& r, 
        const float& theta);
	~Voxel();
		
  using OpReferenceTable = std::map<unsigned, float>;

	float       X()     const { return m_x; };
	float       Y()     const { return m_y; };
  float       R()     const { return m_r; };
  float       Theta() const { return m_theta; };
  float       Size()  const { return m_size; };
  unsigned    ID()    const { return m_id; };
  unsigned    NPrimaries() const { return m_nPrimaries; };
  float       Intensity() const { return m_intensity; };

  void SetSize(const float& s) { m_size = s; };
  void SetIntensity(const float& i) { m_intensity = i; };
  void AddReference(const unsigned& mppcID, 
                    const float& prob) { m_referenceTable.emplace(mppcID, prob); };

private:
    
  float  m_x;      ///< x position that this voxel is centered on
  float  m_y;      ///< y position that this voxel is centered on
  float  m_r;      ///< radius from center for this voxel
  float  m_theta;  ///< angle with respect to sipm 1 (in degrees)
  float  m_size;   ///< size of voxel 
  float  m_intensity;    ///< if reconstructing, this is the reconstructed intensity
  unsigned m_id;         ///< id number
  unsigned m_nPrimaries; ///< number of primaries to launch from this voxel
  OpReferenceTable m_referenceTable; ///< stores mppc to probability map
};

Voxel::Voxel(const unsigned& voxelID,
             const float&    x, 
             const float&    y, 
             const float&    r, 
             const float&    theta)
 : m_id(voxelID),
   m_x(x), 
   m_y(y), 
   m_r(r), 
   m_theta(theta)
{
  m_referenceTable.clear();
}

Voxel::~Voxel()
{}

void voxelizationAna::Loop()
{
  // First we have to make the voxelization map
  ReadVoxelFile();
  // Initialize map
  std::cout << "Initializing map...\n";
  for (const auto& v : voxelList)
  {
    std::map<unsigned, double> sipmLYMap;
    for (unsigned s = 1; s <= 16; s++) sipmLYMap.emplace(s, 0);
    moneyMap.emplace(v.ID(), sipmLYMap);
  }

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;    

    // Now we need to compute the running average of LY for each voxel
    // What's the voxelID?
    nPrim = nPrimaries;
    float x = sourcePosXYZ[0];
    float y = sourcePosXYZ[1];
    unsigned voxelID = FindVoxelID(x, y);
    // Store in map
    VoxelIDToSiPMMap::iterator thisVoxel = moneyMap.find(voxelID); 
    for (unsigned s = 1; s <= 16; s++)
    {
      std::map<unsigned, double>::iterator thisSiPM = thisVoxel->second.find(s);
      thisSiPM->second = thisSiPM->second + mppcToLY[s-1];
    }
  }
  Ana();
}

unsigned FindVoxelID(const float& x, const float& y)
{
  for (const auto& v : voxelList)
  {
    if (v.X() == x && v.Y() == y) return v.ID();
  }
  std::cout << "COULDN'T FIND ID!!\n";
  return 1;
}

void ReadVoxelFile()
{
  // Make voxels for each position
  std::ifstream f(voxelizationPath.c_str());
  if (!f.is_open())
  { 
    std::cerr << "VoxelTable::Initialize() Error! Cannot open voxelization file!\n";
    exit(1);
  }
  std::cout << "Reading voxelization file..." << std::endl;

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
    std::cout << "Error! VoxelizationFile must have "
           << "\'voxelID x y\' on the top row.\n"
           << std::endl;
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
    voxelList.emplace_back(v);
  }
  f.close();
}

void Ana()
{
  // Divide by number of events to get average
  for (auto& v : moneyMap)
  {
    for (auto& s : v.second) s.second = s.second/(50.0*nPrim);
    float sum(0);
    //for (const auto& s : v.second) sum = sum + s.second;
    //for (auto& s : v.second) s.second = s.second/sum;
  }

  // Write to file
  // voxelID mppcID prob
  std::string name = "opReferenceTable.txt";
  std::ofstream f(name.c_str());
  f << "voxelID mppcID probability" << std::endl;

  for (const auto& v : moneyMap)
  {
    for (const auto& s : v.second)
    { 
      f << v.first << " " << s.first << " " << s.second << std::endl;
    }
  }
  f.close();
}

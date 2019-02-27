#define makeOpRefTable_cxx
#include "makeOpRefTable.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void ReadPixelFile();
void HandleMap();
unsigned FindPixelID(const double&, const double&);

std::string pixelizationPath = "/home/hunter/projects/Majorana/config/pixelization.txt";
class Pixel;
std::list<Pixel> pixelList;
// <pixelID, <sipmID, LY>>
using PixelIDToSiPMMap = std::map<unsigned, std::map<unsigned, std::vector<double> >>;
PixelIDToSiPMMap moneyMap;

unsigned nPrim(1);
unsigned nSiPMs(128); // should be the total simulated
unsigned nEvents(25);
Int_t factor(4); // used to switch nSiPMs, nSiPMs --> nSiPMs / 2^(factor)
unsigned modNSiPMs = nSiPMs/TMath::Power(2, factor);

std::vector<double> vecX, vecY;

class Pixel 
{

public:
	Pixel(const unsigned& id,
        const double& x, 
        const double& y, 
        const double& r, 
        const double& theta);
	~Pixel();
		
  using OpReferenceTable = std::map<unsigned, double>;

	double       X()     const { return m_x; };
	double       Y()     const { return m_y; };
  double       R()     const { return m_r; };
  double       Theta() const { return m_theta; };
  double       Size()  const { return m_size; };
  unsigned    ID()    const { return m_id; };
  unsigned    NPrimaries() const { return m_nPrimaries; };
  double       Intensity() const { return m_intensity; };

  void SetSize(const double& s) { m_size = s; };
  void SetIntensity(const double& i) { m_intensity = i; };
  void AddReference(const unsigned& mppcID, 
                    const double& prob) { m_referenceTable.emplace(mppcID, prob); };

private:
    
  double  m_x;      ///< x position that this pixel is centered on
  double  m_y;      ///< y position that this pixel is centered on
  double  m_r;      ///< radius from center for this pixel
  double  m_theta;  ///< angle with respect to sipm 1 (in degrees)
  double  m_size;   ///< size of pixel 
  double  m_intensity;    ///< if reconstructing, this is the reconstructed intensity
  unsigned m_id;         ///< id number
  unsigned m_nPrimaries; ///< number of primaries to launch from this pixel
  OpReferenceTable m_referenceTable; ///< stores mppc to probability map
};

Pixel::Pixel(const unsigned& pixelID,
             const double&    x, 
             const double&    y, 
             const double&    r, 
             const double&    theta)
 : m_id(pixelID),
   m_x(x), 
   m_y(y), 
   m_r(r), 
   m_theta(theta)
{
  m_referenceTable.clear();
}

Pixel::~Pixel()
{}

/*******************************************
 * Main Loop
 * *****************************************/
void makeOpRefTable::Loop()
{
  // First we have to make the pixelization map
  ReadPixelFile();
  // Initialize map
  std::cout << "Initializing map...\n";
  for (const auto& v : pixelList)
  {
    std::map<unsigned, std::vector<double>> sipmLYMap;
    for (unsigned s = 1; s <= modNSiPMs; s++) sipmLYMap.emplace(s, std::vector<double>(0,0));
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

    // What's the pixelID?
    nPrim = nPrimaries;
    double x = sourcePosXYZ[0];
    double y = sourcePosXYZ[1];
    unsigned pixelID = FindPixelID(x, y);

    // Store in map
    PixelIDToSiPMMap::iterator thisPixel = moneyMap.find(pixelID); 
    if (thisPixel == moneyMap.end()) {std::cout << "Couldn't find pixelID!\n"; exit(1);}

    unsigned s(1);    // counter for nSiPMs
    unsigned modS(1); // counter for modNSiPMs
    unsigned inc(nSiPMs/modNSiPMs);
    while (s <= nSiPMs)
    { 
      std::map<unsigned, std::vector<double> >::iterator thisSiPM = thisPixel->second.find(modS);
      if (thisSiPM == thisPixel->second.end()) {std::cout << "Couldn't find sipm!\n"; exit(1);}

      thisSiPM->second.push_back(mppcToLY[s-1]);

      modS++;
      s = s + inc;
    }
  }

  HandleMap();
}

unsigned FindPixelID(const double& x, const double& y)
{
  for (const auto& v : pixelList)
  {
    if (v.X() == x && v.Y() == y) return v.ID();
  }
  std::cout << "COULDN'T FIND ID!!\n";
  exit(1);
}

/******************************
 * Read pixelization scheme
 ******************************/
void ReadPixelFile()
{
  // Make pixels for each position
  std::ifstream f(pixelizationPath.c_str());
  if (!f.is_open())
  { 
    std::cerr << "PixelTable::Initialize() Error! Cannot open pixelization file!\n";
    exit(1);
  }
  std::cout << "Reading pixelization file..." << std::endl;

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
    std::cout << "Error! PixelizationFile must have "
           << "\'pixelID x y\' on the top row.\n"
           << std::endl;
    exit(1);
  }

  while (std::getline(f, string1, ' '))
  {
    std::getline(f, string2, ' ');
    std::getline(f, string3);

    unsigned pixelID = std::stoi(string1);
    double   x       = std::stof(string2);
    double   y       = std::stof(string3);
        
    // Get r and theta just in case we need it
    double r     = std::sqrt(x*x + y*y);
    double thetaDeg(0);
    if (r > 0.01) thetaDeg = TMath::ASin(std::abs(y/r))*180/TMath::Pi();
    // Handle theta convention
    if (x <  0 && y >= 0) thetaDeg = 180 - thetaDeg;
    if (x <  0 && y <  0) thetaDeg = 180 + thetaDeg;
    if (x >= 0 && y <  0) thetaDeg = 360 - thetaDeg; 
 
    Pixel v(pixelID, x, y, r, thetaDeg);
    //std::cout << "pixelID = " << pixelID << "  x = " << x << "  y = " << y << "  r = " << r << "  theta = " << thetaDeg << std::endl;
    pixelList.emplace_back(v);
  }
  f.close();
}

void HandleMap()
{ 
  // Compute the average
  for (auto& v : moneyMap)
  {
    for (auto& s : v.second)
    {
      double sum(0);
      unsigned n = s.second.size();
      assert(n == nEvents);

      for (const auto& ly : s.second) sum = sum+ly;
      s.second.clear();
      s.second.resize(1);
      s.second[0] = sum/n; // average
      s.second[0] = s.second[0]/nPrim; // probability
    }
  }

  // Write to file
  // pixelID mppcID prob
  std::string name = "opReferenceTable.txt";
  std::ofstream f(name.c_str());
  f << "pixelID mppcID probability" << std::endl;

  std::vector<double> vec;
  for (const auto& v : moneyMap)
  {
    for (const auto& s : v.second)
    { 
      f << v.first << " " << s.first << " " << s.second[0] << std::endl;
    }
  }
  f.close();
}

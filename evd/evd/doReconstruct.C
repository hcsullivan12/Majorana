
#include "Reconstructor.h"
R__LOAD_LIBRARY(evd/libReconstructor.so)
#include "Pixel.h"

// Some useful variables
std::string thePixelPath;
std::string theOpRefPath;
std::string theDataPath;
double      theDiskRadius;
std::shared_ptr<std::vector<Pixel>> thePixelVec;

// Protos
void LoadPixelization();
void LoadOpRefTable();
void Reconstruct();
const std::map<unsigned, unsigned> ReadDataFile();

/********************************/
void doReconstruct(std::string pixelizationPath, 
                   std::string opRefTablePath, 
                   std::string datapath,
                   double diskRadius)
{
  thePixelPath = pixelizationPath;
  theOpRefPath = opRefTablePath;
  theDataPath  = datapath;
  theDiskRadius = diskRadius;

  thePixelVec = std::make_shared<std::vector<Pixel>>();
  thePixelVec->clear();

  // Load pixelization
  LoadPixelization();
  // Load opRef
  LoadOpRefTable();
  // Reconstruct
  Reconstruct();
}

/********************************/
void LoadPixelization()
{
  // Make pixels for each position
  std::ifstream f(thePixelPath.c_str());
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
    float    x       = std::stof(string2);
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
 
    thePixelVec->emplace_back(pixelID, x, y, r, thetaDeg);
  }
  for (auto& p : *thePixelVec) p.SetSize(min);
  f.close();

  // Sort 
  std::sort( (*thePixelVec).begin(), (*thePixelVec).end(), [](const Pixel& left, const Pixel& right) { return left.ID() < right.ID(); } );

  std::cout << "Initialized " << thePixelVec->size() << " " << min << "x" << min << "cm2 pixels...\n";
}

/********************************/
void LoadOpRefTable()
{
  // Make sure pixels have been initialized
  assert(thePixelVec->size() != 0 && "Pixels have not been initialized!");

  // Read in reference table
  std::ifstream f(theOpRefPath.c_str());
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
    (*thePixelVec)[pixelID-1].AddReference(mppcID, prob);
  }
  f.close();
}

/****************************************/
const std::map<unsigned, unsigned> ReadDataFile() 
{
  // The file should contain the number of photons detected by the sipms
  std::ifstream theFile(theDataPath.c_str());
  std::string line;
  if (theFile.is_open()) {
    // we only care about the last line
    while(std::getline(theFile, line)) {};
  }

  std::map<unsigned, unsigned> v;
  size_t pos = 0;
  size_t counter = 1;
  std::string delimiter = " ";
  while ((pos = line.find(delimiter)) != std::string::npos) {
    v.emplace(counter, std::stoi(line.substr(0, pos)));
    line.erase(0, pos + delimiter.length());
    counter++;
  }
  if (line.size() > 1) v.emplace(counter, std::stoi(line));
  return v;
}

/********************************/
void Reconstruct()
{
  // Read from DAQ file
  auto mydata = ReadDataFile();
  //for (auto& d : mydata) std::cout << d.first << " " << d.second << std::endl;

  // Initialize the reconstructor
  Reconstructor theReconstructor;
  theReconstructor.Initialize(mydata, thePixelVec, theDiskRadius);
  theReconstructor.Reconstruct();
  theReconstructor.MakePlots("recoanatree.root");
  cout << "Finished!" << endl;
}
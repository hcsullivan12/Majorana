#include <iostream>
#include <fstream>

unsigned nVoxels    = 225; 
// With voxelsX odd, we can add the origin
unsigned voxelsX    = 29; //std::sqrt(nVoxels); 
unsigned voxelsY    = std::sqrt(nVoxels);
float    diskRadius = 14.5;
float    inc        = 1;
unsigned nEvents    = 50;

void makeLightSteeringXAxis()
{
  std::string filename = "../config/LightSourceSteering.txt";
  std::ofstream file(filename.c_str());
  if (!file.is_open()) return;

  // Make a vector of xAxis voxels, and then translate
  std::vector<std::pair<float, float>> xAxisVoxelPos;
  // Start at edge
  float x(-diskRadius + inc/2.0), y(0);
  for (unsigned v = 1; v <= voxelsX; v++)
  {
    std::cout << x << " " << y << std::endl;
    std::pair<float, float> p = std::make_pair(x, y);
    xAxisVoxelPos.push_back(p);
    x = x + inc;
  }

  // output top line
  file << "x y n" << std::endl; 

  // Now shift
  unsigned voxelID(0);
  y = 0;
    for (const auto p : xAxisVoxelPos)
    {
      float r = std::sqrt(p.first*p.first + (p.second+y)*(p.second+y));
      if (r >= diskRadius) continue;
      unsigned event = 1;
      while (event <= nEvents)
      {
        if (p.first <  0) file << p.first << " 0 50000" << std::endl;
        if (p.first >= 0) file << p.first << " 0 50000"   << std::endl;
 
        event++;
      }
      voxelID++;
    }

  /*  

  float r(1);
  // 90 degrees
  while (r < diskRadius)
  {
    for (int i = 1; i <= n; i++)
    {
      file << r << " " << "90\n";  
    }
    r = r + rInc;
  }
  r = 1;
  // 270 degrees
  while (r < diskRadius)
  {
    for (int i = 1; i <= n; i++)
    {
      file << r << " " << "270\n";  
    }
  r = r + rInc;
  }
*/
  file.close();
}


/*// 180 degrees
  for (int r = 0; r <= 14; r++)
  {
    for (int n = 1; n <= 10; n++)
    {
      file << r << " " << "180\n";  
    }
  }
  // 0 degrees
  for (int r = 1; r <= 14; r++)
  {
    for (int n = 1; n <= 10; n++)
    {
      file << r << " " << "0\n";  
    }
  }*/


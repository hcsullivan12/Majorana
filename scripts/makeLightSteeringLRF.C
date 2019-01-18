#include <iostream>
#include <fstream>

float    diskRadius = 14.5;
double    inc        = 0.1;
unsigned nEvents    = 50;
unsigned nPrim      = 50000;

TGraph* g = new TGraph(665);

void makeLightSteeringLRF()
{
  std::string lss = "../config/LightSourceSteering.txt";
  std::ofstream lssFile(lss.c_str());
  if (!lssFile.is_open()) return;

  std::string v = "../config/voxelization.txt";
  std::ofstream vFile(v.c_str());
  if (!vFile.is_open()) return;


  // Make a vector of xAxis voxels, and then translate
  std::vector<std::pair<float, float>> xAxisVoxelPos;
  // Start at edge
  double x(0), y(0);
  while (x < diskRadius)
  {
    //std::cout << x << " " << y << std::endl;
    std::pair<float, float> p = std::make_pair(x, y);
    xAxisVoxelPos.push_back(p);
    x = x + inc;
  }
  x = 0 - inc;
  y = 0;
  while (x > -diskRadius)
  {
    //std::cout << x << " " << y << std::endl;
    std::pair<float, float> p = std::make_pair(x, y);
    xAxisVoxelPos.push_back(p);
    x = x - inc;
  }

  // output top line
  lssFile << "voxelID n" << std::endl; 
  vFile   << "voxelID x y" << std::endl;

  // Now shift
  unsigned voxelID(1);
  y = 0;
//  while (y < diskRadius)
  {
    for (const auto p : xAxisVoxelPos)
    {
      float thisX = p.first;
      float thisY = p.second + y;
      float r = std::sqrt(thisX*thisX + thisY*thisY);
      if (r >= diskRadius) continue;
      g->SetPoint(voxelID, thisX, thisY);
      vFile   << voxelID << " " << thisX << " " << thisY << std::endl;

      unsigned event = 1;
      while (event <= nEvents)
      {
        lssFile << voxelID << " " << nPrim << std::endl;
        event++;
      }
      voxelID++;
    }
    y = y + inc;
  }
  
  y = 0 + inc;
  /*while (y < diskRadius)
  {
    for (const auto p : xAxisVoxelPos)
    {
      float thisX = p.first;
      float thisY = p.second - y;
      float r = std::sqrt(thisX*thisX + thisY*thisY);
      if (r >= diskRadius) continue;
      g->SetPoint(voxelID, thisX, thisY);
      vFile   << voxelID << " " << thisX << " " << thisY << std::endl;

      unsigned event = 1;
      while (event <= nEvents)
      {
        lssFile << voxelID << " " << nPrim << std::endl;
        event++;
      }
      voxelID++;
    }
    y = y + inc;
  }
  std::cout << voxelID << std::endl;*/

  lssFile.close();
  vFile.close();

  g->SetMarkerStyle(21);
  g->SetMarkerSize(2);
  g->Draw("ap");
}

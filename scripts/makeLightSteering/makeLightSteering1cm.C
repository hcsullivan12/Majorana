#include <iostream>
#include <fstream>

float    diskRadius = 14.5;
float    inc        = 1.0;
unsigned nEvents    = 10;
unsigned nPrim      = 50000;

TGraph* g = new TGraph(665);

void makeLightSteering1cm()
{
  std::string lss = "LightSourceSteering.txt";
  std::ofstream lssFile(lss.c_str());
  if (!lssFile.is_open()) return;

  std::string v = "pixelization.txt";
  std::ofstream vFile(v.c_str());
  if (!vFile.is_open()) return;


  // Make a vector of xAxis pixels, and then translate
  std::vector<std::pair<float, float>> xAxisPixelPos;
  // Start at edge
  float x(0), y(0);
  while (x < diskRadius)
  {
    //std::cout << x << " " << y << std::endl;
    std::pair<float, float> p = std::make_pair(x, y);
    xAxisPixelPos.push_back(p);
    x = x + inc;
  }
  x = 0 - inc;
  y = 0;
  while (x > -diskRadius)
  {
    //std::cout << x << " " << y << std::endl;
    std::pair<float, float> p = std::make_pair(x, y);
    xAxisPixelPos.push_back(p);
    x = x - inc;
  }

  // output top line
  lssFile << "pixelID n" << std::endl; 
  vFile   << "pixelID x y" << std::endl;

  // Now shift
  unsigned pixelID(1);
  y = 0;
  while (y < diskRadius)
  {
    for (const auto p : xAxisPixelPos)
    {
      float thisX = p.first;
      float thisY = p.second + y;
      float r = std::sqrt(thisX*thisX + thisY*thisY);
      if (r >= diskRadius) continue;
      g->SetPoint(pixelID, thisX, thisY);
      vFile   << pixelID << " " << thisX << " " << thisY << std::endl;

      unsigned event = 1;
      while (event <= nEvents)
      {
        lssFile << pixelID << " " << nPrim << std::endl;
        event++;
      }
      pixelID++;
    }
    y = y + inc;
  }
  
  y = 0 + inc;
  while (y < diskRadius)
  {
    for (const auto p : xAxisPixelPos)
    {
      float thisX = p.first;
      float thisY = p.second - y;
      float r = std::sqrt(thisX*thisX + thisY*thisY);
      if (r >= diskRadius) continue;
      g->SetPoint(pixelID, thisX, thisY);
      vFile   << pixelID << " " << thisX << " " << thisY << std::endl;

      unsigned event = 1;
      while (event <= nEvents)
      {
        lssFile << pixelID << " " << nPrim << std::endl;
        event++;
      }
      pixelID++;
    }
    y = y + inc;
  }
  std::cout << pixelID << std::endl;

  lssFile.close();
  vFile.close();

  g->SetMarkerStyle(21);
  g->SetMarkerSize(2);
  g->Draw("ap");
}

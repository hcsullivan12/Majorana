#include <iostream>
#include <fstream>

float  diskRadius = 14.5;
double inc        = 0.1;
double r          = 7.0; 
unsigned nEvents    = 50;
unsigned nPrim      = 50000;

TGraph* g = new TGraph(81);

void makeLightSteeringTheta()
{
  std::string lss = "../config/LightSourceSteering.txt";
  std::ofstream lssFile(lss.c_str());
  if (!lssFile.is_open()) return;

  std::string v = "../config/voxelization.txt";
  std::ofstream vFile(v.c_str());
  if (!vFile.is_open()) return;
 
  // output top line
  lssFile << "voxelID n" << std::endl; 
  vFile   << "voxelID x y" << std::endl;

  // Do the alpha = 0 first
  unsigned voxelID(1);
  vFile << voxelID << " " << diskRadius - r << " " << 0 << std::endl;
  for (unsigned e = 1; e <= nEvents; e++)
  {
    lssFile << voxelID << " " << nPrim << std::endl; 
  }
  g->SetPoint(voxelID-1, diskRadius-r, 0);
  voxelID++;

  // Starting point
  double alpha = 40;
  while (alpha > 0)
  {
    // Compute x and y
    double x = diskRadius - r*std::cos(alpha*TMath::Pi()/180);
    double y = r*std::sin(alpha*TMath::Pi()/180);
    g->SetPoint(voxelID-1, x, y);
    if (std::sqrt(x*x+y*y) >= diskRadius) {std::cout << "ERROR!" << std::endl; exit(1);}
    vFile << voxelID << " " << x << " " << y << std::endl;
    for (unsigned e = 1; e <= nEvents; e++)
    {
      lssFile << voxelID << " " << nPrim << std::endl; 
    }
    voxelID++;
    vFile << voxelID << " " << x << " " << -1*y << std::endl;
    g->SetPoint(voxelID-1, x, -y);
    for (unsigned e = 1; e <= nEvents; e++)
    {
      lssFile << voxelID << " " << nPrim << std::endl; 
    }
    voxelID++;

    alpha = alpha - 0.5;
  }


  lssFile.close();
  vFile.close();

  g->SetMarkerStyle(21);
  g->SetMarkerSize(2);
  g->SetMaximum(14.5);
  g->SetMinimum(-14.5);
  g->GetXaxis()->SetLimits(-14.5,14.5);
  g->Draw("ap");
}

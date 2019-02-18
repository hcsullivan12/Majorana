#include <iostream>
#include <fstream>

float  diskRadius = 14.5;
double inc        = 0.5;
double r          = 7.0; 
unsigned nEvents  = 25;
unsigned nPrim    = 500000;

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

  // Do the alpha = 90 first
  unsigned voxelID(1);
  vFile << voxelID << " " << 0 << " " << diskRadius - r << " " << std::endl;
  for (unsigned e = 1; e <= nEvents; e++)
  {
    lssFile << voxelID << " " << nPrim << std::endl; 
  }
  g->SetPoint(voxelID-1, 0, diskRadius-r);
  voxelID++;

  // Starting point
  double alpha = 70;
  while (alpha > 0)
  {
    // Compute x and y
    double y = diskRadius - r*std::cos(alpha*TMath::Pi()/180);
    double x = r*std::sin(alpha*TMath::Pi()/180);
    g->SetPoint(voxelID-1, x, y);
    double R = std::sqrt(x*x+y*y);
    if (R >= diskRadius) {std::cout << "ERROR!" << std::endl; exit(1);}
    vFile << voxelID << " " << x << " " << y << std::endl;

    for (unsigned e = 1; e <= nEvents; e++)
    {
      lssFile << voxelID << " " << nPrim << std::endl; 
    }
    voxelID++;

    // Now do the mirror image
    x = -1*x;
    y = y;
    vFile << voxelID << " " << x << " " << y << std::endl;
    g->SetPoint(voxelID-1, x, y);
    for (unsigned e = 1; e <= nEvents; e++)
    {
      lssFile << voxelID << " " << nPrim << std::endl; 
    }
    voxelID++;

    alpha = alpha - 1;
  }


  lssFile.close();
  vFile.close();

  TF2* f = new TF2("f", "x*x + y*y", -15.5, 15.5, -15.5, 15.5);
  double contours[1];
  contours[0] = 14.5*14.5;
  f->SetContour(1, contours);
  f->Draw("cont3");

  g->SetMarkerStyle(21);
  g->SetMarkerSize(2);
  g->SetMaximum(14.5);
  g->SetMinimum(-14.5);
  g->GetXaxis()->SetLimits(-14.5,14.5);
  g->Draw("p same");
}

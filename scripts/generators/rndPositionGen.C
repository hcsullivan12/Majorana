#include <iostream>
#include <fstream>
 
Double_t diskRadius = 14.5;

// rng
TRandom rng;

void GetPosition(Double_t& x, Double_t& y)
{
  x = rng.Uniform(-diskRadius+1., diskRadius-1.);
  y = rng.Uniform(-diskRadius+1., diskRadius-1.);

  Double_t r = std::sqrt(x*x+y*y);
  if (r >= (diskRadius-1)) GetPosition(x, y);
}

void rndPositionGen(unsigned nPositions)
{
  std::string filename = "../config/LightSourceSteering.txt";
  std::ofstream file(filename.c_str());
  if (!file.is_open()) return;

  rng.SetSeed(time(NULL));

  // output top line
  file << "x y n" << std::endl; 

  // Now shift
  for (unsigned p = 1; p <= nPositions; p++)
  {
    Double_t x, y;
    GetPosition(x,y);
    
    file << x << " " << y << " 50000" << std::endl;
  }

  file.close();
}

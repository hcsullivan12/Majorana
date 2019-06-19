//
// File: Reconstructor.cxx
//
// Author: Hunter Sullivan
//
// Description: Structure to run reconstruction on sipm data.
//

#include "Reconstructor.h"

#include "TFile.h"
#include "TH2F.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"

#include <iostream>
#include <cmath>

namespace majorana
{

Reconstructor::Reconstructor()
{
  fPixelEstimates = std::make_unique<std::vector<float>>();
  fPixelEstimates->clear();
  fPixelVec = std::make_shared<std::vector<Pixel>>();
  fPixelVec.get()->clear();
}

Reconstructor::~Reconstructor()
{}

void Reconstructor::Initialize(const std::map<unsigned, unsigned>& data,
                               std::shared_ptr<std::vector<Pixel>> pixelVec,
                               const float& diskRadius)
{
  fData      = data;
  fPixelVec.reset();
  fPixelVec = pixelVec;

  fPixelEstimates->resize(fPixelVec->size());

  fDenomSums.clear();
  fDenomSums.resize(fData.size());
  fDiskRadius = diskRadius;
  fLogLikehs.clear();
}

void Reconstructor::Reconstruct()
{
  // The procedure:
  //
  //   1) Make initial estimates for pixel intensities
  //   2) Make next prediction
  //   3) Check for convergence
  //        *if yes, save, return
  //        *if not, old estimates = current estimates -> 2)
  //

  // 1)
  InitPixelList();
  // 2)
  unsigned iteration(0);
  Estimate(iteration);
}

void Reconstructor::InitPixelList()
{
  // We will fill the first iteration
  // We will sample from a uniform distribution
  // of the total amount of light seen at the
  // photodetectors
  unsigned totalPE(0);
  for (const auto& d : fData) totalPE = totalPE + d.second;

  // Sample
  std::srand(time(NULL));
  for (auto& v : *fPixelVec)
  {
    double u = rand()%totalPE+1;
    v.SetIntensity(u);
  }
  // Log likelihood
  //fLogLikehs.push_back(CalculateLL());
}

void Reconstructor::Estimate(unsigned& iteration)
{
  iteration++;
  if (iteration > 200) return;

  // To reduce complexity, find denominator sum seperately
  for (const auto& d : fData)
  {
    float denomSum = DenominatorSum(d.first); 
    fDenomSums[d.first-1] = denomSum;
  }
  for (auto& pixel : *fPixelVec)
  {
    // pixelID
    unsigned pixelID = pixel.ID();
    float    theEst  = pixel.Intensity(); 
    std::vector<float> opRefTable = pixel.ReferenceTable();
    // Apply the money formula
    float nextEst = MoneyFormula(pixelID, theEst, opRefTable);
    (*fPixelEstimates)[pixelID-1] = nextEst;
  }

  // 3) 
	//bool didConverge = CheckConvergence();
	//if (!didConverge) 
  {
    // Log likelihood
    //fLogLikehs.push_back(CalculateLL());
    Reset();
    Estimate(iteration);
  }
}

float Reconstructor::CalculateLL()
{
  // Loop over detectors
  float sum(0);
  for (const auto& d : fData)
  {
    float mean = CalculateMean(d.first);
    sum = sum + d.second*std::log(mean) - mean - std::log(TMath::Factorial(d.second));
  }
  return sum;
}

float Reconstructor::CalculateMean(const unsigned& sipmID)
{
  // Loop over pixels
  float sum(0);
  for (const auto& pixel : *fPixelVec)
  {
    auto opRefTable = pixel.ReferenceTable();
    sum = sum + opRefTable[sipmID-1]*pixel.Intensity();
  }
}

void Reconstructor::Reset()
{
  // Update the intensities
  for (auto& v : *fPixelVec)
  {
    unsigned id = v.ID();
    float nextEst = (*fPixelEstimates)[id-1];
    v.SetIntensity(nextEst);
  }
}

float Reconstructor::MoneyFormula(const unsigned& pixelID, 
                                  const float&    theEst,
                                  const std::vector<float>& opRefTable)
{
  // Looping over detectors
  float sum(0);
  float totalP(0);
  for (const auto& d : fData)
  {
    // n p.e. and ref table
    unsigned n = d.second;
    float p(0);
    if (d.first > opRefTable.size())
    {
      std::cerr << "Uh oh! Could not find MPPC" << d.first << " in reference table!" << std::endl;
    }
    else p = opRefTable[d.first-1];
    // Add to totalP
    totalP = totalP + p;

    sum = sum + n*p/fDenomSums[d.first-1];
  }
  return theEst*sum/totalP;
}

float Reconstructor::DenominatorSum(const unsigned& mppcID)
{
  // Loop over pixels
  float denomSum(0);
  for (const auto& pixel : *fPixelVec)
  {
    float theEst = pixel.Intensity();
    float p(0);
    auto opRefTable = pixel.ReferenceTable();
    if (mppcID > opRefTable.size())
    {
      std::cerr << "Uh oh! Could not find MPPC" 
                << mppcID << " in reference table!" << std::endl;
    }
    else p = opRefTable[mppcID-1];
    denomSum = denomSum + theEst*p;
  }
  return denomSum;
}

bool Reconstructor::CheckConvergence()
{
  return false;
}

void Reconstructor::MakePlots(const std::string& filename)
{
  TFile f(filename.c_str(), "UPDATE");
  // Set bin size
  float pixelSpacing = (*fPixelVec).front().Size();
  unsigned n = 2*fDiskRadius/pixelSpacing - 1; // assuming pixel is in the center

  std::string name = "histFinal";
  TH2F hist(name.c_str(), name.c_str(), n, -fDiskRadius, fDiskRadius, n, -fDiskRadius, fDiskRadius);

  // Fill and find mean position weighted by charge
  float meanX(0);
  float meanY(0);
  float totalInt(0);
  for (const auto& v : *fPixelVec)
  {
    unsigned xbin = hist.GetXaxis()->FindBin(v.X());
    unsigned ybin = hist.GetYaxis()->FindBin(v.Y());
    hist.SetBinContent(xbin, ybin, v.Intensity());

    meanX = meanX + v.X()*v.Intensity();
    meanY = meanY + v.Y()*v.Intensity();
    totalInt = totalInt + v.Intensity();
  } 
  hist.Write();

  meanX = meanX/totalInt;
  meanY = meanY/totalInt;
  fMLX = meanX;
  fMLY = meanY;
  fMLN0 = totalInt;
}
}

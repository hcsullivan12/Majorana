/**
 * @file Reconstructor.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief The SiPM wheel reconstruction algorithm.
 * @date 07-04-2019
 * 
 */

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

//------------------------------------------------------------------------
Reconstructor::Reconstructor()
 : fDoPenalized(true),
   fDiskRadius(0),
   fGamma(0),
   fPenalizedIterStop(100),
   fUnpenalizedIterStop(100)
{
  fPixelVec = std::make_shared<std::vector<Pixel>>();
  fPixelVec.get()->clear();
}

//------------------------------------------------------------------------
Reconstructor::~Reconstructor()
{}

//------------------------------------------------------------------------
void Reconstructor::Initialize(const std::map<size_t, size_t>&     data,
                               std::shared_ptr<std::vector<Pixel>> pixelVec,
                               const float&                        diskRadius,
                               const bool&                         doPenalized,
                               const float&                        gamma, 
                               const size_t&                       pStop,
                               const size_t&                       upStop)
{
  fData      = data;
  fPixelVec.reset();
  fPixelVec = pixelVec;

  fDoPenalized = doPenalized;
  fPenalizedIterStop   = pStop;
  fUnpenalizedIterStop = upStop;

  fDenomSums.clear();
  fDenomSums.resize(fData.size());
  fDiskRadius = diskRadius;
  fLogLikehs.clear();
}

//------------------------------------------------------------------------
void Reconstructor::Reconstruct()
{
  // first do unpenalized
  // this will give us our prior for a penalized reconstruction
  DoUnpenalized();

  // Update histogram
  UpdateHistogram();

  // option to do penalized
  if (fDoPenalized) 
  {
    // Initialize our priors
    InitializePriors();
    DoPenalized();
  }
}

//------------------------------------------------------------------------
void Reconstructor::DoUnpenalized()
{
  // Initialize the pixels
  InitPixelList();
  // Start iterating
  size_t iteration(1);
  while (iteration <= fUnpenalizedIterStop) UnpenalizedEstimate();
}

//------------------------------------------------------------------------
void Reconstructor::InitializePriors()
{

}

//------------------------------------------------------------------------
void Reconstructor::UnpenalizedEstimate()
{
  // To reduce complexity, find denominator sum seperately
  for (const auto& d : fData)
  {
    float denomSum = DenominatorSum(d.first); 
    fDenomSums[d.first-1] = denomSum;
  }
  for (auto& pixel : *fPixelVec)
  {
    // Get the current estimate, and reference table
    float              theEst      = pixel.Intensity(); 
    std::vector<float> theRefTable = pixel.ReferenceTable();
    // Apply the money formula
    float nextEst = MoneyFormula(theEst, theRefTable);
    // Update
    pixel.SetIntensity(nextEst);
  }
}

//------------------------------------------------------------------------
void Reconstructor::DoPenalized()
{
  // Initialize the pixels
  InitPixelList();
  // Start iterating
  size_t iteration(1);
  while (iteration <= fPenalizedIterStop) PenalizedEstimate();
}

//------------------------------------------------------------------------
void Reconstructor::PenalizedEstimate()
{
  // Run the unpenalized estimate
  UnpenalizedEstimate();

  // Apply penalized formula
  // We will assume an identity matrix for now 
  float var = 1.0;
  for (auto& pixel : *fPixelVec)
  {
    float m     = fPriors[pixel.ID()-1];
    float term1 = fGamma*var*m;
    float est   = pixel.Intensity();
    float nextEst = ( term1 - 1 + std::sqrt((term1-1)*(term1-1) + 4*fGamma*var*est) )/(2*fGamma*var);
    pixel.SetIntensity(nextEst);
  }
  PenalizedEstimate(iteration);
}

//------------------------------------------------------------------------
void Reconstructor::InitPixelList()
{
  // We will fill the first iteration
  // We will sample from a uniform distribution
  // of the total amount of light seen at the
  // photodetectors
  size_t totalPE(0);
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

//------------------------------------------------------------------------
float Reconstructor::CalculateLL()
{
  // Loop over detectors
  float sum(0);

  for (const auto& d : fData)
  {
    float mean = CalculateMean(d.first);
    sum = sum + d.second*std::log(mean) - mean - std::log(doFactorial(d.second));
  }
  return sum;
}

//------------------------------------------------------------------------
float Reconstructor::CalculateMean(const size_t& sipmID)
{
  // Loop over pixels
  float sum(0);
  for (const auto& pixel : *fPixelVec)
  {
    auto opRefTable = pixel.ReferenceTable();
    sum = sum + opRefTable[sipmID-1]*pixel.Intensity();
  }
}

//------------------------------------------------------------------------
float Reconstructor::MoneyFormula(const float&              theEst,
                                  const std::vector<float>& theRefTable)
{
  // Looping over detectors
  float sum(0);
  float totalP(0);
  for (const auto& d : fData)
  {
    // n p.e. and ref table
    size_t n = d.second;
    float p(0);
    if (d.first > theRefTable.size())
    {
      std::cerr << "Uh oh! Could not find MPPC" << d.first << " in reference table!" << std::endl;
    }
    else p = theRefTable[d.first-1];
    // Add to totalP
    totalP = totalP + p;

    sum = sum + n*p/fDenomSums[d.first-1];
  }
  return theEst*sum/totalP;
}

//------------------------------------------------------------------------
float Reconstructor::DenominatorSum(const size_t& mppcID)
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

//------------------------------------------------------------------------
bool Reconstructor::CheckConvergence()
{
  return false;
}

//------------------------------------------------------------------------
void Reconstructor::UpdateHistogram()
{
  // Set bin size
  float pixelSpacing = (*fPixelVec).front().Size();
  size_t n = 2*fDiskRadius/pixelSpacing - 1; // assuming pixel is in the center

  std::string name = "histFinal";
  TH2F hist(name.c_str(), name.c_str(), n, -fDiskRadius, fDiskRadius, n, -fDiskRadius, fDiskRadius);

  // Fill and find total amount of light
  float totalInt(0);
  for (const auto& v : *fPixelVec)
  {
    size_t xbin = hist.GetXaxis()->FindBin(v.X());
    size_t ybin = hist.GetYaxis()->FindBin(v.Y());
    hist.SetBinContent(xbin, ybin, v.Intensity());
    totalInt = totalInt + v.Intensity();
  } 

  // Let's fit our current estimate using a 2D gaussian
  TF2 g("g", "bigaus", -fDiskRadius, fDiskRadius, -fDiskRadius, fDiskRadius);
  hist.Fit(&g);
  auto max = g.GetMaximum();

  fMLHist       = hist;
  fMLX          = max[0];
  fMLY          = max[1];
  fMLTotalLight = totalInt;
}

}

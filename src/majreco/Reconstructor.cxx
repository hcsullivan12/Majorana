/**
 * @file Reconstructor.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief The SiPM wheel reconstruction algorithm.
 * @date 07-04-2019
 * 
 */

#include "majreco/Reconstructor.h"

#include "TFile.h"
#include "TH2F.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"

#include <iostream>
#include <cmath>

namespace majreco
{

//------------------------------------------------------------------------
Reconstructor::Reconstructor()
 : fDiskRadius(0),
   fGamma(0),
   fPenalizedIterStop(100),
   fUnpenalizedIterStop(100),
   fMLHistogram(nullptr),
   fMLGauss(nullptr)
{
  fPixelVec = std::make_shared<std::vector<majutil::Pixel>>();
  fPixelVec.get()->clear();
}

//------------------------------------------------------------------------
Reconstructor::~Reconstructor()
{
  if (!fMLHistogram) delete fMLHistogram;
  if (!fMLGauss)     delete fMLGauss;
}

//------------------------------------------------------------------------
void Reconstructor::Initialize(const std::map<size_t, size_t>&     data,
                               std::shared_ptr<std::vector<majutil::Pixel>> pixelVec,
                               const float&                        diskRadius,
                               const float&                        gamma, 
                               const size_t&                       pStop,
                               const size_t&                       upStop)
{
  fData      = data;
  fPixelVec.reset();
  fPixelVec = pixelVec;

  fPenalizedIterStop   = pStop;
  fUnpenalizedIterStop = upStop;
  fGamma = gamma;

  fDenomSums.clear();
  fDenomSums.resize(fData.size());
  fDiskRadius = diskRadius;
  fLogLikehs.clear();

  // Init histo and gaussian
  float pixelSpacing = (*fPixelVec).front().Size();
  size_t n = 2*fDiskRadius/pixelSpacing - 1; // assuming pixel is in the center
  std::string name = "histFinal";
  if (!fMLHistogram) fMLHistogram = new TH2F(name.c_str(), name.c_str(), n, -fDiskRadius, fDiskRadius, n, -fDiskRadius, fDiskRadius);
  if (!fMLGauss)     fMLGauss     = new TF2("g", "bigaus", -fDiskRadius, fDiskRadius, -fDiskRadius, fDiskRadius);
}

//------------------------------------------------------------------------
void Reconstructor::Reconstruct(const bool& doPenalized)
{
  // first do unpenalized
  // this will give us our prior for a penalized reconstruction
  std::cout << "Starting unpenalized reconstruction...\n";
  DoUnpenalized();

  // Update histogram
  std::cout << "Updating histogram...\n";
  UpdateHistogram();

  // Let's fit our current estimate using a 2D gaussian
  fMLHistogram->Fit(fMLGauss, "NQ");
  Double_t maxX, maxY;
  fMLGauss->GetMaximumXY(maxX, maxY);
  fMLX          = maxX;
  fMLY          = maxY;

  // option to do penalized
  if (doPenalized) 
  {
    // Initialize our priors
    InitializePriors();

    std::cout << "Starting penalized reconstruction...\n";
    DoPenalized();
    std::cout << "Updating histogram...\n";
    UpdateHistogram();
  }
}

//------------------------------------------------------------------------
void Reconstructor::InitializePriors()
{
  // We should an updated gaussian fit now
  fPriors.resize(fPixelVec->size());
  for (const auto& pixel : *fPixelVec) fPriors[pixel.ID()-1] = fMLGauss->Eval(pixel.X(), pixel.Y());
}

//------------------------------------------------------------------------
void Reconstructor::DoUnpenalized()
{
  // Initialize the pixels
  InitPixelList();
  // Start iterating
  for (size_t iter = 1; iter <= fUnpenalizedIterStop; iter++) UnpenalizedEstimate();
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
  for (size_t iter = 1; iter <= fPenalizedIterStop; iter++) 
  {
    PenalizedEstimate();
  }
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
  // Fill and find total amount of light
  float totalInt(0);
  for (const auto& v : *fPixelVec)
  {
    size_t xbin = fMLHistogram->GetXaxis()->FindBin(v.X());
    size_t ybin = fMLHistogram->GetYaxis()->FindBin(v.Y());
    fMLHistogram->SetBinContent(xbin, ybin, v.Intensity());
    totalInt = totalInt + v.Intensity();
  } 
  fMLTotalLight = totalInt;
}

//------------------------------------------------------------------------
void Reconstructor::Dump()
{
  std::cout << "\nNumber of pixels:   " << fPixelVec->size()
            << "\nUnpenalized stop:   " << fUnpenalizedIterStop
            << "\nPenalized stop:     " << fPenalizedIterStop
            << "\nPenalty strength:   " << fGamma
            << "\nDisk radius:        " << fDiskRadius
            << "\nML estimate for X:  " << fMLX
            << "\nML estimate for Y:  " << fMLY
            << "\nML estimate for LY: " << fMLTotalLight
            << "\n";
}

}

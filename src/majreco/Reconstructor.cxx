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
Reconstructor::Reconstructor(const std::map<size_t, size_t>&              data,
                             std::shared_ptr<std::vector<majutil::Pixel>> pixelVec,
                             const float&                                 diskRadius)
 : fData(data),
   fDiskRadius(diskRadius),
   fGamma(0),
   fPenalizedIterStop(100),
   fUnpenalizedIterStop(100),
   fMLHist(nullptr),
   fMLGauss(nullptr),
   fChi2Hist(nullptr)
{
  fPixelVec = std::make_shared<std::vector<majutil::Pixel>>();
  fPixelVec.get()->clear();
  fPixelVec.reset();
  fPixelVec = pixelVec;

  fDenomSums.clear();
  fDenomSums.resize(fData.size());
  fLogLikehs.clear();

  // Initialize histogram and gaussian
  float pixelSpacing = (*fPixelVec).front().Size();
  size_t n = 2*fDiskRadius/pixelSpacing - 1; // assuming pixel is in the center
  if (!fMLHist)   fMLHist  = new TH2F("histFinal", "histFinal", n, -fDiskRadius, fDiskRadius, n, -fDiskRadius, fDiskRadius);
  if (!fMLGauss)  fMLGauss = new TF2("g", "bigaus", -fDiskRadius, fDiskRadius, -fDiskRadius, fDiskRadius);
  if (!fChi2Hist) fChi2Hist = new TH2F("chi2Final", "chi2Final", n, -fDiskRadius, fDiskRadius, n, -fDiskRadius, fDiskRadius);
}


//------------------------------------------------------------------------
Reconstructor::~Reconstructor()
{
  if (fMLHist)   delete fMLHist;
  if (fMLGauss)  delete fMLGauss;
  if (fChi2Hist) delete fChi2Hist;
}

/**
 * @brief Reconstructs the image by maximizing the likelihood. 
 * 
 * There are two versions: unpenalized and penalized. 
 * The unpenalized method uses expectation maximization to minimize 
 * the likelihood. The resulting 'Money Formula' is applied. The result 
 * is an image of reconstructed pixel colors.
 * The penalized method introduces a prior and maximizes the posterior. 
 * This reduces to minimizing the likelihood where a regularization function
 * has been added to the likelihood function. The regularization function is 
 * chosen so that the prior is gaussian. 
 * 
 */
void Reconstructor::DoEmMl(const float&  gamma, 
                           const size_t& upStop,
                           const size_t& pStop,
                           const bool&   doPenalized)
{
  // Initialize variables
  fPenalizedIterStop   = pStop;
  fUnpenalizedIterStop = upStop;
  fGamma               = gamma;

  // first do unpenalized
  // this will give us our prior for a penalized reconstruction
  std::cout << "Starting unpenalized reconstruction...\n";
  DoUnpenalized();
  // Update histogram
  UpdateHistogram();

  // Let's fit our current estimate using a 2D gaussian
  fMLHist->Fit(fMLGauss, "NQ");
  Double_t maxX, maxY;
  fMLGauss->GetMaximumXY(maxX, maxY);
  fEstimateX          = maxX;
  fEstimateY          = maxY;

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

/**
 * @brief Reconstructs mean position based on Chi2 minimization.
 * 
 * This method uses the lookup tables to estimate the expected number of 
 * counts for each detector. A Chi2 is calculated and minimized using the 
 * expected and the measured counts. After minimization, a 2D gaussian is
 * formed using the minimum chi2 X and Y value.
 * 
 * @todo Need to devise a way to get the number of photons to be used in
 *       the 'expected' calculation. Also, what do we use for the sigma 
 *       in the gaussian? 
 * 
 */
void Reconstructor::DoChi2()
{
  // we have the "data", what is the truth?

  // Let's go backwards
  // Assuming 50,000 photons were fired from a single pixel,
  // what is the expected light yield from each pixel?
  // Compute chi2 for pixel w/ to data

  size_t nPhotons = 50000;
  struct Chi2Pixel {
    std::vector<float> vertex;
    size_t Id;
  };
  Chi2Pixel chi2Pixel;
  float chi2Min(std::numeric_limits<float>::max());
  
  // Loop over pixels
  for (const auto& pixel : *fPixelVec)
  {
    // The lookup table for this pixel
    auto lookupTable = pixel.ReferenceTable();
    size_t nDet = fData.size();
    std::map<size_t, size_t> expectedData;

    // Loop over detectors
    for (size_t d = 1; d <= nDet; d++)
    {
      // How much light do we expect?
      size_t nExpected = nPhotons * lookupTable[d-1];
      expectedData.emplace(d, nExpected);
    }

    // Now calculate chi2 for this
    float chi2(0);
    for (size_t d = 1; d <= nDet; d++)
    {
      size_t nExpected = expectedData.find(d)->second;
      size_t nMeasured = fData.find(d)->second;

      float diff2 = (nExpected - nMeasured)*(nExpected - nMeasured);
      chi2 = chi2 + diff2/nExpected;
    }

    // Fill chi2 distribution
    size_t xBin = fChi2Hist->GetXaxis()->FindBin(pixel.X());
    size_t yBin = fChi2Hist->GetYaxis()->FindBin(pixel.Y());
    fChi2Hist->SetBinContent(xBin, yBin, chi2);

    // check for minimum
    if (chi2 < chi2Min)
    {
      chi2Min = chi2;
      chi2Pixel.Id = pixel.ID();
      chi2Pixel.vertex.clear();
      chi2Pixel.vertex.push_back(pixel.X());
      chi2Pixel.vertex.push_back(pixel.Y());
    }
  }

  std::cout << "\nChi2 pixel information:"
            << "\nX  = " << chi2Pixel.vertex[0] 
            << "\nY  = " << chi2Pixel.vertex[1]
            << "\nId = " << chi2Pixel.Id
            << "\n";

  // form a 2D gaussian hypothesis centered on chi2 prediction
  float sigma = 2.;
  if (fMLGauss) delete fMLGauss;
  
  fMLGauss = new TF2("g", "bigaus", -fDiskRadius, fDiskRadius, -fDiskRadius, fDiskRadius);
  fMLGauss->SetParameters(nPhotons, chi2Pixel.vertex[0], sigma, chi2Pixel.vertex[1], sigma, 0);
  std::cout << fMLGauss->Eval(chi2Pixel.vertex[0], chi2Pixel.vertex[1]) << std::endl; 

  Double_t x, y;
  fMLGauss->GetMaximumXY(x, y);
  fEstimateX = x;
  fEstimateY = y;
}

//------------------------------------------------------------------------
void Reconstructor::InitializePriors()
{
  // We should an updated gaussian fit now
  fPriors.resize(fPixelVec->size());
  for (const auto& pixel : *fPixelVec) 
  {
    auto content = fMLGauss->Eval(pixel.X(), pixel.Y());
    auto xBin = fMLHist->GetXaxis()->FindBin(pixel.X());
    auto yBin = fMLHist->GetYaxis()->FindBin(pixel.Y());
    fPriors[pixel.ID()-1] = content;
  }
}

/**
 * @brief Do unpenalized version of reconstruction.
 * 
 * The procedure is as follows:
 * 1) Make initial estimates for pixel intensities
 * 2) Make next prediction
 * 3) Check for convergence
 *      if yes, save, return
 *      if not, old estimates = current estimates -> 2)
 * 
 */
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

/**
 * @brief Do penalized version of reconstruction. Same as 
 *        unpenalized except uses different formula to update
 *        estimates.
 * 
 */
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

/**
 * @brief Updates the ML histogram with current estimates. Fits 
 *        distribution using a 2D gaussian function to find the
 *        mean x and y coordinates.
 * 
 */
void Reconstructor::UpdateHistogram()
{
  // Fill and find total amount of light
  float totalInt(0);
  for (const auto& v : *fPixelVec)
  {
    size_t xbin = fMLHist->GetXaxis()->FindBin(v.X());
    size_t ybin = fMLHist->GetYaxis()->FindBin(v.Y());
    fMLHist->SetBinContent(xbin, ybin, v.Intensity());
    totalInt = totalInt + v.Intensity();
  } 
  fEstimateTotalLight = totalInt;
}

//------------------------------------------------------------------------
void Reconstructor::Dump()
{
  std::cout << "\nNumber of pixels:   " << fPixelVec->size()
            << "\nUnpenalized stop:   " << fUnpenalizedIterStop
            << "\nPenalized stop:     " << fPenalizedIterStop
            << "\nPenalty strength:   " << fGamma
            << "\nDisk radius:        " << fDiskRadius
            << "\nEstimate for X:     " << fEstimateX
            << "\nEstimate for Y:     " << fEstimateY
            << "\nEstimate for LY:    " << fEstimateTotalLight
            << "\n";
}

}

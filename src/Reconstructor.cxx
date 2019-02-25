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
#include "TCanvas.h"
#include "TImage.h"

#include <iostream>
#include <cmath>

namespace majorana
{

Reconstructor::Reconstructor()
{}

Reconstructor::~Reconstructor()
{}

void Reconstructor::Initialize(const std::map<unsigned, unsigned>& data,
                               const std::list<Voxel>& voxelList)
{
  fData      = data;
  fVoxelList = voxelList;
  fVoxelEstimates.clear();
  fVoxelEstimates.resize(fVoxelList.size());
  fDenomSums.clear();
  fDenomSums.resize(fData.size());
  fNumber = 0;
}

void Reconstructor::Reconstruct()
{
  // The procedure:
  //
  //   1) Make initial estimates for voxel intensities
  //   2) Make next prediction
  //   3) Check for convergence
  //        *if yes, save, return
  //        *if not, initial estimates = current estimates -> 2)
  //

  // 1)
  InitVoxelList();
  // 2)
  unsigned iteration(0);
  Estimate(iteration);
}

void Reconstructor::InitVoxelList()
{
  // We will fill the first iteration
  // We will sample from a uniform distribution
  // of the total amount of light seen at the
  // photodetectors
  // But I think we have to bias this slightly:
  //   voxels near the sipms need to have low intensities
  //   since the probabilities of detection are relatively 
  //   high there ??
  unsigned totalPE(0);
  for (const auto& d : fData) totalPE = totalPE + d.second;

  // Sample
  std::srand(time(NULL));
  for (auto& v : fVoxelList)
  {
    //if (v.R() > 12) { v.SetIntensity(rand()%10 + 1); continue;}
    double u = rand()%totalPE+1;
    v.SetIntensity(u);
  }
  //***
  // TEMPORARY
  //***
  //MakePlots("/home/hunter/projects/Majorana/output/recoAnaTree.root");
}

void Reconstructor::Estimate(unsigned& iteration)
{
  iteration++;
  if (iteration > 100) return;

  // Log likelihood
  CalculateLL();

  // Temp
  //MakePlots("/home/hunter/projects/Majorana/output/recoAnaTree.root");

  // To reduce complexity, find denominator sum seperately
  // Old: O(nSiPMS*nVoxels*nVoxels)
  // New: O(2*nSiPMS*nVoxels)
  for (const auto& d : fData)
  {
    float denomSum = DenominatorSum(d.first); 
    fDenomSums[d.first-1] = denomSum;
  }
  for (auto& voxel : fVoxelList)
  {
    // voxelID
    unsigned voxelID = voxel.ID();
    float    theEst  = voxel.Intensity(); 
    std::vector<float> opRefTable = voxel.ReferenceTable();
    // Apply the money formula
    float nextEst = MoneyFormula(voxelID, theEst, opRefTable);
    fVoxelEstimates[voxelID-1] = nextEst;
  }

  // 3) 
	//bool didConverge = CheckConvergence();
	//if (!didConverge) 
  {
    CalculateLL();
    Reset();
    Estimate(iteration);
  }
}

void Reconstructor::CalculateLL()
{
  // Loop over detectors
  float sum(0);
  for (const auto& d : fData)
  {
    float mean = CalculateMean(d.first);
    sum = sum + d.second*std::log(mean) - mean;
  }
  //std::cout << sum << std::endl;
}

float Reconstructor::CalculateMean(const unsigned& sipmID)
{
  // Loop over voxels
  float sum(0);
  for (const auto& voxel : fVoxelList)
  {
    auto opRefTable = voxel.ReferenceTable();
    sum = sum + opRefTable[sipmID-1]*voxel.Intensity();
  }
}

void Reconstructor::Reset()
{
  // Update the intensities
  for (auto& v : fVoxelList)
  {
    unsigned id = v.ID();
    float nextEst = fVoxelEstimates[id-1];
    v.SetIntensity(nextEst);
  }
  fVoxelEstimates.clear();
  fDenomSums.clear();
}

float Reconstructor::MoneyFormula(const unsigned& voxelID, 
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
  // Loop over voxels
  float denomSum(0);
  for (const auto& voxel : fVoxelList)
  {
    float theEst = voxel.Intensity();
    float p(0);
    auto opRefTable = voxel.ReferenceTable();
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

}

void Reconstructor::MakePlots(const std::string& filename)
{
  TFile f(filename.c_str(), "UPDATE");
  unsigned n = 27;
  std::string name = "hist"+std::to_string(fNumber);
  TH2F hist(name.c_str(), name.c_str(), n, -14.5, 14.5, n, -14.5, 14.5);

  for (const auto& v : fVoxelList)
  {
    unsigned xbin = hist.GetXaxis()->FindBin(v.X());
    unsigned ybin = hist.GetYaxis()->FindBin(v.Y());

    hist.SetBinContent(xbin, ybin, v.Intensity());
  } 
  hist.Write();
  f.Close();
  fNumber++;
}
}

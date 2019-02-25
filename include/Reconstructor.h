// 
// File: Reconstructor.h
//
// Author: Hunter Sullivan
//
// Description: Structure to perform reconstruction sipm wheel.
//

#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include "Voxel.h"

#include <map>
#include <list>

namespace majorana
{

class Reconstructor 
{

public:
  Reconstructor(const std::map<unsigned, unsigned>& data,
                const std::list<Voxel>& voxelList);
  ~Reconstructor();
  
  void Reconstruct();
  void Initialize();
  void MakePlots(const std::string& filename);

  const double   ML()    { return fMLLogLikelihood; }
  const float    X()     { return fMLX; }
  const float    Y()     { return fMLY; }
  const float    R()     { return fMLRadius; }
  const float    Theta() { return fMLTheta; }
  const unsigned N0()    { return fMLN0; }
    
private:

  void Estimate(unsigned& iteration);  
  void CalculateLL();
  float CalculateMean(const unsigned& sipmID);
  float DenominatorSum(const unsigned& sipmID);
  float MoneyFormula(const unsigned& voxelID,
                     const float& theEst,
                     const std::vector<float>& referenceTable);
  bool CheckConvergence();
  void Reset();
   
  double                       fMLLogLikelihood; //< Log likelihood for the MLE
  float                        fMLN0;            //< MLE for N0
  float                        fMLX;             //< MLE for x (cm)
  float                        fMLY;             //< MLE for y (cm)
  float                        fMLRadius;        //< MLE for r (cm)
  float                        fMLTheta;         //< MLE for theta (deg)
  std::list<Voxel>             fVoxelList;          //< list of created voxels
  std::vector<float>           fVoxelEstimates;     //< 
  std::vector<float>           fDenomSums;
  std::map<unsigned, unsigned> fData;               //< measured counts (sipm, np.e.)
  unsigned fNumber;
};
}

#endif

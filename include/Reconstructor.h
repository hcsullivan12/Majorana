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

  const double   ML()    { return m_mlLogLikelihood; }
  const float    X()     { return m_mlX; }
  const float    Y()     { return m_mlY; }
  const float    R()     { return m_mlRadius; }
  const float    Theta() { return m_mlTheta; }
  const unsigned N0()    { return m_mlN0; }
    
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
   
  double                       m_mlLogLikelihood; //< Log likelihood for the MLE
  float                        m_mlN0;            //< MLE for N0
  float                        m_mlX;             //< MLE for x (cm)
  float                        m_mlY;             //< MLE for y (cm)
  float                        m_mlRadius;        //< MLE for r (cm)
  float                        m_mlTheta;         //< MLE for theta (deg)
  std::list<Voxel>             m_voxelList;          //< list of created voxels
  std::vector<float>           m_voxelEstimates;     //< 
  std::vector<float>           m_denomSums;
  std::map<unsigned, unsigned> m_data;               //< measured counts (sipm, np.e.)
  unsigned m_number;
};
}

#endif

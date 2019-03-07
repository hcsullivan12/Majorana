// 
// File: Reconstructor.h
//
// Author: Hunter Sullivan
//
// Description: Structure to perform reconstruction sipm wheel.
//

#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include "Pixel.h"

#include <map>
#include <list>

namespace majorana
{

class Reconstructor 
{

public:
  Reconstructor();
  ~Reconstructor();
  
  void Reconstruct();
  void Initialize(const std::map<unsigned, unsigned>& data,
                  const std::list<Pixel>& pixelList,
                  const float& diskRadius);
  void MakePlots(const std::string& filename);

  const double   ML()    const { return fMLLogLikelihood; }
  const float    X()     const { return fMLX; }
  const float    Y()     const { return fMLY; }
  const float    R()     const { return fMLRadius; }
  const float    Theta() const { return fMLTheta; }
  const std::vector<float> PixelEstimates() const { return fPixelEstimates; }
    
private:
  void InitPixelList();
  void Estimate(unsigned& iteration);  
  float CalculateLL();
  float CalculateMean(const unsigned& sipmID);
  float DenominatorSum(const unsigned& sipmID);
  float MoneyFormula(const unsigned& pixelID,
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
  float                        fDiskRadius;
  std::list<Pixel>             fPixelList;          //< list of created pixels
  std::vector<float>           fPixelEstimates;     //< 
  std::vector<float>           fDenomSums;
  std::map<unsigned, unsigned> fData;               //< measured counts (sipm, np.e.)
  std::vector<float>           fLogLikehs;
};
}

#endif

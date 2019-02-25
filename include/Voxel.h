// 
// File: Voxel.h
//
// Author: Hunter Sullivan
//
// Description: Simple voxel structure. 
//

#ifndef VOXEL_H
#define VOXEL_H

#include <vector>

namespace majorana
{

class Voxel 
{

public:
	Voxel(const unsigned& id,
        const float& x, 
        const float& y, 
        const float& r, 
        const float& theta);
	~Voxel();

	float       X()     const { return fX; };
	float       Y()     const { return fY; };
  float       R()     const { return fR; };
  float       Theta() const { return fTheta; };
  float       Size()  const { return fSize; };
  unsigned    ID()    const { return fID; };
  unsigned    NPrimaries() const { return fNPrimaries; };
  float       Intensity() const { return fIntensity; };
  std::vector<float> ReferenceTable() const { return fReferenceTable; };

  void SetSize(const float& s) { fSize = s; };
  void SetIntensity(const float& i) { fIntensity = i; };
  void AddReference(const unsigned& mppcID, const float& prob);

private:
    
  float  fX;      ///< x position that this voxel is centered on
  float  fY;      ///< y position that this voxel is centered on
  float  fR;      ///< radius from center for this voxel
  float  fTheta;  ///< angle with respect to sipm 1 (in degrees)
  float  fSize;   ///< size of voxel 
  float  fIntensity;    ///< if reconstructing, this is the reconstructed intensity
  unsigned fID;         ///< id number
  unsigned fNPrimaries; ///< number of primaries to launch from this voxel
  std::vector<float> fReferenceTable; ///< stores mppc to probability map
};
}

#endif

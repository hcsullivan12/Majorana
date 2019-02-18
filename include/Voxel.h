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

	float       X()     const { return m_x; };
	float       Y()     const { return m_y; };
  float       R()     const { return m_r; };
  float       Theta() const { return m_theta; };
  float       Size()  const { return m_size; };
  unsigned    ID()    const { return m_id; };
  unsigned    NPrimaries() const { return m_nPrimaries; };
  float       Intensity() const { return m_intensity; };
  std::vector<float> ReferenceTable() const { return m_referenceTable; };

  void SetSize(const float& s) { m_size = s; };
  void SetIntensity(const float& i) { m_intensity = i; };
  void AddReference(const unsigned& mppcID, const float& prob);

private:
    
  float  m_x;      ///< x position that this voxel is centered on
  float  m_y;      ///< y position that this voxel is centered on
  float  m_r;      ///< radius from center for this voxel
  float  m_theta;  ///< angle with respect to sipm 1 (in degrees)
  float  m_size;   ///< size of voxel 
  float  m_intensity;    ///< if reconstructing, this is the reconstructed intensity
  unsigned m_id;         ///< id number
  unsigned m_nPrimaries; ///< number of primaries to launch from this voxel
  std::vector<float> m_referenceTable; ///< stores mppc to probability map
};
}

#endif

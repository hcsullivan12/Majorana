/**
 * @file Pixel.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Simple pixel structure.
 * @date 07-04-2019
 *
 */

#ifndef MAJUTIL_PIXEL_H
#define MAJUTIL_PIXEL_H

#include <vector>

namespace majutil
{

class Pixel 
{

using AReferenceTable = std::vector<float>;

public:

  /**
   * @brief Construct a new Pixel object.
   * @todo Change to only require 2 coordinates (not 4).
   * 
   * @param id The pixel ID.
   * @param x The x position.
   * @param y The y position.
   * @param r The r position.
   * @param theta The theta position.
   */
	Pixel(const unsigned& id,
        const float& x, 
        const float& y, 
        const float& r, 
        const float& theta);
	~Pixel();

	float       X()     const { return fX; };
	float       Y()     const { return fY; };
  float       R()     const { return fR; };
  float       Theta() const { return fTheta; };
  float       Size()  const { return fSize; };
  unsigned    ID()    const { return fID; };
  unsigned    NPrimaries() const { return fNPrimaries; };
  float       Intensity() const { return fIntensity; };
  AReferenceTable const& ReferenceTable() const { return fReferenceTable; };

  void SetSize(const float& s) { fSize = s; };
  void SetIntensity(const float& i) { fIntensity = i; };
  void AddReference(const unsigned& mppcID, const float& prob);

private:
    
  float  fX;                       ///< x position that this pixel is centered on
  float  fY;                       ///< y position that this pixel is centered on
  float  fR;                       ///< Radius from center for this pixel
  float  fTheta;                   ///< Angle with respect to sipm 1 (in degrees)
  float  fSize;                    ///< Size of pixel 
  float  fIntensity;               ///< If reconstructing, this is the reconstructed intensity
  unsigned fID;                    ///< ID number
  unsigned fNPrimaries;            ///< Number of primaries to launch from this pixel
  AReferenceTable fReferenceTable; ///< Stores mppc to probability map
};
}

#endif

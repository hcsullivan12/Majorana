/**
 * @file Pixel.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Simple pixel structure.
 * @date 07-04-2019
 *
 */

#include "majutil/Pixel.h"

namespace majutil
{

Pixel::Pixel(const unsigned& pixelID,
             const float&    x, 
             const float&    y, 
             const float&    r, 
             const float&    theta)
 : fID(pixelID),
   fX(x), 
   fY(y), 
   fR(r), 
   fTheta(theta)
{
  fReferenceTable.clear();
}

Pixel::~Pixel()
{}

void Pixel::AddReference(const unsigned& mppcID, const float& prob) 
{
  if (mppcID > fReferenceTable.size())
  {
    fReferenceTable.resize(mppcID);
  } 
  fReferenceTable[mppcID-1] = prob; 
};

}

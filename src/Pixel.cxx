//
// File: Pixel.cxx
//
// Author: Hunter Sullivan
//
// Description: Pixel structure.
//

#include "Pixel.h"

namespace majorana
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

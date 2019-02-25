//
// File: Voxel.cxx
//
// Author: Hunter Sullivan
//
// Description: Voxel structure.
//

#include "Voxel.h"

namespace majorana
{

Voxel::Voxel(const unsigned& voxelID,
             const float&    x, 
             const float&    y, 
             const float&    r, 
             const float&    theta)
 : fID(voxelID),
   fX(x), 
   fY(y), 
   fR(r), 
   fTheta(theta)
{
  fReferenceTable.clear();
}

Voxel::~Voxel()
{}

void Voxel::AddReference(const unsigned& mppcID, const float& prob) 
{
  if (mppcID > fReferenceTable.size())
  {
    fReferenceTable.resize(mppcID);
  } 
  fReferenceTable[mppcID-1] = prob; 
};

}

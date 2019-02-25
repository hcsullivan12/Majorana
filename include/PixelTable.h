//
// File: PixelTable.h
//
// Author: Hunter Sullivan
//
// Discription:
//

#ifndef VOXELTABLE_H
#define VOXELTABLE_H

#include "Pixel.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>    

namespace majorana 
{

class PixelTable
{

public:
  static PixelTable* Instance();
  static PixelTable* CreateInstance();
  ~PixelTable();

  void Initialize(const std::string& pixelizationPath);
  void LoadReferenceTable(const std::string& path);

  const std::list<Pixel>& GetPixels() const { return fPixelList; };
  Pixel*                  GetPixel(const unsigned& id); 
  
private:
  PixelTable();
  static PixelTable* instance;

  std::list<Pixel> fPixelList;

};
}
#endif

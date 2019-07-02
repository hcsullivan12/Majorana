//
// File: PixelTable.h
//
// Author: Hunter Sullivan
//
// Discription:
//

#ifndef PIXELTABLE_H
#define PIXELTABLE_H

#include "Pixel.h"

#include <vector>
#include <string>  
#include <memory>  

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

  std::shared_ptr<std::vector<Pixel>> GetPixels() const { return fPixelVec; };
  Pixel* GetPixel(const unsigned& id); 
  double GetSpacing() const { return fSpacing; }
  
private:
  PixelTable();
  static PixelTable* instance;

  std::shared_ptr<std::vector<Pixel>> fPixelVec;
  double fSpacing; 

};
}
#endif

/**
 * @file PixelTable.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton table for holding the pixelization scheme.
 * @date 07-04-2019
 * 
 */

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

  /**
   * @brief Reads and stores the pixel IDs and positions.
   * 
   * @param pixelizationPath The filepath to the pixelization scheme.
   */
  void Initialize(const std::string& pixelizationPath);

  /**
   * @brief Reads the lookup table.
   * 
   * @param path The filepath to the lookup table.
   */
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

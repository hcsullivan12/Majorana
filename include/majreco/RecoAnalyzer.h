/**
 * @file RecoAnalyzer.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Module to produce a ROOT analysis TTree.
 * @date 07-04-2019
 * 
 */

#ifndef MAJRECO_RECOANALYZER_HH
#define MAJRECO_RECOANALYZER_HH

#include "TTree.h"
#include "TFile.h"

namespace majreco {

/**
 * @brief Class to produce analysis TTree.
 * 
 */
class RecoAnalyzer
{
  public:
    RecoAnalyzer();
    ~RecoAnalyzer();

    void Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY);
      
  private: 
    void ResetVars();

    TTree*      fAnaTree;
    TFile*      fOutputFile;
    std::string fRecoOutputPath;

    int                fEvent;
    int                fNPixels;
    float              fDiskRadius;
    int                fNPrimaries;
    std::vector<float> fSourcePosXYZ;
    std::vector<int> fSiPMToLY;
    int                fRecoTotalLY;
};
}
#endif

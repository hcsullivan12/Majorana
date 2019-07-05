/**
 * @file Analyzer.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Module to produce a ROOT analysis TTree.
 * @date 07-04-2019
 * 
 */

#ifndef MAJRECO_ANALYZER_HH
#define MAJRECO_ANALYZER_HH

#include "TTree.h"

namespace majreco {

/**
 * @brief Class to produce analysis TTree.
 * 
 */
class Analyzer
{
  public:
    Analyzer(const std::string& simOutputPath);
    ~Analyzer();

    void Fill(const unsigned& e);
      
  private: 
    void ResetVars();

    TTree* fAnaTree;
    int    fEvent;
    int    fPixelID;
    int    fNPixels;
    //double fPixelX[kMaxNPixels];
    //double fPixelY[kMaxNPixels];
    int    fNMPPCs;
    double fDiskRadius;
    int    fNPrimaries;
    int    fNPhotonsAbs;
    std::vector<float> fSourcePosXYZ;
    std::vector<float> fSourcePosRTZ;
    std::vector<float> fMPPCToLY;
    std::vector<float> fMPPCToSourceR;
    std::vector<float> fMPPCToSourceT;
    double fMLX;
    double fMLY;

    //double fMLIntensities[kMaxNPixels];
    std::string fSimulateOutputPath;
};
}
#endif

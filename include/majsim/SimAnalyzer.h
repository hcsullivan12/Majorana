/**
 * @file SimAnalyzer.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Module to produce a ROOT analysis TTree.
 * @date 07-04-2019
 * 
 */

#ifndef MAJSIM_SIMANALYZER_HH
#define MAJSIM_SIMANALYZER_HH

#include "TTree.h"

namespace majsim {

/**
 * @brief Class to produce analysis TTree.
 * 
 */
class SimAnalyzer
{
  public:
    SimAnalyzer(const std::string& simOutputPath);
    ~SimAnalyzer();

    void Fill(const unsigned& e);
      
  private: 
    void ResetVars();

    TTree* fAnaTree;
    int    fEvent;
    int    fPixelID;
    int    fNPixels;
    int    fNMPPCs;
    double fDiskRadius;
    int    fNPrimaries;
    int    fNPhotonsAbs;
    std::vector<float> fSourcePosXYZ;
    std::vector<float> fSourcePosRTZ;
    std::vector<float> fMPPCToLY;
    std::vector<float> fMPPCToSourceR;
    std::vector<float> fMPPCToSourceT;
    std::string fSimulateOutputPath;
};
}
#endif

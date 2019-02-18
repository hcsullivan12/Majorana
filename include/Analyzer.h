// 
// File: Analyzer.h
//
// Author: Hunter Sullivan
//
// Discription: 
//

#ifndef ANALYZER_HH
#define ANALYZER_HH

#include "TTree.h"

const int kMaxMPPCs = 100;

namespace majorana {

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
    int    fNMPPCs;
    double fDiskRadius;
    int    fNPrimaries;
    int    fNPhotonsAbs;
    double fSourcePosXYZ[3];
    double fSourcePosRTZ[3];
    double fMPPCToLY[kMaxMPPCs];
    double fMPPCToSourceR[kMaxMPPCs];
    double fMPPCToSourceT[kMaxMPPCs];
    std::string fSimulateOutputPath;
};
}
#endif

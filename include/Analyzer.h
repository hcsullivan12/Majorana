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

// This is so we don't run into any seg faults 
static const int kMaxNMPPCs  = 128;
static const int kMaxNVoxels = 10000;

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
    int    fVoxelID;
    int    fNVoxels;
    int    fNMPPCs;
    double fDiskRadius;
    int    fNPrimaries;
    int    fNPhotonsAbs;
    double fSourcePosXYZ[3];
    double fSourcePosRTZ[3];
    double fMPPCToLY[kMaxNMPPCs];
    double fMPPCToSourceR[kMaxNMPPCs];
    double fMPPCToSourceT[kMaxNMPPCs];
    double fMLX;
    double fMLY;
    double fMLR;
    double fMLT;
    double fMLIntensities[kMaxNVoxels];
    std::string fSimulateOutputPath;
};
}
#endif

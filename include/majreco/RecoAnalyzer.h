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
    TFile * OutputFile()  {return fOutputFile;}

    void Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY,
              const std::vector<float>& SimrecoPosXYZ,
              const std::vector<float>& truePosXYZ,
              const std::vector<float>& DatarecoPosXYZ);
    void Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY,
              const std::vector<float>& SimrecoPosXYZ,
              const std::vector<float>& truePosXYZ,
              const std::vector<float>& DatarecoPosXYZ,
              const float & NoiseETime,
              const float & GoodETime,
              const float & TotalPE_GoodE,
              const float & TotalPE_NoiseE,
              const float & TotalRatio_GoodE,
              const float & TotalRatio_NoiseE

              );
    void Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY,
              const std::vector<float>& SimrecoPosXYZ,
              const std::vector<float>& truePosXYZ,
              const std::vector<float>& DatarecoPosXYZ,
              const float & Time,
              const float & TotalPEDATA,
              const float & TotalPESim,
              const float & Chi2SquareProb,
              const std::vector<float> & Chi2SquareResidues,
              const std::vector<int> & RealData,
              const std::vector<int> & SimData,
              const std::vector<float> & PercentError



    );
    void Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY,
              const std::vector<float>& SimrecoPosXYZ,
              const std::vector<float>& truePosXYZ,
              const std::vector<float>& DatarecoPosXYZ,
              const float & Time,
              const float & TotalPEDATA,
              const float & TotalPESim,
              const float & Chi2SquareProb,
              const std::vector<float> & Chi2SquareResidues,
              const std::vector<int> & RealData,
              const std::vector<int> & SimData,
              const std::vector<float> & PercentError,
              const float & DataRecoChi2Square,
              const int & DataRecoChi2ID,
              const std::vector<float> & DataRecoChi2Vertex,
              const float & SimRecoChi2Square,
              const int & SimRecoChi2ID,
              const std::vector<float> & SimRecoChi2Vertex




    );
    void Fill(const size_t&             e,
              const size_t&             nPixels,
              const float&              diskRadius,
              const size_t&             nPrimaries,
              const std::vector<float>& sourcePosXYZ,
              const std::vector<int>&   sipmToLY,
              const size_t&             recoLY,
              const std::vector<float>& SimrecoPosXYZ,
              const std::vector<float>& truePosXYZ);

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
    std::vector<float> fSimRecoPosXYZ;
    std::vector<float> fTruePosXYZ;
    std::vector<float> fDatarecoPosXYZ;
    float fGoodETime;
    float fNoiseETime;
    int   fEventCounts=0;
    float fTotalPE_GoodE;
    float fTotalPE_NoiseE;
    float fTotalRatio_GoodE;
    float fTotalRatio_NoiseE;
    float fTime;
    float fTotalPEDATA;
    float fTotalPESim;
    float fChi2SquareProb;
    float fDataRecoChi2Square;
    float fSimRecoChi2Square;
    std::vector<float>  fChi2SquareResidues;
    std::vector<int>  fRealData;
    std::vector<int>  fSimData;
    std::vector<float>  fPercentError;
    int  fDataRecoChi2ID;
    std::vector<float>  fDataRecoChi2Vertex;
    int  fSimRecoChi2ID;
    std::vector<float>  fSimRecoChi2Vertex;

};
}
#endif

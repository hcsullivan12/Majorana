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

    void Fill(const unsigned& e);
      
  private: 
    void ResetVars();

    TTree* fAnaTree;

};
}
#endif

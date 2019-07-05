/**
 * @file RecoHelper.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Helper class to interface with simulation output and run reconstruction.
 * @date 07-04-2019
 * 
 */

#ifndef MAJRECO_RECOHELPER_H
#define MAJRECO_RECOHELPER_H

namespace majreco
{

class RecoHelper 
{
  public:
    static RecoHelper* Instance();
    static RecoHelper* CreateInstance();
    ~RecoHelper();

    /**
     * @brief Main entry point.
     * 
     */
    void Start();

  private:
    RecoHelper();
    static RecoHelper* instance; 
};
}

#endif

/**
 * @file OpDetPhotonTable.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton table for holding the detected photons.
 * @date 07-04-2019
 * 
 */

#include "majutil/OpDetPhotonTable.h"
#include "majsim/Configuration.h"

#include "globals.hh"

#include <iostream> 
#include <assert.h>

namespace majutil 
{

//------------------------------------------------------------------------
Photon::Photon(const std::vector<float>& photonVertex)
 : fVertex(photonVertex)
{}

//------------------------------------------------------------------------
Photon::~Photon()
{}

OpDetPhotonTable* OpDetPhotonTable::instance = 0;

//------------------------------------------------------------------------
OpDetPhotonTable* OpDetPhotonTable::CreateInstance(const size_t& nMPPCs)
{
  if (instance == 0)
  {
    static OpDetPhotonTable opDetPhotonTable(nMPPCs);
    instance = &opDetPhotonTable;
  }
  return instance;
}

//------------------------------------------------------------------------
OpDetPhotonTable* OpDetPhotonTable::Instance()
{
  assert(instance);
  return instance;
}

//------------------------------------------------------------------------
OpDetPhotonTable::OpDetPhotonTable(const size_t& nMPPCs)
 : fNPhotonsAbs(0),
   fNOpDet(nMPPCs)
{
  fPhotonsDetected.clear();
  Initialize();
}

//------------------------------------------------------------------------
OpDetPhotonTable::~OpDetPhotonTable()
{}

//------------------------------------------------------------------------
void OpDetPhotonTable::AddPhoton(const unsigned& opchannel, const Photon& photon)
{ 
  if (fPhotonsDetected.find(opchannel) == fPhotonsDetected.end())
  {
    fPhotonsDetected.emplace(opchannel, std::vector<Photon>());
  }
  fPhotonsDetected.find(opchannel)->second.push_back(photon);
}

//------------------------------------------------------------------------
void OpDetPhotonTable::Dump()
{
  for (const auto& k : fPhotonsDetected)
  {
    std::cout << "MPPC" << k.first << " detected " << k.second.size() << " photons\n";
  }
}

//------------------------------------------------------------------------
void OpDetPhotonTable::Initialize()
{
  std::vector<Photon> vec;
  for (unsigned m = 1; m <= fNOpDet; m++)
  {
    fPhotonsDetected.emplace(m, vec);
  }
}

}

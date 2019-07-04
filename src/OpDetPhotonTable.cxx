/**
 * @file OpDetPhotonTable.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton table for holding the detected photons.
 * @date 07-04-2019
 * 
 */

#include "OpDetPhotonTable.h"
#include "Configuration.h"

#include "globals.hh"

#include <iostream> 
#include <assert.h>

namespace majorana 
{

Photon::Photon(const std::vector<float>& photonVertex)
 : fVertex(photonVertex)
{}

Photon::~Photon()
{}

OpDetPhotonTable* OpDetPhotonTable::instance = 0;

OpDetPhotonTable* OpDetPhotonTable::CreateInstance()
{
  if (instance == 0)
  {
    static OpDetPhotonTable opDetPhotonTable;
    instance = &opDetPhotonTable;
  }
  return instance;
}

OpDetPhotonTable* OpDetPhotonTable::Instance()
{
  assert(instance);
  return instance;
}

OpDetPhotonTable::OpDetPhotonTable()
 : fNPhotonsAbs(0)
{
  fPhotonsDetected.clear();
  Initialize();
}

OpDetPhotonTable::~OpDetPhotonTable()
{}

void OpDetPhotonTable::AddPhoton(const unsigned& opchannel, const Photon& photon)
{ 
  if (fPhotonsDetected.find(opchannel) == fPhotonsDetected.end())
  {
    fPhotonsDetected.emplace(opchannel, std::vector<Photon>());
  }
  fPhotonsDetected.find(opchannel)->second.push_back(photon);
}

void OpDetPhotonTable::Print()
{
  //for (const auto& k : fPhotonsDetected)
  {
    //std::cout << "MPPC" << k.first << " detected " << k.second.size() << " photons\n";
  }
}

void OpDetPhotonTable::Initialize()
{
  Configuration* config = Configuration::Instance();
  std::vector<Photon> vec;
  for (unsigned m = 1; m <= config->NMPPCs(); m++)
  {
    fPhotonsDetected.emplace(m, vec);
  }
}

}

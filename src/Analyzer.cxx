// 
// File: Analyzer.cxx
//
// Author: Hunter Sullivan
//
// Discription: 
//

#include "Analyzer.h"
#include "TFile.h"
#include "OpDetPhotonTable.h"
#include "G4Helper.h"

namespace majorana 
{

Analyzer::Analyzer(const std::string& simOutputPath)
 : fAnaTree(NULL),
   fSimulateOutputPath(simOutputPath)
{
  // Reset variables
  ResetVars();
  
  fAnaTree = new TTree("anatree", "analysis tree");
  fAnaTree->Branch("event",      &fEvent, "event/I");
  fAnaTree->Branch("nMPPCs",     &fNMPPCs, "nMPPCs/I");
  fAnaTree->Branch("diskRadius", &fDiskRadius, "diskRadius/D");
  fAnaTree->Branch("nPrimaries", &fNPrimaries, "nPrimaries/I");
  fAnaTree->Branch("sourcePosXYZ", fSourcePosXYZ, "sourcePosXYZ[3]/D");
  fAnaTree->Branch("sourcePosRTZ", fSourcePosRTZ, "sourcePosRTZ[3]/D");
  fAnaTree->Branch("mppcToLY", fMPPCToLY, "mppcToLY[nMPPCs]/D");
  fAnaTree->Branch("mppcToSourceR", fMPPCToSourceR, "mppcToSourceR[nMPPCs]/D");
  fAnaTree->Branch("mppcToSourceT", fMPPCToSourceT, "mppcToSourceT[nMPPCs]/D");
  fAnaTree->Branch("nPhotonsAbsorbed", &fNPhotonsAbs, "nPhotonsAbsorbed/I");
}

Analyzer::~Analyzer()
{
  TFile f(fSimulateOutputPath.c_str(), "UPDATE");
  fAnaTree->Write();
  f.Close();

  if (fAnaTree) delete fAnaTree;
}

void Analyzer::Fill(const unsigned& e)
{
  ResetVars();
  // Get the necessary information
  OpDetPhotonTable* photonTable = OpDetPhotonTable::Instance();
  if (!photonTable) return;
  G4Helper* g4Helper = G4Helper::Instance();
  if (!g4Helper) return;

  // Basically we want to look at the light yield as a function of position 
  fEvent        = e;
  fNMPPCs       = g4Helper->GetDetectorConstruction()->WheelGeometry()->NMPPCs();
  fDiskRadius   = g4Helper->GetDetectorConstruction()->WheelGeometry()->Radius()/10; // convert to cm
  fNPrimaries   = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetNPrimaries();

  auto xyzVec = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetSourcePositionXYZ();
  auto rtzVec = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetSourcePositionRTZ();
  fSourcePosXYZ[0] = xyzVec[0]/10; // convert to cm 
  fSourcePosXYZ[1] = xyzVec[1]/10; // convert to cm 
  fSourcePosXYZ[2] = xyzVec[2]/10; // convert to cm
  fSourcePosRTZ[0] = rtzVec[0]/10; // convert to cm 
  fSourcePosRTZ[1] = rtzVec[1]*180/pi; // convert to degrees   
  fSourcePosRTZ[2] = rtzVec[2]/10; // convert to cm
  
  auto photonsDetected = photonTable->GetPhotonsDetected();
  photonTable->Print();

  fNPhotonsAbs = photonTable->GetNPhotonsAbsorbed();

  for (unsigned m = 1; m <= fNMPPCs; m++)
  {
    int photons(0);
    if (photonsDetected.find(m) != photonsDetected.end())
    {
      photons = photonsDetected.find(m)->second.size();
    }
    fMPPCToLY[m-1] = photons;

    // Calculate distances to each mppc
    float r        = fSourcePosRTZ[0];
    float thetaDeg = fSourcePosRTZ[1];
    float betaDeg  = (m - 1)*360/fNMPPCs;
    float diffRad = (betaDeg - thetaDeg)*pi/180;

    float R = std::sqrt(r*r + fDiskRadius*fDiskRadius - 2*r*fDiskRadius*std::cos(diffRad));
    float alphaDeg = std::abs(TMath::ASin(r*std::sin(diffRad)/R)*180/pi);
    
    fMPPCToSourceR[m-1] = R;
    fMPPCToSourceT[m-1] = alphaDeg;

    std::cout << "SiPM = " << m << " R = " << R << " T = " << alphaDeg << std::endl;
  }
  
  fAnaTree->Fill();
}

void Analyzer::ResetVars()
{
  fEvent  = -99999;
  fNMPPCs = -99999;
  fDiskRadius = -99999;
  fNPrimaries = -99999;
  fNPhotonsAbs = -99999;
  fSourcePosXYZ[0] = -99999; fSourcePosXYZ[1] = -99999; fSourcePosXYZ[2] = -99999;
  fSourcePosRTZ[0] = -99999; fSourcePosRTZ[1] = -99999; fSourcePosRTZ[2] = -99999;
  for (unsigned k = 0; k < kMaxMPPCs; k++)
  {
    fMPPCToLY[k]      = -99999;
    fMPPCToSourceR[k] = -99999;
    fMPPCToSourceT[k] = -99999;
  }
}

}

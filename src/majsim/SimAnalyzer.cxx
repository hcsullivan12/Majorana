/**
 * @file SimAnalyzer.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Module to produce a ROOT analysis TTree from simulation.
 * @date 07-04-2019
 * 
 */

#include "majsim/SimAnalyzer.h"
#include "majsim/G4Helper.h"
#include "majsim/Configuration.h"
#include "majutil/OpDetPhotonTable.h"
#include "majutil/PixelTable.h"

#include "TFile.h"

namespace majsim 
{

//------------------------------------------------------------------------
SimAnalyzer::SimAnalyzer(const std::string& simOutputPath)
 : fAnaTree(NULL),
   fSimulateOutputPath(simOutputPath)
{
  // Reset variables
  ResetVars();
  // Get config
  Configuration* config = Configuration::Instance();
  
  fAnaTree = new TTree("anatree", "analysis tree");
  fAnaTree->Branch("event",      &fEvent, "event/I");
  fAnaTree->Branch("nPixels",    &fNPixels, "nPixels/I");
  //fAnaTree->Branch("pixelX", fPixelX, "pixelX[nPixels]/D");
  //fAnaTree->Branch("pixelY", fPixelY, "pixelY[nPixels]/D");
  fAnaTree->Branch("nMPPCs",     &fNMPPCs, "nMPPCs/I");
  fAnaTree->Branch("diskRadius", &fDiskRadius, "diskRadius/D");
  fAnaTree->Branch("nPrimaries", &fNPrimaries, "nPrimaries/I");
  fAnaTree->Branch("sourcePosXYZ",  &fSourcePosXYZ);
  fAnaTree->Branch("sourcePosRTZ",  &fSourcePosRTZ);
  fAnaTree->Branch("mppcToLY",      &fMPPCToLY);
  fAnaTree->Branch("mppcToSourceR", &fMPPCToSourceR);
  fAnaTree->Branch("mppcToSourceT", &fMPPCToSourceT);
  fAnaTree->Branch("nPhotonsAbsorbed", &fNPhotonsAbs, "nPhotonsAbsorbed/I");
}

//------------------------------------------------------------------------
SimAnalyzer::~SimAnalyzer()
{
  TFile f(fSimulateOutputPath.c_str(), "UPDATE");
  fAnaTree->Write();
  f.Close();

  if (fAnaTree) delete fAnaTree;
}

//------------------------------------------------------------------------
void SimAnalyzer::Fill(const unsigned& e)
{
  ResetVars();

  // Get the necessary information
  majutil::OpDetPhotonTable* photonTable = majutil::OpDetPhotonTable::Instance();
  majsim::G4Helper*          g4Helper    = majsim::G4Helper::Instance();
  majsim::Configuration*     config      = majsim::Configuration::Instance();

  fEvent        = e;
  fNMPPCs       = g4Helper->GetDetectorConstruction()->WheelGeometry()->NMPPCs();
  fDiskRadius   = g4Helper->GetDetectorConstruction()->WheelGeometry()->Radius()/10; // convert to cm
  fNPrimaries   = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetNPrimaries();

  auto xyzVec = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetSourcePositionXYZ();
  auto rtzVec = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetSourcePositionRTZ();
  fSourcePosXYZ.push_back(xyzVec[0]/CLHEP::cm);  
  fSourcePosXYZ.push_back(xyzVec[1]/CLHEP::cm);  
  fSourcePosXYZ.push_back(xyzVec[2]/CLHEP::cm); 
  fSourcePosRTZ.push_back(rtzVec[0]/CLHEP::cm);  
  fSourcePosRTZ.push_back(rtzVec[1]*180/pi);    // convert to degrees   
  fSourcePosRTZ.push_back(rtzVec[2]/CLHEP::cm);
  
  auto photonsDetected = photonTable->GetPhotonsDetected();
  photonTable->Dump();

  fNPhotonsAbs = photonTable->GetNPhotonsAbsorbed();

  // Get the number of photons detected per sipm
  for (unsigned m = 1; m <= fNMPPCs; m++)
  {
    int photons(0);
    if (photonsDetected.find(m) != photonsDetected.end())
    {
      photons = photonsDetected.find(m)->second.size();
    }
    fMPPCToLY.push_back(photons);

    // Calculate distances to each mppc
    float r        = fSourcePosRTZ[0];
    float thetaDeg = fSourcePosRTZ[1];
    float betaDeg  = (m - 1)*360/fNMPPCs;
    float diffRad = (betaDeg - thetaDeg)*pi/180;

    float R = std::sqrt(r*r + fDiskRadius*fDiskRadius - 2*r*fDiskRadius*std::cos(diffRad));
    float alphaDeg = std::abs(std::asin(r*std::sin(diffRad)/R)*180/pi);
    
    fMPPCToSourceR.push_back(R);
    fMPPCToSourceT.push_back(alphaDeg);
  }
  fAnaTree->Fill();
}

//------------------------------------------------------------------------
void SimAnalyzer::ResetVars()
{
  fEvent   = -99999;
  fPixelID = -99999;
  fNPixels = -99999;
  fNMPPCs  = -99999;
  fDiskRadius = -99999;
  fNPrimaries = -99999;
  fNPhotonsAbs = -99999;
  fSourcePosXYZ.clear();
  fSourcePosRTZ.clear();
  fMPPCToLY.clear();
  fMPPCToSourceR.clear();
  fMPPCToSourceT.clear();
  fNPhotonsAbs = -99999;
}

}

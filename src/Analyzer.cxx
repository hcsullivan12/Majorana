/**
 * @file Analyzer.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Module to produce a ROOT analysis TTree.
 * @date 07-04-2019
 * 
 */

#include "Analyzer.h"
#include "TFile.h"
#include "OpDetPhotonTable.h"
#include "G4Helper.h"
#include "PixelTable.h"
#include "Reconstructor.h"
#include "Configuration.h"

namespace majorana 
{

//------------------------------------------------------------------------
Analyzer::Analyzer(const std::string& simOutputPath)
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
  if (config->Reconstruct())
  {
    fAnaTree->Branch("mlX", &fMLX, "mlX/D");
    fAnaTree->Branch("mlY", &fMLY, "mlY/D");
    //fAnaTree->Branch("mlIntensities", fMLIntensities, "mlIntensities[nPixels]/D");
  }
}

//------------------------------------------------------------------------
Analyzer::~Analyzer()
{
  TFile f(fSimulateOutputPath.c_str(), "UPDATE");
  fAnaTree->Write();
  f.Close();

  if (fAnaTree) delete fAnaTree;
}

//------------------------------------------------------------------------
void Analyzer::Fill(const unsigned& e)
{
  ResetVars();

  // Get the necessary information
  OpDetPhotonTable* photonTable = OpDetPhotonTable::Instance();
  G4Helper* g4Helper = G4Helper::Instance();
  Configuration* config = Configuration::Instance();

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

    //std::cout << "SiPM = " << m << " R = " << R << " T = " << alphaDeg << std::endl;
  }

  // Pixel info
  //PixelTable* pixelTable = PixelTable::Instance();
  //auto thePixelList = pixelTable->GetPixels();
  //fNPixels = thePixelList->size();
  //for (const auto& p : thePixelList)
  //{
  //  fPixelX[p.ID()-1] = p.X();
  //  fPixelY[p.ID()-1] = p.Y();
  //}

  // Fill reconstruction info
  if (config->Reconstruct())
  {
    fMLX = g4Helper->GetReconstructor().X();
    fMLY = g4Helper->GetReconstructor().Y();
  }
  
  fAnaTree->Fill();
}

//------------------------------------------------------------------------
void Analyzer::ResetVars()
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
  fMLX = -99999;
  fMLY = -99999;
  //for (unsigned k = 0; k < kMaxNPixels; k++)
  //{
  //  fMLIntensities[k] = -99999;
  //  fPixelX[k] = -99999;
  //  fPixelY[k] = -99999;
  //}
}

}

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
#include "PixelTable.h"
#include "Reconstructor.h"
#include "Configuration.h"

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
  fAnaTree->Branch("nPixels",    &fNPixels, "nPixels/I");
  fAnaTree->Branch("pixelX", fPixelX, "pixelX[nPixels]/D");
  fAnaTree->Branch("pixelY", fPixelY, "pixelY[nPixels]/D");
  fAnaTree->Branch("nMPPCs",     &fNMPPCs, "nMPPCs/I");
  fAnaTree->Branch("diskRadius", &fDiskRadius, "diskRadius/D");
  fAnaTree->Branch("nPrimaries", &fNPrimaries, "nPrimaries/I");
  fAnaTree->Branch("sourcePosXYZ", fSourcePosXYZ, "sourcePosXYZ[3]/D");
  fAnaTree->Branch("sourcePosRTZ", fSourcePosRTZ, "sourcePosRTZ[3]/D");
  fAnaTree->Branch("mppcToLY", fMPPCToLY, "mppcToLY[nMPPCs]/D");
  fAnaTree->Branch("mppcToSourceR", fMPPCToSourceR, "mppcToSourceR[nMPPCs]/D");
  fAnaTree->Branch("mppcToSourceT", fMPPCToSourceT, "mppcToSourceT[nMPPCs]/D");
  fAnaTree->Branch("nPhotonsAbsorbed", &fNPhotonsAbs, "nPhotonsAbsorbed/I");
  fAnaTree->Branch("mlX", &fMLX, "mlX/D");
  fAnaTree->Branch("mlY", &fMLY, "mlY/D");
  fAnaTree->Branch("mlR", &fMLR, "mlR/D");
  fAnaTree->Branch("mlT", &fMLT, "mlT/D");
  fAnaTree->Branch("mlIntensities", fMLIntensities, "mlIntensities[nPixels]/D");
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
  G4Helper* g4Helper = G4Helper::Instance();
  Configuration* config = Configuration::Instance();

  // Basically we want to look at the light yield as a function of position 
  fEvent        = e;
  fNMPPCs       = g4Helper->GetDetectorConstruction()->WheelGeometry()->NMPPCs();
  fDiskRadius   = g4Helper->GetDetectorConstruction()->WheelGeometry()->Radius()/10; // convert to cm
  fNPrimaries   = g4Helper->GetActionInitialization()->GetGeneratorAction()->GetNPrimaries();
  // Check the number of sipms
  if (fNMPPCs > kMaxNMPPCs) 
  { 
    std::cout << "CAUTION! kMaxNMPPCs is set to "     << kMaxNMPPCs 
              << ", but nMPPCs in config is set to " << fNMPPCs 
              << ". Please increase kMaxNMPPCs.\n";
              exit(1);
  }

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
  PixelTable* pixelTable = PixelTable::Instance();
  fNPixels = pixelTable->GetPixels().size();
  for (const auto& p : pixelTable->GetPixels())
  {
    fPixelX[p.ID()-1] = p.X();
    fPixelY[p.ID()-1] = p.Y();
  }

  // Fill reconstruction info
  Reconstructor reconstructor = g4Helper->GetReconstructor();
  if (config->Reconstruct())
  {
    fMLX = reconstructor.X();
    fMLY = reconstructor.Y();
    fMLR = reconstructor.R();
    fMLT = reconstructor.Theta();
    std::vector<float> pixelEstimates = reconstructor.PixelEstimates();
    for (unsigned k = 0; k < pixelEstimates.size(); k++)
    {
      fMLIntensities[k] = pixelEstimates[k];
    }
  }
  
  fAnaTree->Fill();
}

void Analyzer::ResetVars()
{
  fEvent   = -99999;
  fPixelID = -99999;
  fNPixels = -99999;
  fNMPPCs  = -99999;
  fDiskRadius = -99999;
  fNPrimaries = -99999;
  fNPhotonsAbs = -99999;
  fSourcePosXYZ[0] = -99999; fSourcePosXYZ[1] = -99999; fSourcePosXYZ[2] = -99999;
  fSourcePosRTZ[0] = -99999; fSourcePosRTZ[1] = -99999; fSourcePosRTZ[2] = -99999;
  for (unsigned k = 0; k < kMaxNMPPCs; k++)
  {
    fMPPCToLY[k]      = -99999;
    fMPPCToSourceR[k] = -99999;
    fMPPCToSourceT[k] = -99999;
  }
  fNPhotonsAbs = -99999;
  fMLX = -99999;
  fMLY = -99999;
  fMLR = -99999;
  fMLT = -99999;
  for (unsigned k = 0; k < kMaxNPixels; k++)
  {
    fMLIntensities[k] = -99999;
    fPixelX[k] = -99999;
    fPixelY[k] = -99999;
  }
}

}

/**
 * @file RecoHelper.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Helper class to interface with simulation output and run reconstruction.
 * @date 07-04-2019
 * 
 */

#include "majreco/RecoHelper.h"
#include "majreco/Configuration.h"
#include "majutil/PixelTable.h"
#include "majreco/Reconstructor.h"
#include "majreco/RecoAnalyzer.h"
#include "majutil/Cfunctions.h"

#include <assert.h>

#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <filesystem>

namespace majreco
{

RecoHelper* RecoHelper::instance = 0;

//------------------------------------------------------------------------

RecoHelper* RecoHelper::CreateInstance()
{
  if (instance == 0)
  {
    static RecoHelper recoHelper;
    instance = &recoHelper;
  }
  return instance;
}

//------------------------------------------------------------------------
RecoHelper* RecoHelper::Instance()
{
  assert(instance);
  return instance;
}

//------------------------------------------------------------------------
RecoHelper::RecoHelper() 
{}

//------------------------------------------------------------------------
RecoHelper::~RecoHelper()
{

}
//---------------------------------
    void RecoHelper::getSIPMGains (string Path,vector<float> &Gains)
    {
        ifstream file(Path);
        string str;
        while (getline(file,str))
            Gains.push_back(stof(str));
        if(Gains.size()>0)
            cout<<"SIPM gains are obtained from the file"<<endl;
        else
            cout<<"SIPM gains could not obtained from the file"<<endl;
    }

//------------------------------------------------------------------------
void RecoHelper::Start()
{
  gROOT->Reset();
  // Get config and pixels
  Configuration*       config     = Configuration::Instance();
  majutil::PixelTable* pixelTable = majutil::PixelTable::Instance();

  // Read the tree from simulation
  TFile simFile(config->SimulateOutputPath().c_str(), "READ");
  TTree* tree = (TTree*)simFile.Get("anatree");
  if (!tree) return;
  assert(tree);
  auto nentries = tree->GetEntries();

  // Variables to extract from tree
  TBranch *b_Neutron,*b_Electron,*b_Alpha,*b_Photon;
  int    event;
  int    nPixels;
  int    nSiPMs;
  double diskRadius;
  int    nPrimaries;
  int pressure;
  std::vector<double> *NeutronEnergies=0;
  std::vector<double> *ElectronEnergies=0;
  std::vector<double> *AlphaEnergies=0;
  std::vector<double> *PhotonEnergies=0;
  double TrueX,TrueY;
  std::vector<float> Gains;


    std::vector<float>* sourcePosXYZ = 0;
  std::vector<int>*   sipmToLY     = 0;

  int errorInt = -90000;
  tree->SetBranchAddress("event",        &event);
  tree->SetBranchAddress("nPixels",      &nPixels);
  tree->SetBranchAddress("nMPPCs",       &nSiPMs);
  tree->SetBranchAddress("diskRadius",   &diskRadius);
  tree->SetBranchAddress("nPrimaries",   &nPrimaries);
  tree->SetBranchAddress("sourcePosXYZ", &sourcePosXYZ);
  tree->SetBranchAddress("mppcToLY",     &sipmToLY);
  tree->SetBranchAddress("Pressure",     &pressure);

  tree->SetBranchAddress("NeutronE",  &NeutronEnergies);
  tree->SetBranchAddress("PhotonE", &PhotonEnergies);
  tree->SetBranchAddress("AlphaE", &AlphaEnergies);
  tree->SetBranchAddress("ElectronE", &ElectronEnergies);

  tree->SetBranchAddress("TrueX",&TrueX);
  tree->SetBranchAddress("TrueY",&TrueY);

  // Create RecoAnatree object
  RecoAnalyzer analyzer;
    std::string hname;

    int EventID=0;
    majutil::Cfunctions fun;
    //Define Static Parameters for Reconstruction
    fun.EventID=EventID;
    fun.ConfigUnpanilized=config->UnpenalizedStopId();
    fun.PixelTable=pixelTable;

    fun.FilePath=config->GetPathToSavePics()+"/";


    fun.RecoDataFilePath=config->GetRecoDataFilePath();
    fun.DeadChannelPath=config->GetPathToDeadChannels();
    fun.f=analyzer.OutputFile();
    fun.ana=&analyzer;
    fun.CountEvents=0;
    //Getting Dead Channels if they Exist
    if(fun.DeadChannelPath!="None" || fun.DeadChannelPath!="")
        fun.DeadChs(fun.DeadChannels);
    // Loop over all entries in the tree from simulation.
  for (auto entry = 0; entry < nentries; entry++)
  {
    std::cout << "\nReconstructing event " << entry << std::endl;
    tree->GetEntry(entry);

    //Get Gains
      if(nSiPMs<32 and entry==0 and Gains.size()!=0)
          getSIPMGains("/media/ilker/DATA/Project/sipm_wheel/Argon/SmallWheelGains.txt",Gains);
      else if(nSiPMs>32 and entry==0 and Gains.size()!=0)
          getSIPMGains("/media/ilker/DATA/Project/sipm_wheel/Argon/sipmGains.txt",Gains);
    /*
      hname="DataX_"+std::to_string(entry);
      TH1F *DataX = new TH1F (hname.c_str(), "DataX;NumbofEvents;Position (cm)", 10000, -diskRadius-0.5, diskRadius+0.5);
      hname="DataY_"+std::to_string(entry);
      TH1F *DataY = new TH1F (hname.c_str(), "DataX;NumbofEvents;Position (cm)", 10000, -diskRadius-0.5, diskRadius+0.5);

      hname="SimX_"+std::to_string(entry);
      TH1F *SimX = new TH1F (hname.c_str(), "SimX;NumbofEvents;Position (cm)", 10000, -diskRadius-0.5, diskRadius+0.5);
      hname="SimY_"+std::to_string(entry);
      TH1F *SimY = new TH1F (hname.c_str(), "SimX;NumbofEvents;Position (cm)", 10000, -diskRadius-0.5, diskRadius+0.5);
    */

    // Make sure the values make sense first
    assert(nSiPMs > 0 && diskRadius > 0 && nPrimaries > 0);
    for (auto& p : *sourcePosXYZ) assert(p > errorInt);
    for (auto& p : *sipmToLY)     assert(p > errorInt);

    // Store data in more convienent format
    std::map<size_t, size_t> data;
    size_t sipmId(1);
    size_t DataVal;


      for (size_t d = 1; d <= sipmToLY->size(); d++) {
          if(std::find(fun.DeadChannels.begin(),fun.DeadChannels.end(),d-1)==fun.DeadChannels.end()){

            if(Gains.size()==0)
                DataVal=(*sipmToLY)[d-1];
            else
                DataVal=(*sipmToLY)[d-1]/Gains.at(d-1);
          } else DataVal=0;
          data.emplace(d, DataVal);
    }

    // Initialize our reconstruction algorithm 
    Reconstructor reconstructor(data, pixelTable->GetPixels(), diskRadius,entry);
    //reconstructor.DoEmMl(config->Gamma(),
    //                     config->UnpenalizedStopId(),
    //                     config->PenalizedStopId(),
    //                     config->DoPenalized());
 
    reconstructor.DoChi2(config->UnpenalizedStopId());
      // Creating our combined image and assigning the variables


      //Define Reconstruction changing variables
      fun.Radius=(float)diskRadius;
      if (diskRadius>16)
          fun.dif=35;

      fun.Title="Event_"+std::to_string(entry)+"_";
      //fun.SimX=SimX;
      //fun.SimY=SimY;
      //fun.DataX=DataX;
      //fun.DataY=DataY;
      fun.SimGausFit=reconstructor.getGaus();
      //fun.TrueFilePath=config->TrueFilePath();
      fun.FileName=config->FileName();
      fun.SimEvents=entry;
      fun.SimTotalEvents=nentries;
      fun.hmli2D=reconstructor.MLImage();
      fun.Measured=reconstructor.MeasuredImage();
      fun.hchi2D=reconstructor.Chi2Image();
      fun.NSIPMs=nSiPMs;

      fun.TrueX=TrueX;
      fun.TrueY=TrueY;
      fun.fSimReco=&reconstructor;
      fun.ExpX=reconstructor.X();
      fun.ExpY=reconstructor.Y();

      fun.nPrimaries=nPrimaries;
      fun.nPixels=nPixels;
      fun.TotalSimLight=reconstructor.TotalLight();
      fun.sourcePosXYZ=sourcePosXYZ;
      fun.sipmToLY=sipmToLY;
      fun.KSimData=reconstructor.GetSimData();

      if (reconstructor.TotalLight()>0){
          if(fun.RecoDataFilePath=="None" || fun.RecoDataFilePath=="")
            fun.DrawCircle();
          else{

              fun.DrawSuperImposedCircle();
          }
      }
      fun.CountEvents++;
      fun.f->cd();
      //reconstructor.Dump();


      std::vector<float> recoPos = {reconstructor.X(), reconstructor.Y(), 1.0};
      std::vector<float> TruePos = {(float)TrueX, (float)TrueY, 1.0};

    if(fun.RecoDataFilePath=="None" || fun.RecoDataFilePath=="")
        analyzer.Fill(entry,
                  nPixels,
                  diskRadius,
                  nPrimaries,
                  *sourcePosXYZ,
                  *sipmToLY,
                  reconstructor.TotalLight(),
                  recoPos,
                  TruePos);

  /*
  DataX->Write();
  DataY->Write();
  SimX->Write();
  SimY->Write();
   */

  //delete DataX,DataY,SimX,SimY;


  }

    fun.DeadChannels.clear();

}

}

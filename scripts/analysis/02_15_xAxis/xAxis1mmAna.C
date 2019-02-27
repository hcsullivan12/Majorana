#define xAxis1mmAna_cxx
#include "xAxis1mmAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Ana();

std::map<unsigned, std::map<float, std::vector<float>> > theMap;
unsigned sipmA   = 1;
unsigned sipmB   = 9;
unsigned nEvents = 25; 

//*******************************************
void xAxis1mmAna::Loop()
{
  // Prepare map!
  std::map<float, std::vector<float>> m;
  theMap.emplace(sipmA, m);
  theMap.emplace(sipmB, m);

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    // Store all points according to position
    for (unsigned s = 1; s <= nMPPCs; s++)
    {
      if (mppcToSourceT[s-1] > 0.01) continue;
      if (s!=sipmA && s!=sipmB) continue;

      // Store for (r, t) = (r, 0) plot
      float r  = mppcToSourceR[s-1];
      float t  = mppcToSourceT[s-1];
      float R  = sourcePosRTZ[0];
      float T  = sourcePosRTZ[1];
      float ly = mppcToLY[s-1];
      if (r < 0.2) continue;

      std::vector<float> v = {ly};

      auto sipmIter = theMap.find(s);      
      auto rIter    = sipmIter->second.find(r);

      if (rIter == sipmIter->second.end()) sipmIter->second.emplace(r, v);
      else rIter->second.push_back(ly);
    }
  }

  for (const auto& sipmIter : theMap) 
  {
     //for (const auto& rIter : sipmIter.second) cout << "s = " << sipmIter.first << "  r = " << rIter.first << endl;
  }

  Ana();
}

//*************************************************
void Ana()
{
  // Get mean and std
  std::vector<float> xA, yA, stdevA;
  for (const auto& rIter : theMap.find(sipmA)->second)
  {
    TGraph gA(rIter.second.size());
    unsigned point(0);
    for (const auto& ly : rIter.second) {gA.SetPoint(point, ly, 0); point++;}
    xA.push_back(rIter.first);
    yA.push_back(gA.GetMean());
    stdevA.push_back(gA.GetRMS());
    //std::cout << "r = " << rIter.first << " " << gA.GetMean() << " " << gA.GetRMS() << std::endl;
  }
  // Get mean and std
  std::vector<float> xB, yB, stdevB;
  for (const auto& rIter : theMap.find(sipmB)->second)
  {
    TGraph gB(rIter.second.size());
    unsigned point(0);
    for (const auto& ly : rIter.second) {gB.SetPoint(point, ly, 0); point++;}
    xB.push_back(rIter.first);
    yB.push_back(gB.GetMean());
    stdevB.push_back(gB.GetRMS());
    //std::cout << "r = " << rIter.first << " " << gB.GetMean() << " " << gB.GetRMS() << std::endl;
  }

  TGraphErrors* plotA = new TGraphErrors(xA.size(), &xA[0], &yA[0], 0, 0);//&stdevA[0]); 
  plotA->SetMarkerStyle(8);
  plotA->SetMarkerSize(0.8);
  plotA->SetMarkerColor(kBlue);
  plotA->Draw("AP");
  TGraphErrors* plotB = new TGraphErrors(xB.size(), &xB[0], &yB[0], 0, 0);//&stdevB[0]); 
  plotB->SetMarkerStyle(8);
  plotB->SetMarkerSize(0.8);
  plotB->SetMarkerColor(kRed);
  plotB->Draw("P same");

  std::string den = "1 + [1]*[1]*x*x";
//  std::string eq = "[0]/TMath::Power("+den+", 1.5) + [2]*xA + [3]";
  std::string eq = "[0]/(1+[1]*xA) + [2]*x + [3]";

  TF1* fit = new TF1("fit", "(1/x)*(pol 9)", 1.5, 29);
  plotA->Fit(fit, "R");
  fit->SetLineWidth(4);
  fit->SetLineColor(1);
  fit->Draw("L same");
  //gStyle->SetOptFit(1);

 /* TF1* fit = new TF1("fit", eq.c_str(), 0.4, 25);
  fit->SetParameters(400, 0.2, 0.5, 2);
  //fit->SetParameter(1, -.5);
  g->Fit(fit, "R");
  fit->Draw("L same");*/
  
}

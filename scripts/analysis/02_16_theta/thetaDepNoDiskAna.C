#define thetaDepNoDiskAna_cxx
#include "thetaDepNoDiskAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Ana();

std::map<unsigned, std::map<double, std::vector<double>> > theMap;
unsigned sipmID   = 5;
unsigned nEvents = 25; 

//*******************************************
void thetaDepNoDiskAna::Loop()
{
  // Prepare map!
  std::map<double, std::vector<double>> m;
  theMap.emplace(sipmID, m);

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
      //if (mppcToSourceT[s-1] > 0.01) continue;
      if (s!=sipmID) continue;

      // Store for (r, t) = (r, 0) plot
      double r  = mppcToSourceR[s-1];
      double t  = std::round(mppcToSourceT[s-1]);
      double R  = sourcePosRTZ[0];
      double T  = sourcePosRTZ[1];
      double ly = mppcToLY[s-1];
      //if (r < 0.2) continue;

      std::vector<double> v = {ly};

      auto sipmIter = theMap.find(s);      
      auto tIter    = sipmIter->second.find(t);

      if (tIter == sipmIter->second.end()) sipmIter->second.emplace(t, v);
      else tIter->second.push_back(ly);
    }
  }

  for (const auto& sipmIter : theMap) 
  {
     for (const auto& tIter : sipmIter.second) cout << "  t = " << tIter.first << endl;
  }

  Ana();
}

//*************************************************
void Ana()
{
  // Get mean and std
  std::vector<double> xA, yA, stdevA;
  for (const auto& tIter : theMap.find(sipmID)->second)
  {
    TGraph gA(tIter.second.size());
    std::cout << tIter.second.size() << std::endl;
    unsigned point(0);
    for (const auto& ly : tIter.second) {gA.SetPoint(point, ly, 0); point++;}
    xA.push_back(tIter.first);
    yA.push_back(gA.GetMean());
    stdevA.push_back(gA.GetRMS());
    //std::cout << "t = " << tIter.first << " " << gA.GetMean() << " " << gA.GetRMS() << std::endl;
  }

  TGraphErrors* plotA = new TGraphErrors(xA.size(), &xA[0], &yA[0], 0, 0);//&stdevA[0]); 
  plotA->SetMarkerStyle(8);
  plotA->SetMarkerSize(0.8);
  plotA->SetMarkerColor(kBlue);
  plotA->SetMinimum(0);
  plotA->Draw("AP");
 
  TF1* fit = new TF1("fit", "(pol 9)", 0, 70);
  //plotA->Fit(fit, "R");
  fit->SetLineWidth(4);
  fit->SetLineColor(1);
  //fit->Draw("L same");
  //gStyle->SetOptFit(1);

  TF1* cosL = new TF1("cosL", "77*TMath::Cos((20-x)*TMath::Pi()/180)", 0, 70);
  cosL->Draw("same");
  cosL->SetLineWidth(4);
  cosL->SetLineColor(1);
}

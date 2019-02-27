#define thetaDepNoDiskCorrectionAna_cxx
#include "thetaDepNoDiskCorrectionAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Ana();

std::map<unsigned, std::map<double, std::vector<double>> > theMap;
unsigned sipmA   = 5;
unsigned sipmB   = 13;
unsigned nEvents = 25; 

//*******************************************
void thetaDepNoDiskCorrectionAna::Loop()
{
  // Prepare map!
  std::map<double, std::vector<double>> m;
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
      //if (mppcToSourceT[s-1] > 0.01) continue;
      if (s!=sipmA && s!= sipmB) continue;

      double r  = mppcToSourceR[s-1];
      double t  = mppcToSourceT[s-1];
      double R  = sourcePosRTZ[0];
      double T  = sourcePosRTZ[1];
      double ly = mppcToLY[s-1];
      if (r > 8) continue;

      if (s == sipmA) cout << s << " " << mppcToSourceT[s-1] << " " << ly<< endl;

      std::vector<double> v = {ly};

      auto sipmIter = theMap.find(s);      
      auto tIter    = sipmIter->second.find(t);

      if (tIter == sipmIter->second.end()) sipmIter->second.emplace(t, v);
      else tIter->second.push_back(ly);
    }
  }

  for (const auto& sipmIter : theMap) 
  {
    //for (const auto& tIter : sipmIter.second) cout << "s = " << sipmIter.first << "  t = " << tIter.first << " size = " << tIter.second.size() << endl;
  }

  Ana();
}

//*************************************************
void Ana()
{
  // Get mean and std
  std::vector<double> xA, yA, stdevA;
  for (const auto& tIter : theMap.find(sipmA)->second)
  {
    TGraph gA(tIter.second.size());
    double mean(0);
    unsigned point(0);
    for (const auto& ly : tIter.second) {gA.SetPoint(point, ly, 0); point++; mean = mean + ly;}
    mean = mean/tIter.second.size();
    //cout << mean << " " << gA.GetMean() << endl;
    xA.push_back(tIter.first);
    yA.push_back(gA.GetMean());
    stdevA.push_back(gA.GetRMS());
    //std::cout << "t = " << tIter.first << " " << gA.GetMean() << " " << gA.GetRMS() << std::endl;
  }
  std::vector<double> xB, yB, stdevB;
  for (const auto& tIter : theMap.find(sipmB)->second)
  {
    TGraph gB(tIter.second.size());
    double mean(0);
    unsigned point(0);
    for (const auto& ly : tIter.second) {gB.SetPoint(point, ly, 0); point++; mean = mean + ly;}
    mean = mean/tIter.second.size();
    cout << mean << " " << gB.GetMean() << endl;
    xB.push_back(tIter.first);
    yB.push_back(gB.GetMean());
    stdevB.push_back(gB.GetRMS());
    //std::cout << "t = " << tIter.first << " " << gA.GetMean() << " " << gA.GetRMS() << std::endl;
  }
  

  TGraphErrors* plotA = new TGraphErrors(xA.size(), &xA[0], &yA[0], 0, 0);//&stdevA[0]); 
  plotA->SetMarkerStyle(8);
  plotA->SetMarkerSize(0.8);
  plotA->SetMarkerColor(kBlue);
  plotA->SetMinimum(0);
  plotA->Draw("AP");

  TGraphErrors* plotB = new TGraphErrors(xB.size(), &xB[0], &yB[0], 0, 0);//&stdevA[0]); 
  plotB->SetMarkerStyle(8);
  plotB->SetMarkerSize(0.8);
  plotB->SetMarkerColor(kRed);
  plotB->SetMinimum(0);
  plotB->Draw("same P");
 
  TF1* fit = new TF1("fit", "(pol 9)", 0, 70);
  //plotA->Fit(fit, "R");
  fit->SetLineWidth(4);
  fit->SetLineColor(1);
  //fit->Draw("L same");
  //gStyle->SetOptFit(1);

  TF1* cosL = new TF1("cosL", "[0]*TMath::Cos(x*TMath::Pi()/180)", 0, 70);
  cosL->SetParameter(0, 61);
  cosL->Draw("same");
  cosL->SetLineWidth(4);
  cosL->SetLineColor(1);
}

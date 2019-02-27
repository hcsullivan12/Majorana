#define lrfVoxelAna_cxx
#include "lrfVoxelAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Ana();

std::map<float, std::vector<float>> theMap;

void lrfVoxelAna::Loop()
{
  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    // Let's just store all points according to position
    for (unsigned s = 1; s <= nMPPCs; s++)
    {
      if (mppcToSourceT[s-1] > 0.01) continue;
      if (s != 5 && s!=1 && s!=9 && s!=13) continue;

      // Store for (r, t) = (r, 0) plot
      float r  = mppcToSourceR[s-1];
      float t = mppcToSourceT[s-1];
      float R = sourcePosRTZ[0];
      float T = sourcePosRTZ[1];
      float ly = mppcToLY[s-1];
      auto it = theMap.find(r);
      std::vector<float> v = {ly};
      if (it == theMap.end()) theMap.emplace(r, v);
      else it->second.push_back(ly);
    }
  }

  Ana();
}

void Ana()
{
  // Get mean and std
  TGraph hist(50);
  std::vector<float> x, y, stdev;
  for (const auto& p : theMap)
  {
    unsigned point(0);
    for (const auto& x : p.second) {hist.SetPoint(point, x, 0); point++;}
    x.push_back(p.first);
    y.push_back(hist.GetMean());
    stdev.push_back(hist.GetRMS());
    //std::cout << hist.GetMean() << " " << hist.GetRMS() << std::endl;
  }

  TGraphErrors* g = new TGraphErrors(x.size(), &x[0], &y[0], 0, &stdev[0]); 
  g->SetMarkerStyle(8);
  g->SetMarkerSize(1);
  g->Draw("AP");

  std::string den = "1 + [1]*[1]*x*x";
//  std::string eq = "[0]/TMath::Power("+den+", 1.5) + [2]*x + [3]";
  std::string eq = "[0]/(1+[1]*x) + [2]*x + [3]";

  TF1* fit = new TF1("fit", "(1/x)*(pol 9)", 0.3, 29);
  g->Fit(fit, "R");
  fit->Draw("L same");
  //gStyle->SetOptFit(1);

 /* TF1* fit = new TF1("fit", eq.c_str(), 0.4, 25);
  fit->SetParameters(400, 0.2, 0.5, 2);
  //fit->SetParameter(1, -.5);
  g->Fit(fit, "R");
  fit->Draw("L same");*/
}

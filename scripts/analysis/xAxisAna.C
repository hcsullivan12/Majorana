#define xAxisAna_cxx
#include "xAxisAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

TH2S* mppc1Hist = new TH2S("mppc1Hist", "mppc1Hist", 11, 0, 2*14.5, 50, 0, 50);
TH2S* mppc5Hist = new TH2S("mppc5Hist", "mppc5Hist", 11, 0, 2*14.5, 50, 0, 50);

std::map<Double_t, std::vector<unsigned>> mppc1Map, mppc5Map;

unsigned e = 0;

void xAxisAna::Loop()
{
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    e++;
    if (e > 50) e = 1;

    float mppc1Dist  = mppcToSourceR[0];
    float mppc5Dist  = mppcToSourceR[4];
    unsigned mppc1LY = mppcToLY[0];
    unsigned mppc5LY = mppcToLY[4];

    if (mppc1Map.find(mppc1Dist) == mppc1Map.end())
    {
      std::vector<unsigned> vec = {static_cast<unsigned int>(mppc1LY)};
      mppc1Map.emplace(mppc1Dist, vec);
    }
    else mppc1Map.find(mppc1Dist)->second.push_back(mppc1LY);
    if (mppc5Map.find(mppc5Dist) == mppc5Map.end())
    {
      std::vector<unsigned> vec = {static_cast<unsigned int>(mppc5LY)};
      mppc5Map.emplace(mppc5Dist, vec);
    }
    else mppc5Map.find(mppc5Dist)->second.push_back(mppc5LY);

    /*auto mppc1Xbin = mppc1Hist->GetXaxis()->FindBin(mppc1Dist);
    auto mppc5Xbin = mppc5Hist->GetXaxis()->FindBin(mppc5Dist);
 
    if (mppc1Xbin == 2) 
    {
      std::cout << e << " " << mppc1Dist << " " <<  mppc1LY << std::endl;
    }
    mppc1Hist->SetBinContent(mppc1Xbin, e, mppc1LY);
    mppc5Hist->SetBinContent(mppc5Xbin, e, mppc5LY);*/
  }

/*  TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
  mppc1Hist->Draw("colz");
  TCanvas* c5 = new TCanvas("c5", "c5", 800, 800);
  mppc5Hist->Draw("colz");*/

 
  Int_t n = mppc1Map.size();
  Double_t x1[n], y1[n], ex1[n], ey1[n];
  Double_t x2[n], y2[n], ex2[n], ey2[n];

  unsigned count = 0;
  for (const auto& xbin : mppc1Map)
  {
    std::vector<unsigned> data = xbin.second;
    TH1S h("h", "h", data.size(), 0, data.size());
    for (unsigned b = 1; b <= data.size(); b++) h.SetBinContent(b, data[b-1]);
    // Get min and max
    Double_t min(0), max(0);
    h.GetMinimumAndMaximum(min, max);

    // Fill  
    TH1S hist("hist", "hist", 25, min, max);
    for (unsigned ybin = 1; ybin <= data.size(); ybin++)
    {
      hist.Fill(data[ybin]);
    }
    Double_t mean  = hist.GetMean();
    Double_t stdev = hist.GetStdDev();
  
    x1[count]  = xbin.first;
    y1[count]  = mean;
    ex1[count] = 0;//1.933/2; 
    ey1[count] = 0;//stdev;
    count++;
  }
  count = 0;
  for (const auto& xbin : mppc5Map)
  {
    std::vector<unsigned> data = xbin.second;
    TH1S h("h", "h", data.size(), 0, data.size());
    for (unsigned b = 1; b <= data.size(); b++) h.SetBinContent(b, data[b-1]);
    // Get min and max
    Double_t min(0), max(0);
    h.GetMinimumAndMaximum(min, max);

    // Fill  
    TH1S hist("hist", "hist", 25, min, max);
    for (unsigned ybin = 1; ybin <= data.size(); ybin++)
    {
      hist.Fill(data[ybin]);
    }
    Double_t mean  = hist.GetMean();
    Double_t stdev = hist.GetStdDev();
  
    x2[count]  = xbin.first;
    y2[count]  = mean;
    ex2[count] = 0;//1.933/2; 
    ey2[count] = 0;//stdev;
    count++;
  }
 
  TGraphErrors* mppc1 = new TGraphErrors(n, x1, y1, ex1, ey1);
  TGraphErrors* mppc5 = new TGraphErrors(n, x2, y2, ex2, ey2);

  mppc1->SetLineColor(2);
  mppc1->SetLineWidth(3);
  mppc5->SetLineColor(4);
  mppc5->SetLineWidth(3);
  TCanvas *c1 = new TCanvas("c1", "SiPM Wheel Light Yield", 800, 800);
  mppc1->GetXaxis()->SetTitle("Distance from source (#theta=0)/cm");
  mppc1->GetYaxis()->SetTitle("LY/photons");
  mppc1->SetTitle("SiPM Wheel Light Yield");
  mppc1->Draw("AL");
  mppc5->Draw("same L");

  /*for (unsigned xbin = 1; xbin <= mppc1Hist->GetXaxis()->GetNbins(); xbin++)
  {
    // We need the y projection here
    auto yProjHist = *mppc1Hist->ProjectionY("yProjHist", xbin, xbin);
    auto temp = mppc1Hist->ProjectionY("temp", 2, 2);
    temp->Draw();
    // Get min and max
    Double_t min(0), max(0);
    yProjHist.GetMinimumAndMaximum(min, max);
    // Fill  
    TH1S hist("hist", "hist", 25, min, max);
    for (unsigned ybin = 1; ybin <= yProjHist.GetXaxis()->GetNbins(); ybin++)
    {
      hist.Fill(yProjHist.GetBinContent(ybin));
    }
    Double_t mean  = hist.GetMean();
    Double_t stdev = hist.GetStdDev();
  
    mppc1->SetBinContent(xbin, mean);
    mppc1->SetBinError(xbin, stdev);
  }
  for (unsigned xbin = 1; xbin <= mppc5Hist->GetXaxis()->GetNbins(); xbin++)
  {
    // We need the y projection here
    auto yProjHist = *mppc5Hist->ProjectionY("yProjHist2", xbin, xbin);
    // Get min and max
    Double_t min(0), max(0);
    yProjHist.GetMinimumAndMaximum(min, max);
    // Fill  
    TH1S hist("hist", "hist", 25, min, max);
    for (unsigned ybin = 1; ybin <= yProjHist.GetXaxis()->GetNbins(); ybin++)
    {
      hist.Fill(yProjHist.GetBinContent(ybin));
    }
    Double_t mean  = hist.GetMean();
    Double_t stdev = hist.GetStdDev();
  
    mppc5->SetBinContent(xbin, mean);
    mppc5->SetBinError(xbin, stdev);
  }*/

  //TCanvas *c3 = new TCanvas("c3", "c4", 800, 800);
  //mppc1->Draw();
  //TCanvas *c4 = new TCanvas("c4", "c4", 800, 800);
  //mppc5->Draw("same");

}

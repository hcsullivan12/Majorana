#define Center1000Ana_cxx
#include "Center1000Ana.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

std::vector<TH1S*> histos;
TH1D* finalHisto;

unsigned kMPPCs = 0;

void Center1000Ana::Loop()
{
  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) 
  {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    //std::cout << nPrimaries << std::endl;
    // Initialize histos
    if (jentry == 0) 
    {
      for (unsigned m = 1; m <= nMPPCs; m++)
      {
        std::string name = "mppc" + std::to_string(m);
        histos.push_back(new TH1S(name.c_str(), name.c_str(), 100, 0, 100));
      }
    }
    kMPPCs = nMPPCs;
    for (unsigned m = 1; m <= nMPPCs; m++)
    {
      //std::cout << m << " " << mppcToSourceR[m-1] << "  " << mppcToSourceT[m-1] << std::endl;
      histos[m-1]->Fill(mppcToLY[m-1]);
    }
  }

  /*
  unsigned m = 0;
  for (auto histo : histos)
  {
    std::string name = "c" + std::to_string(m);
    TCanvas *c1 = new TCanvas(name.c_str(), name.c_str(), 800, 800);
    histo->Draw();
    m++;
  }
  */

  finalHisto = new TH1D("finalHisto", "MPPCs LY", kMPPCs, 0, kMPPCs);

  for (unsigned m = 0; m < nMPPCs; m++)
  {
    // Fit the histos and fill
    TF1 fit("fit", "gaus", 0, 100);
    histos[m]->Fit(&fit, "Q");

    finalHisto->SetBinContent(m+1, fit.GetParameter(1));
    finalHisto->SetBinError(m+1, fit.GetParameter(2));
    //std::cout << fit.GetParameter(2) << std::endl;
  }

  finalHisto->SetMinimum(0);
  finalHisto->SetMaximum(60);
  //finalHisto->SetMarkerStyle(8);
  finalHisto->Draw("hist");
}

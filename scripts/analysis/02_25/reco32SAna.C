#define reco32SAna_cxx
#include "reco32SAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

std::vector<Double_t> rDiffVec;

void Ana();

void reco32SAna::Loop()
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) 
   {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     // if (Cut(ientry) < 0) continue;

     Double_t xTrue = sourcePosXYZ[0];
     Double_t yTrue = sourcePosXYZ[1];
     //cout << xTrue << " " << yTrue << endl;

     Double_t min(0);
     unsigned pixel(1);
     for (unsigned p = 1; p <= nPixels; p++)
     {
       if (mlIntensities[p-1] > min) {min = mlIntensities[p-1]; pixel = p;}
     }
     Double_t xReco = pixelX[pixel-1];
     Double_t yReco = pixelY[pixel-1];
     //cout << xReco << " " << yReco << endl;
     //cout << endl;

     Double_t xDiff = xTrue-xReco;
     Double_t yDiff = yTrue-yReco;

     Double_t rDiff = std::sqrt(xDiff*xDiff+yDiff*yDiff);
     if (rDiff > 10) cout << event << " " << xTrue << " " << yTrue << " " << xReco << " " << yReco << endl;
     rDiffVec.push_back(rDiff);
   }
   Ana();
}

void Ana()
{
  TH1S* h = new TH1S("h21", "h21", 40, 0, 20);
  for (unsigned p = 0; p < rDiffVec.size(); p++)
  {
    //cout << rDiffVec[p] << endl;
    h->Fill(rDiffVec[p]);
  }
  for (unsigned b = 1; b <= h->GetNbinsX(); b++)
  {
    //h->SetBinContent(b, h->GetBinContent(b)/h->GetXaxis()->GetBinCenter(b));
  }

  h->Draw();
}

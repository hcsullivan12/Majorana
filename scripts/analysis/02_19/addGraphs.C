void addGraphs()
{
  TFile* f1 = new TFile("thetaDepWithDiskCorrection.root", "READ");
  TFile* f2 = new TFile("thetaDepNoDiskCorrection.root", "READ");  

  TCanvas* c1 = nullptr;
  TCanvas* c2 = nullptr;

  f1->GetObject("c1", c1);
  f2->GetObject("c1", c2);

  auto primL1 = c1->GetListOfPrimitives();
  auto primL2 = c2->GetListOfPrimitives();

  auto g1 = primL1->At(1);
  auto g2 = primL1->At(2);
  auto g3 = (TGraphErrors*)primL2->At(1);
  auto g4 = (TGraphErrors*)primL2->At(2);
  auto fit = primL2->At(3);

  // subtract 
  Double_t* xs3 = g3->GetX();
  Double_t* ys3 = g3->GetY();
  for (unsigned p = 0; p < g3->GetN(); p++) { g3->SetPoint(p, xs3[p], ys3[p]-10); }
  Double_t* xs4 = g4->GetX();
  Double_t* ys4 = g4->GetY();
  for (unsigned p = 0; p < g4->GetN(); p++) { g4->SetPoint(p, xs4[p], ys4[p]-10); } 

  TCanvas* c4 = new TCanvas("c4","c4", 800, 800);
  g1->Draw("AP");
  g2->Draw("same P");
  g3->Draw("same P");
  g4->Draw("same P");
  fit->Draw("same");
}

void addGraphs()
{
  TFile* f1 = new TFile("thetaDepWithDisk.root", "READ");
  TFile* f2 = new TFile("thetaDepNoDisk.root", "READ");  

  TCanvas* c1 = nullptr;
  TCanvas* c2 = nullptr;

  f1->GetObject("c1", c1);
  f2->GetObject("c1", c2);

  TGraphErrors *g1 = (TGraphErrors*)c1->GetListOfPrimitives()->FindObject("Graph");
  TGraphErrors *g2 = (TGraphErrors*)c2->GetListOfPrimitives()->FindObject("Graph");

  g1->Draw();
  //g2->Draw("same");
}

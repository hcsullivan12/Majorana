void getGraph()
{
  TFile *f = new TFile("../output/simulateOutput.root");
  TGraph *g = nullptr;
  f->GetObject("Graph", g);

  if (!g) std::cout << "Error" << std::endl;

  g->SetMarkerStyle(8);
  g->SetMarkerSize(0.5);
  g->SetMinimum(-15.5);
  g->SetMaximum(15.5);
  g->GetXaxis()->SetLimits(-15.5,  15.5);
  g->Draw("AP");
  TF2 *c = new TF2("c", "x*x + y*y - 14.5*14.5", -15.5, 15.5, -15.5, 15.5);
  double contours[1] = {0};
  c->SetContour(1, contours);
  c->Draw("cont3 same"); 
}

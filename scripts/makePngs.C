void makePngs()
{
  TFile* f = new TFile("../output/recoAnaTree.root");
  std::vector<TH2F*> histos = {101, nullptr};
 
  for (unsigned count = 1; count <= 101; count++)
  {
    std::string name = "hist"+std::to_string(count);
    std::cout << name << std::endl;
    f->GetObject(name.c_str(), histos[count-1]);
    if (!histos[count-1]) std::cout << "Error\n";
  }

  for (unsigned count = 1; count <=101; count++)
  {
    std::string name = "canvas"+std::to_string(count);
    TCanvas* c = new TCanvas(name.c_str(), name.c_str(), 800, 800);
    histos[count-1]->Draw("colz");
    std::string name2 = "/home/hunter/projects/Majorana/output/" + name+ ".png";
    c->SaveAs(name2.c_str());
    c->Close();
    gSystem->ProcessEvents();
  }
}

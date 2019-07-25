#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

const std::vector<unsigned> febChannelsVec = {12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
const std::vector<unsigned> sipmIDVec      = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
//const std::vector<float>    sipmGains      = {57.72,60.21,59.33,59.16,60.03,58.42,59.77,58.08,58.91,62.64,50.51,48.67,37.07,37.78,25,18.46};
const std::vector<float>    sipmGains      = {60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60};
const std::vector<float>    sipmPed        = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
const unsigned ledTriggers = 5000;

std::vector<std::vector<Double_t>> param;
std::vector<TH1F> histVec;
std::string theOutputPath;
std::string theTreePath;
std::string theEvdPath;

void initHistos();
void ana();

//------------------------------------------------------------------------
void doAnalyze(std::string treepath, std::string outputpath, std::string evdpath)
{
  std::cout << "Running doAnalyze...\n";

  theTreePath   = treepath;
  theOutputPath = outputpath;
  theEvdPath    = evdpath;
  // Initialize histograms
  initHistos();

  UShort_t chg[32];
  TFile theFile(theTreePath.c_str(), "READ");
  TTree* theTree = (TTree*)theFile.Get("mppc");
  if (!theTree) { std::cout << "WARNING: Couldn't find mppc tree\n"; return; }
  theTree->SetBranchAddress("chg", chg);
  unsigned nentries = theTree->GetEntries();

  std::vector<int> tempCount(febChannelsVec.size(), 0.);
  for (Long64_t jentry=0; jentry<nentries; jentry++) 
  {
    theTree->GetEntry(jentry);
   
    // Fill the histos
    unsigned counter(0);
    for (const auto& febID : febChannelsVec)
    {
      histVec[counter].Fill(chg[febID]);
      tempCount[counter] += chg[febID];
      counter++;
    }
  }

  // Plotting/fitting
  std::cout << "Running ana...\n";
  bool isZero = false;
  bool isTooLarge = false;
  long int thresh = 10000000000000;
  for (const auto& c : tempCount) 
  {
    isZero     = c == 0 ? true : false;
    isTooLarge = c > thresh ? true : false; 
  }
  if (isTooLarge) cout << "too  Large\n";
  if (!isZero && !isTooLarge) ana();
  gROOT->cd("Rint:/");
  theTree->Reset();
  param.clear();
  histVec.clear();
}

//------------------------------------------------------------------------
void initHistos()
{
  histVec.clear();
  for (const auto& s : sipmIDVec)
  {
    std::string name = "SiPM"+std::to_string(s)+"_hist";
    TH1F* h = ((TH1F*)(gROOT->FindObject(name.c_str())));
    if (h) delete h;

    histVec.push_back( TH1F(name.c_str(), name.c_str(), 200, 0, 2000) );
  }
}

//------------------------------------------------------------------------
void ana()
{
  gStyle->SetOptFit(1);
  gROOT->SetBatch(kTRUE);

  // Draw
  TCanvas* c = ((TCanvas*)(gROOT->FindObject("SiPM Canvas")));
  if (c) delete c;
  TCanvas c1("SiPM Canvas", "SiPM Spectra", 1000, 1000);
  c1.Divide(4,4);
  
  unsigned counter(0);
  for (auto& h : histVec)
  {
    c1.cd(counter+1);

    float mean = h.GetMean();
    float sig  = h.GetStdDev();

    TF1 fit("fit", "gaus", mean - sig, mean + sig);
    fit.SetParameters(1000, mean, sig);
    h.Fit(&fit, "RQ");
    std::vector<Double_t> temp = {fit.GetParameter(0), fit.GetParameter(1), fit.GetParameter(2)};
    param.push_back(temp);

    h.SetMarkerStyle(8);
    h.SetMarkerSize(1);
    h.Draw("pe1");

    counter++;
  }

  // save results of fitting
  std::cout << "Saving results from doAnalyze...\n";
  TFile f(theOutputPath.c_str(), "RECREATE");
  c1.Write();
  f.Close();

  // find area within n*sigma of mean
  counter = 0;
  std::vector<unsigned> theCounts;
  for (const auto& p : param)
  {
    TF1 gaus("fit", "gaus");
    gaus.SetParameters(p[0]/ledTriggers, 0., p[2]);

    Double_t theArea = gaus.Integral(-2*p[2], 2*p[2]);

    // How many photons does the mean correspond to?
    unsigned n = p[1]/sipmGains[counter];
    
    theCounts.push_back(theArea*n);
    //cout<<theArea*n<<std::endl;
    //cout << std::round(theArea/sipmGains[counter]) << endl;
    counter++;
  }

  // Update the file for evd
  std::cout << "Updating daq file for event display...\n";
  std::fstream outfile(theEvdPath, ios::in | ios::out | ios::trunc);
  if(outfile.is_open())
  {
    for (const auto &c : theCounts)
    {
      outfile << c << " ";
    }
  }
  outfile.close();
}

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

const std::vector<unsigned> febChannelsVec = { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 };
const std::vector<unsigned> sipmIDVec      = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 };
const std::vector<float>    sipmGains      = {57.72,60.21,59.33,59.16,60.03,58.42,59.77,58.08,58.91,62.64,50.51,48.67,37.07,37.78,25,18.46};
const std::vector<float>    sipmPed        = {  45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45 };
//const std::vector<unsigned> sipmIDVec      = { 1,  2,  3,  4 };
//const std::vector<unsigned> febChannelsVec = { 16, 17, 18, 19 };
//const std::vector<float>    sipmGains      = {64.0, 64.0, 64.0, 64.0};
//const std::vector<float>    sipmPed        = { 50.8, 50.8, 50.8, 50.8};
const unsigned ledTriggers = 30000;
Int_t increment =1000;
Int_t id=1;


std::vector<TH1F> histVec;
std::string theOutputPath;
std::string theTreePath;
std::string theFileName;



void InitializeHistos();
void Ana(std::vector<Double_t> param,TFile *f);
void Update(std::vector<unsigned>,TFile *f );

/**********************/
void doAnalyze_v2(std::string inputpath, std::string outputpath, std::string filename)
{
  theTreePath   = inputpath;
  theOutputPath = outputpath;
  theFileName   = filename;

  // Initialize histograms
  //InitializeHistos();

  UShort_t chg[32];
  TFile* theFile = new TFile(theTreePath.c_str(), "READ");
  TTree* theTree = (TTree*)theFile->Get("mppc");
  theTree->SetBranchAddress("chg", chg);
  unsigned nentries = theTree->GetEntries();

  TFile * f= new TFile (theOutputPath.c_str(),"UPDATE");

  increment=nentries;
for (Long64_t kentry=0; kentry<nentries; kentry+=increment) {

    /*unsigned counter(0);
    for (const auto &febID : febChannelsVec) {
        histVec[counter].Reset("ICESM");
        counter++;
    }*/


    unsigned events(0);
    std::cout<<"Computing " << nentries << std::endl;
    for (Long64_t jentry = kentry; jentry < increment; jentry++)
    {
        theTree->GetEntry(jentry);

        std::vector<Double_t> param;
        // Fill the histos
        unsigned counter(0);
        for (const auto &febID : febChannelsVec)
        {

            //histVec[counter].Fill(chg[febID]);
            param.push_back(chg[febID]);
            counter++;


        }
        events++;
        //std::cout<< events  << " Done!" <<std::endl;
        Ana(param,f);
        id++;

    }
    std::cout<< events  << " Done!" <<std::endl;
    f->Close();




}
  // Plotting/fitting

  gApplication->Terminate(0);
}

/**********************/
void InitializeHistos()
{
  for (const auto& s : sipmIDVec)
  {
    std::string name = "SiPM"+std::to_string(s)+"_hist";
    histVec.push_back( TH1F(name.c_str(), name.c_str(), 200, 0, 2000) );
  }
}




/**********************/
void Ana(std::vector<Double_t> param, TFile * f)
{

  //gStyle->SetErrorX(0);
  gStyle->SetOptFit(1);

  // Draw

  /*TCanvas c1(theFileName.c_str(), "SiPM Spectra", 1000, 1000);
  c1.Divide(4,2);

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
  }*/

  //TFile f(theOutputPath.c_str(), "UPDATE");
  //c1.Write();
  //f.Close();

  // find area within 1 sigma of mean
  unsigned counter (0);
  std::vector<unsigned> theCounts;
  //for (const auto& p : param)
  for(int p =0;p<param.size();p++)
  {
    //TF1 gaus("fit", "gaus");
    //gaus.SetParameters(p[0]/ledTriggers, 0., p[2]);
    //gaus.SetParameters(p[0]/1000, 0., p[2]);

    //Double_t theArea = gaus.Integral(-2*p[2], 2*p[2]);

    // How many photons does the mean correspond to?
    unsigned n = param[p]/sipmGains[counter];
    //cout << std::round(theArea*n) << endl;
    //theCounts.push_back(theArea*n);
    theCounts.push_back(n);

    counter++;
  }
  Update(theCounts,f);
}

/**********************/
void Update(std::vector<unsigned> theCounts,TFile *f)
{
  // TEMPORARY FIX....
    Int_t eventID = std::stoi(theFileName.substr(2));
     //Int_t eventID=1;
    // Organize
  Int_t nsipms = theCounts.size();
  Int_t data[nsipms];
  for (unsigned i = 0; i < nsipms; i++)
  {
    unsigned thisSipmID = i+1;
    unsigned pos = std::distance( sipmIDVec.begin(), std::find(sipmIDVec.begin(), sipmIDVec.end(), thisSipmID) );
    data[i] = theCounts[pos];
  }

    TTree* t = 0;
	t = (TTree*)f->Get("wheel");
  if (!t) 
  {

    t = new TTree("wheel", "wheel");

    t->Branch("eventID",&eventID,"eventID/I");
    t->Branch("nsipms",&nsipms,"nsipms/I");
    t->Branch("counts",data,"data[nsipms]/I");

  }
  else
  {

	  t->SetBranchAddress("eventID", &eventID);
      t->SetBranchAddress("nsipms", &nsipms);
      t->SetBranchAddress("counts", data);

  }

	t->Fill();
	t->Write(t->GetName(), TObject::kWriteDelete);
	t->Reset();
}




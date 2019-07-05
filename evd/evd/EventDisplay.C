#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

#include <sys/stat.h>
#include "doReconstruct.C"

//#include "ev_display_WheelReco.cxx"

class MyMainFrame {
   RQ_OBJECT("MyMainFrame")
private:
   TGMainFrame           *fMain;
   
   TRootEmbeddedCanvas *fCanvas1 = nullptr;
   TRootEmbeddedCanvas *fCanvas2 = nullptr;
   
   TGTextButton         *fStartBut;
   TGTextButton      *fSetParamBut;
   
   TGTextEntry        *fDiskREnt;
   TGTextEntry       *fNsipmsEnt;
   TGTextEntry    *fPixelSizeEnt;

   TGVButtonGroup   *dataTypeButGroup;
   TGCheckButton             *isMCBut;
   TGCheckButton           *isDataBut;

   bool fIsRunning = false;
   double fLastUpdate = 0;
   TTimer *fTimer = nullptr;
   std::string fDataFile = "./daq/data.txt";
   std::string fTopDir;
   double fDiskR  = 14.5;
   int fNsipms    = 64;
   int fPixelSize = 5;
   double fX;
   double fY;
   std::string fDataType = "data";

public:
   MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h, std::string topDir);
   virtual ~MyMainFrame();
   void DoDraw();
   void ChangeStartLabel();
   void StartReco();
   void StopReco();
   void SetParameters();
   bool IsDAQFileModified();
   void HandleTimer();
   const std::map<unsigned, unsigned> ReadDataFile();
   void UpdatePlots(const std::map<unsigned, unsigned>& mydata);
   void SetDataTypeMC();
   void SetDataTypeData();
};

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h, std::string topDir) 
{
   // Create a main frame
   fMain = new TGMainFrame(p,w,h);

   fTopDir = topDir;

   // Create a horizontal frame
   TGHorizontalFrame *hframe1 = new TGHorizontalFrame(fMain,1500,1000);

   // Create vertical frames
   TGVerticalFrame *vframe1 = new TGVerticalFrame(hframe1,400,1000);
   TGVerticalFrame *vframe2 = new TGVerticalFrame(hframe1,800,1000);
   TGVerticalFrame *vframe3 = new TGVerticalFrame(hframe1,400,1000);

   vframe1->Resize(200,1000);
   hframe1->AddFrame(vframe1, new TGLayoutHints(kLHintsLeft | kLHintsExpandY,5,5,30,100));

   // 2nd vertical frame
   fCanvas1 = new TRootEmbeddedCanvas("fCanvas1",vframe2,200,200);   
   vframe2->AddFrame(fCanvas1, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5));   
   fCanvas2 = new TRootEmbeddedCanvas("fCanvas2",vframe2,200,200);
   vframe2->AddFrame(fCanvas2, new TGLayoutHints(kLHintsExpandX | kLHintsCenterX | kLHintsCenterY | kLHintsExpandY,5,5,5,5));
   vframe2->Resize(1100,1000);
   hframe1->AddFrame(vframe2, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsCenterY | kLHintsExpandY, 5,5,20,20) );

   // bold font for labels
   TGGC *fTextGC;
   const TGFont *boldfont = gClient->GetFont("-*-times-bold-r-*-*-18-*-*-*-*-*-*-*");
   if (!boldfont) boldfont = gClient->GetResourcePool()->GetDefaultFont();
   FontStruct_t labelboldfont = boldfont->GetFontStruct();
   GCValues_t   gval;
   gval.fMask = kGCBackground | kGCFont | kGCForeground;
   gval.fFont = boldfont->GetFontHandle();
   gClient->GetColorByName("yellow", gval.fBackground);
   fTextGC = gClient->GetGC(&gval, kTRUE);

   TGHorizontalFrame *vhframe20 = new TGHorizontalFrame(vframe3,400,20);
   TGLabel *title = new TGLabel(vhframe20, "SiPM Wheel\nEvent Display", fTextGC->GetGC(), labelboldfont, kChildFrame);
   vhframe20->AddFrame(title, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 50, 5));
   //title->SetTextColor(blackcolor);

   /* majorana picture */
   TRootEmbeddedCanvas *majPicCanvas = new TRootEmbeddedCanvas("majPicCanvas",vhframe20,100,100);
   vhframe20->AddFrame(majPicCanvas, new TGLayoutHints(kLHintsTop | kLHintsRight,5,5,5,5));
   TImage *etimg = TImage::Open("evd/et.png");
   if (!etimg) {
      printf("Could not find image... \n");
   }
   else {
     etimg->SetConstRatio(0);
     etimg->SetImageQuality(TAttImage::kImgBest);
     TCanvas* c = majPicCanvas->GetCanvas();
     c->cd();
     c->Clear();
     c->SetLineColor(0);
     etimg->Draw("xxx");
     c->Update();
   }
   vframe3->AddFrame(vhframe20, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 50, 5));

   /* Reco Parameters */
   TGGC *fTextGC1;
   const TGFont *regfont = gClient->GetFont("-*-adobe-r-*-*-12-*-*-*-*-*-*-*");
   if (!regfont) {
        regfont = gClient->GetResourcePool()->GetDefaultFont();
   }
   FontStruct_t label1font = regfont->GetFontStruct();
   GCValues_t   gval1;
   gval1.fMask = kGCBackground | kGCFont | kGCForeground;
   gval1.fFont = regfont->GetFontHandle();
   gClient->GetColorByName("black", gval1.fBackground);
   fTextGC1 = gClient->GetGC(&gval1, kTRUE);
   //gClient->GetColorByName("black", blackcolor);
   TGLabel *tRecoConfig = new TGLabel(vframe3, "Configuration", fTextGC->GetGC(), labelboldfont, kChildFrame);
   vframe3->AddFrame(tRecoConfig, new TGLayoutHints(kLHintsCenterX, 5,5,150,5));

   /* Disk Radius */
   stringstream stream;
   stream << fixed << setprecision(2) << fDiskR;
   TGHorizontalFrame *vhframe6 = new TGHorizontalFrame(vframe3,400,20);
   vhframe6->AddFrame(new TGLabel(vhframe6, "Disk Radius: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *diskRBuf = new TGTextBuffer(10);
   diskRBuf->AddText(0, stream.str().c_str());
   fDiskREnt = new TGTextEntry(vhframe6, diskRBuf);
   fDiskREnt->Resize(50, fDiskREnt->GetDefaultHeight());
   fDiskREnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe6->AddFrame(new TGLabel(vhframe6, "cm", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe6->AddFrame(fDiskREnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   vframe3->AddFrame(vhframe6, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* N sipms */  
   TGHorizontalFrame *vhframe10 = new TGHorizontalFrame(vframe3,400,20);
   vhframe10->AddFrame(new TGLabel(vhframe10, "N SiPMs: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *nsipms = new TGTextBuffer(10);
   nsipms->AddText(0, std::to_string(fNsipms).c_str());
   fNsipmsEnt = new TGTextEntry(vhframe10, nsipms);
   fNsipmsEnt->Resize(50, fNsipmsEnt->GetDefaultHeight());
   fNsipmsEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe10->AddFrame(new TGLabel(vhframe10, "  ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe10->AddFrame(fNsipmsEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   vframe3->AddFrame(vhframe10, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* Pixel size */   
   TGHorizontalFrame *vhframe11 = new TGHorizontalFrame(vframe3,400,20);
   vhframe11->AddFrame(new TGLabel(vhframe11, "Pixel size: ", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsLeft, 0,5,5,5));
   TGTextBuffer *pixelSize = new TGTextBuffer(10);
   pixelSize->AddText(0, std::to_string(fPixelSize).c_str());
   fPixelSizeEnt = new TGTextEntry(vhframe11, pixelSize);
   fPixelSizeEnt->Resize(50, fPixelSizeEnt->GetDefaultHeight());
   fPixelSizeEnt->SetFont("-adobe-courier-r-*-*-12-*-*-*-*-*-iso8859-1");
   vhframe11->AddFrame(new TGLabel(vhframe11, "mm", fTextGC1->GetGC(), label1font, kChildFrame), new TGLayoutHints(kLHintsRight, 5,5,5,0));
   vhframe11->AddFrame(fPixelSizeEnt, new TGLayoutHints(kLHintsRight | kLHintsTop,5,5,2,5));
   vframe3->AddFrame(vhframe11, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,5,5));

   /* Set Params */
   fSetParamBut = new TGTextButton(vframe3,"Set Parameters");
   fSetParamBut->Connect("Clicked()","MyMainFrame",this,"SetParameters()");
   vframe3->AddFrame(fSetParamBut, new TGLayoutHints(kLHintsExpandX,5,5,5,5) );

   vframe3->Resize(400,1000);
   hframe1->AddFrame(vframe3, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5,5,5,5) );

   /* Data type buttons */
   dataTypeButGroup = new TGVButtonGroup(vframe3, "Data Type");
   isDataBut = new TGCheckButton(dataTypeButGroup, new TGHotString("Data"));
   isDataBut->Connect("Clicked()","MyMainFrame",this,"SetDataTypeData()");
   isMCBut   = new TGCheckButton(dataTypeButGroup, new TGHotString("MC"));
   isMCBut->Connect("Clicked()","MyMainFrame",this,"SetDataTypeMC()");
   vframe3->AddFrame(dataTypeButGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5) );
   fDataType == "data" ? isDataBut->SetState(kButtonDown) : isMCBut->SetState(kButtonDown);


   /* UTA picture */
   TRootEmbeddedCanvas *utaPicCanvas = new TRootEmbeddedCanvas("utaPicCanvas",vframe3,150,75);
   vframe3->AddFrame(utaPicCanvas, new TGLayoutHints(kLHintsBottom | /*kLHintsExpandX |*/ kLHintsCenterX | kLHintsCenterY,5,5,5,25));
   TImage *img = TImage::Open("evd/utalogo2.jpg");
   if (!img) {
      printf("Could not find image... \n");
   }
   else {
     img->SetConstRatio(0);
     img->SetImageQuality(TAttImage::kImgBest);
     TCanvas* c = utaPicCanvas->GetCanvas();
     c->cd();
     c->Clear();
     c->SetLineColor(0);
     img->Draw("xxx");
     c->Update();
   }

   /* Quit button */
   TGTextButton *quit = new TGTextButton(vframe3,"&Quit", "gApplication->Terminate(0)");
   vframe3->AddFrame(quit, new TGLayoutHints(kLHintsExpandX | kLHintsBottom,5,5,5,200) );

   /* Start button */
   fStartBut = new TGTextButton(vframe3,"Start");
   fStartBut->Connect("Clicked()","MyMainFrame",this,"ChangeStartLabel()");
   vframe3->AddFrame(fStartBut, new TGLayoutHints(kLHintsBottom | kLHintsExpandX ,5,5,5,5));

   TGLabel *tReco = new TGLabel(vframe3, "Reconstruction", fTextGC->GetGC(), labelboldfont, kChildFrame);
   vframe3->AddFrame(tReco, new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 5,5,5,5));

   // Add horizontal to main frame //
   fMain->AddFrame(hframe1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,5,5));
   // Set a name to the main frame //
   fMain->SetWindowName("Event Display");
   // Map all subwindows of main frame //
   fMain->MapSubwindows();
   // Initialize the layout algorithm // 
   fMain->Resize(fMain->GetDefaultSize());
   // Map main frame //
   fMain->MapWindow();
   fMain->Resize(1500, 1000);
   if (!fTimer) {
     fTimer = new TTimer();
     fTimer->Connect("Timeout()", "MyMainFrame", this, "HandleTimer()");
   }
   std::cout << std::endl;
}

MyMainFrame::~MyMainFrame() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}

void MyMainFrame::SetDataTypeData()
{
  fDataType = "data";

  if (isMCBut->IsOn()) isMCBut->SetState(kButtonUp);
  isDataBut->SetState(kButtonDown);
}

void MyMainFrame::SetDataTypeMC()
{
  fDataType = "mc";

  if (isDataBut->IsOn()) isDataBut->SetState(kButtonUp);
  isMCBut->SetState(kButtonDown);
}

void MyMainFrame::ChangeStartLabel()
{
  fStartBut->SetState(kButtonDown);

  // If it is not currently running, start it
  if (!fIsRunning) 
  {
    fStartBut->SetText("Stop");
    // Start the time to check for updated daq file
    std::cout << "\n"
              << "Listening for DAQ files...\n";
    if (fTimer) fTimer->Start(1000, kFALSE);
    fIsRunning = true;
  } 
  else 
  {
    std::cout << "Stopping reconstruction...\n";
    fStartBut->SetText("Start");
    if (fTimer) fTimer->Stop();
    fIsRunning = false;
  }
  fStartBut->SetState(kButtonUp);
}

void MyMainFrame::StartReco() 
{
   std::cout << "//////////////////////////////////////////////////////\n";
   std::cout << "\nStarting reconstruction..." << std::endl;
   
   // Not the most efficient, but we will read our DAQ file twice
   auto mydata = ReadDataFile();
   if (mydata.size() != fNsipms) {cout << "\nWarning: Data size not equal to " << fNsipms << "\n"; return;}

   // Now we can pass our data to the reconstructor
   std::string pixelizationPath = fTopDir+"/production/production_v1_1/"+std::to_string(fPixelSize)+"mm/pixelization.txt";
   std::string opRefTablePath   = fTopDir+"/production/production_v1_1/"+std::to_string(fPixelSize)+"mm/"+std::to_string(fNsipms)+"sipms/splinedOpRefTable.txt";
   std::string doRecoCmd = "doReconstruct(\""+pixelizationPath+"\",\""+opRefTablePath+"\", \""+fDataFile+"\", "+std::to_string(fDiskR)+")";
   //cout << doRecoCmd << endl;
   gROOT->ProcessLine(doRecoCmd.c_str()); 

   // Update plots
   UpdatePlots(mydata);
}

const std::map<unsigned, unsigned> MyMainFrame::ReadDataFile() 
{
  // The file should contain the number of photons detected by the sipms
  std::ifstream theFile(fDataFile.c_str());
  std::string line;
  std::map<unsigned, unsigned> v;
  if (theFile.is_open()) {
    std::getline(theFile, line);
    if (!line.size()) return v;
    // this is x and y
    fX = std::stod(line.substr(0, line.find(" "))); 
    fY = std::stod(line.substr(line.find(" ")+1));

    // this is data
    std::getline(theFile, line);
  }

  size_t pos = 0;
  size_t counter = 1;
  std::string delimiter = " ";
  while ((pos = line.find(delimiter)) != std::string::npos) {
    v.emplace(counter, std::stoi(line.substr(0, pos)));
    line.erase(0, pos + delimiter.length());
    counter++;
  }
  if (line.size() > 1) v.emplace(counter, std::stoi(line));
  //for (const auto& d : v) cout << d.first << " " << d.second << endl;
  return v;
}

void MyMainFrame::UpdatePlots(const std::map<unsigned, unsigned>& mydata) 
{
  // Grab the top canvas
  TCanvas *tc = fCanvas1->GetCanvas();
  tc->Clear();
  
  // We need the resulting plot from reco
  TFile f("recoanatree.root", "READ");
  if (f.IsOpen()) {
    gStyle->SetPalette(kDarkBodyRadiator);
    TH2F *recoHist = nullptr;
    f.GetObject("histFinal", recoHist);
    if (recoHist) {
      recoHist->SetTitle("Reconstructed");
      recoHist->GetXaxis()->SetTitle("X [cm]");
      recoHist->GetYaxis()->SetTitle("Y [cm]");
      recoHist->Draw("colz");
      tc->cd();
      tc->Update();
    } else {cout << "\nWARNING: Couldn't find reco hist...\n";}
  } else {cout << "\nWARNING: Couldn't open root file...\n";}
  
  // Grab the bottom canvas
  TCanvas *bc = fCanvas2->GetCanvas();
  
  if (fDataType == "data")
  {
    // Make a histogram of the data
    TH1I *h = new TH1I("h", "Measured Light Yield", mydata.size(), 0.5, mydata.size()+0.5);
    for (const auto& d : mydata) h->SetBinContent(d.first, d.second);
    bc->Clear();
    h->SetLineColor(4);
    h->SetLineWidth(4);
    h->GetXaxis()->SetTitle("SiPM ID");
    //h->SetBarOffset(0.5);
    h->Draw("");
    bc->cd();
    bc->Update();
  }
  if (fDataType == "mc")
  {
    // Grab the true distribution for the simulate output
    // This is not the best solution but it'll do for now
    std::string trueDistPath = "trueDist.root";
    TFile s(trueDistPath.c_str(), "READ");
    if (s.IsOpen()) {
      gStyle->SetPalette(kDarkBodyRadiator);
      TH2I *primHist = nullptr;
      s.GetObject("primHist", primHist);
      if (primHist) {
        primHist->SetTitle("True");
        primHist->GetXaxis()->SetTitle("X [cm]");
        primHist->GetYaxis()->SetTitle("Y [cm]");
        bc->Clear();
        primHist->Draw("colz");
        bc->cd();
        bc->Update();
      } else {cout << "\nWARNING: Couldn't find true distribution...\n";}
    } else {cout << "\nWARNING: Couldn't find simulate output path named \'"+trueDistPath+"\'...\n";}
  }
}

void MyMainFrame::SetParameters()
{
  // Get the current settings
  stringstream stream;
  stream << fixed << setprecision(2) << fDiskR;
  fDiskREnt->SetText(stream.str().c_str());

  fNsipms    = std::stoi(fNsipmsEnt->GetText());
  fPixelSize = std::stoi(fPixelSizeEnt->GetText());

  std::cout << "\n"
            << "Updating parameters...\n"
            << "Disk radius set to:        " << fDiskR     << "\n"
            << "Number of SiPMs set to:    " << fNsipms    << "\n"
            << "Pixel size set to:         " << fPixelSize << "\n"
            << std::endl;
}

bool MyMainFrame::IsDAQFileModified() 
{
  struct stat fileStat;
  int err = stat(fDataFile.c_str(), &fileStat);
  if (err != 0) {
    perror(fDataFile.c_str());
  }
  if (fileStat.st_mtime > fLastUpdate) {
     fLastUpdate = fileStat.st_mtime;
     return true;
  }
  return false;
}

void MyMainFrame::HandleTimer() 
{
   if (IsDAQFileModified()) StartReco();
}

void EventDisplay(std::string topDir) 
{
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(),1500,1000, topDir);
}

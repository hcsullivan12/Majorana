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
   
   TGTextButton         *fStart;
   TGTextButton      *fSetParam;
   
   TGTextEntry        *fDiskREnt;
   TGTextEntry        *fSurfRoughEnt;
   TGTextEntry        *fDiskThEnt;
   TGTextEntry        *fSurfAbsEnt;
   TGTextEntry        *fNsipmsEnt;
   TGTextEntry        *fPixelSizeEnt;

   bool fIsRunning = false;
   double fLastUpdate = 0;
   TTimer *fTimer = nullptr;
   std::string fDataFile = "./daq/data.txt";
   std::string fTopDir;
   double fDiskR     = 14.5;    
   double fDiskTh    = 1.0;
   double fSurfRough = 0.95;
   double fSurfAbs   = 0.03;
   int fNsipms    = 64;
   int fPixelSize = 5;
   double fX;
   double fY;

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
};

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h, std::string topDir) {
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
   vframe3->AddFrame(tRecoConfig, new TGLayoutHints(kLHintsCenterX, 5,5,50,5));

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
   fSetParam = new TGTextButton(vframe3,"Set Parameters");
   fSetParam->Connect("Clicked()","MyMainFrame",this,"SetParameters()");
   vframe3->AddFrame(fSetParam, new TGLayoutHints(kLHintsExpandX,5,5,5,5) );

   vframe3->Resize(400,1000);
   hframe1->AddFrame(vframe3, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5,5,5,5) );

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
   fStart = new TGTextButton(vframe3,"Start");
   fStart->Connect("Clicked()","MyMainFrame",this,"ChangeStartLabel()");
   vframe3->AddFrame(fStart, new TGLayoutHints(kLHintsBottom | kLHintsExpandX ,5,5,5,5));

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

void MyMainFrame::ChangeStartLabel()
{
  // Slot connected to the Clicked() signal.
  // It will toggle labels "Start" and "Stop".

  fStart->SetState(kButtonDown);

  // If it is not currently running, start it
  if (!fIsRunning) {
     fStart->SetText("Stop");
     // Start the time to check for updated daq file
     std::cout << "\n"
               << "Listening for DAQ files...\n";
     if (fTimer) fTimer->Start(1000, kFALSE);
     fIsRunning = true;
  } else {
     std::cout << "Stopping reconstruction...\n";
     fStart->SetText("Start");
     if (fTimer) fTimer->Stop();
     fIsRunning = false;
  }
  fStart->SetState(kButtonUp);
}

void MyMainFrame::StartReco() {
   std::cout << "//////////////////////////////////////////////////////\n";
   std::cout << "\nStarting reconstruction..." << std::endl;
   
   // Not the most efficient, but we will read our DAQ file twice
   auto mydata = ReadDataFile();
   if (mydata.size() != fNsipms) {cout << "\nWarning: Data size not equal to " << fNsipms << "\n" << endl; return;}

   // Now we can pass our data to the reconstructor
   std::string pixelizationPath = fTopDir+"/production/production_v1_1/"+std::to_string(fPixelSize)+"mm/pixelization.txt";
   std::string opRefTablePath   = fTopDir+"/production/production_v1_1/"+std::to_string(fPixelSize)+"mm/"+std::to_string(fNsipms)+"sipms/splinedOpRefTable.txt";
   std::string doRecoCmd = "doReconstruct(\""+pixelizationPath+"\",\""+opRefTablePath+"\", \""+fDataFile+"\", "+std::to_string(fDiskR)+")";
   //cout << doRecoCmd << endl;
   gROOT->ProcessLine(doRecoCmd.c_str()); 

   // Update plots
   UpdatePlots(mydata);
}

const std::map<unsigned, unsigned> MyMainFrame::ReadDataFile() {
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

void MyMainFrame::UpdatePlots(const std::map<unsigned, unsigned>& mydata) {
  // Grab the top canvas
  TCanvas *tc = fCanvas1->GetCanvas();
  
  // We need the resulting plot from reco
  TFile f("recoanatree.root", "READ");
  if (f.IsOpen()) {
    gStyle->SetPalette(kDarkBodyRadiator);
    TH2F *recoHist = nullptr;
    f.GetObject("histFinal", recoHist);
    if (recoHist) {
      recoHist->SetTitle("Reconstructed Position");
      recoHist->GetXaxis()->SetTitle("X [cm]");
      recoHist->GetYaxis()->SetTitle("Y [cm]");
      tc->Clear();
      TPad *pad1 = new TPad("pad1","",0,0,1,1);
      pad1->Draw();
      pad1->cd();

      recoHist->Draw("colz");
      TMarker *t = new TMarker(fX, fY, 29);
      t->SetMarkerSize(4);
      t->SetMarkerColor(7);
      t->Draw("same");
      pad1->Update();
      pad1->Modified();
      tc->cd();
    } else {cout << "Couldn't find reco hist...\n";}
  } else {cout << "Couldn't open root file...\n";}
  
  // Grab the bottom canvas
  TCanvas *bc = fCanvas2->GetCanvas();
  
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

void MyMainFrame::SetParameters() {
  // Get the current settings
  fDiskR     = std::stod(fDiskREnt->GetText());
  fDiskTh    = std::stod(fDiskThEnt->GetText());
  fSurfRough = std::stod(fSurfRoughEnt->GetText());
  fSurfAbs   = std::stod(fSurfAbsEnt->GetText());
  fNsipms    = std::stoi(fNsipmsEnt->GetText());
  fPixelSize = std::stoi(fPixelSizeEnt->GetText());

  std::cout << "\n"
            << "Updating parameters...\n"
            << "Disk radius set to:        " << fDiskR     << "\n"
            << "Disk thickness set to:     " << fDiskTh    << "\n"
            << "Surface roughness set to:  " << fSurfRough << "\n"
            << "Surface absorption set to: " << fSurfAbs   << "\n"
            << "Number of SiPMs set to:    " << fNsipms    << "\n"
            << "Pixel size set to:         " << fPixelSize << "\n"
            << std::endl;
}

bool MyMainFrame::IsDAQFileModified() {
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

void MyMainFrame::HandleTimer() {
   if (IsDAQFileModified()) StartReco();
}

void EventDisplay(std::string topDir) {
  // Popup the GUI...
  new MyMainFrame(gClient->GetRoot(),1500,1000, topDir);
}

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jan 16 19:43:57 2019 by ROOT version 6.14/00
// from TTree anatree/analysis tree
// found on file: /home/hunter/projects/Majorana/production/1000000_probabilities.root
//////////////////////////////////////////////////////////

#ifndef probabilitiesAna_h
#define probabilitiesAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class probabilitiesAna {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event;
   Int_t           nMPPCs;
   Double_t        diskRadius;
   Int_t           nPrimaries;
   Double_t        sourcePosXYZ[3];
   Double_t        sourcePosRTZ[3];
   Double_t        mppcToLY[32];   //[nMPPCs]
   Double_t        mppcToSourceR[32];   //[nMPPCs]
   Double_t        mppcToSourceT[32];   //[nMPPCs]

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_nMPPCs;   //!
   TBranch        *b_diskRadius;   //!
   TBranch        *b_nPrimaries;   //!
   TBranch        *b_sourcePosXYZ;   //!
   TBranch        *b_sourcePosRTZ;   //!
   TBranch        *b_mppcToLY;   //!
   TBranch        *b_mppcToSourceR;   //!
   TBranch        *b_mppcToSourceT;   //!

   probabilitiesAna(TTree *tree=0);
   virtual ~probabilitiesAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef probabilitiesAna_cxx
probabilitiesAna::probabilitiesAna(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/hunter/projects/Majorana/production/1000000_probabilities.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/hunter/projects/Majorana/production/1000000_probabilities.root");
      }
      f->GetObject("anatree",tree);

   }
   Init(tree);
}

probabilitiesAna::~probabilitiesAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t probabilitiesAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t probabilitiesAna::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void probabilitiesAna::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("nMPPCs", &nMPPCs, &b_nMPPCs);
   fChain->SetBranchAddress("diskRadius", &diskRadius, &b_diskRadius);
   fChain->SetBranchAddress("nPrimaries", &nPrimaries, &b_nPrimaries);
   fChain->SetBranchAddress("sourcePosXYZ", sourcePosXYZ, &b_sourcePosXYZ);
   fChain->SetBranchAddress("sourcePosRTZ", sourcePosRTZ, &b_sourcePosRTZ);
   fChain->SetBranchAddress("mppcToLY", mppcToLY, &b_mppcToLY);
   fChain->SetBranchAddress("mppcToSourceR", mppcToSourceR, &b_mppcToSourceR);
   fChain->SetBranchAddress("mppcToSourceT", mppcToSourceT, &b_mppcToSourceT);
   Notify();
}

Bool_t probabilitiesAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void probabilitiesAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t probabilitiesAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef probabilitiesAna_cxx

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 18 15:22:30 2019 by ROOT version 6.14/00
// from TTree anatree/analysis tree
// found on file: /home/hunter/projects/Majorana/production/voxelization1deg30.root
//////////////////////////////////////////////////////////

#ifndef angler_RAna_h
#define angler_RAna_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class angler_RAna {
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

   angler_RAna(TTree *tree=0);
   virtual ~angler_RAna();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef angler_RAna_cxx
angler_RAna::angler_RAna(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/hunter/projects/Majorana/production/voxelization1deg30.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/hunter/projects/Majorana/production/voxelization1deg30.root");
      }
      f->GetObject("anatree",tree);

   }
   Init(tree);
}

angler_RAna::~angler_RAna()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t angler_RAna::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t angler_RAna::LoadTree(Long64_t entry)
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

void angler_RAna::Init(TTree *tree)
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

Bool_t angler_RAna::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void angler_RAna::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t angler_RAna::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef angler_RAna_cxx

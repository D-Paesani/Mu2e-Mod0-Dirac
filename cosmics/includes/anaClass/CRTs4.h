//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun 20 17:54:45 2022 by ROOT version 6.24/06
// from TTree CRT/CRT
// found on file: /Users/dp/Documents/Programmi/ROOT/CRT/data/step4/run275_s4.root
//////////////////////////////////////////////////////////

#ifndef CRTs4_h
#define CRTs4_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class CRTs4 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Long64_t        iTrig;
   Int_t           iScint[2];
   Double_t        Z[2];
   Double_t        X[2];
   Double_t        Q[4];
   Double_t        T[4];

   // List of branches
   TBranch        *b_iTrig;   //!
   TBranch        *b_iScint;   //!
   TBranch        *b_Z;   //!
   TBranch        *b_X;   //!
   TBranch        *b_Q;   //!
   TBranch        *b_T;   //!

   CRTs4(TTree *tree=0);
   virtual ~CRTs4();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef CRTs4_cxx
CRTs4::CRTs4(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/Users/dp/Documents/Programmi/ROOT/CRT/data/step4/run275_s4.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/Users/dp/Documents/Programmi/ROOT/CRT/data/step4/run275_s4.root");
      }
      f->GetObject("CRT",tree);

   }
   Init(tree);
}

CRTs4::~CRTs4()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t CRTs4::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t CRTs4::LoadTree(Long64_t entry)
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

void CRTs4::Init(TTree *tree)
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

   fChain->SetBranchAddress("iTrig", &iTrig, &b_iTrig);
   fChain->SetBranchAddress("iScint", &iScint, &b_iScint);
   fChain->SetBranchAddress("Z", Z, &b_Z);
   fChain->SetBranchAddress("X", X, &b_X);
   fChain->SetBranchAddress("Q", Q, &b_Q);
   fChain->SetBranchAddress("T", T, &b_T);
   Notify();
}

Bool_t CRTs4::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void CRTs4::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t CRTs4::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef CRTs4_cxx

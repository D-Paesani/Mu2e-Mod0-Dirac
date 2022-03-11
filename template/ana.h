//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Mar  5 20:25:19 2022 by ROOT version 6.24/06
// from TTree mod0/mod0
// found on file: ../data/in/anarun33_0.root
//////////////////////////////////////////////////////////

#ifndef ana_h
#define ana_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class ana {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           nrun;
   Int_t           nsubrun;
   Int_t           evnum;
   Int_t           nHits;
   Int_t           isLaser;
   Int_t           iDAQ[19];   //[nHits]
   Int_t           iHit[19];   //[nHits]
   Int_t           iRow[19];   //[nHits]
   Int_t           iCol[19];   //[nHits]
   Int_t           SiPM[19];   //[nHits]
   Int_t           iCry[19];   //[nHits]
   Int_t           iMax[19];   //[nHits]
   Double_t        Qval[19];   //[nHits]
   Double_t        Tval[19];   //[nHits]
   Double_t        Vmax[19];   //[nHits]
   Int_t           nSamples[19];   //[nHits]
   Double_t        wave[19][200];   //[nHits]
   Double_t        tWave[19][200];   //[nHits]
   Double_t        bline[19];   //[nHits]
   Double_t        lognTime[19];   //[nHits]
   Double_t        lognChi2[19];   //[nHits]
   Double_t        templTime[19];   //[nHits]
   Double_t        templChi2[19];   //[nHits]
   Double_t        templFit[19][3];   //[nHits]
   Double_t        templErr[19][3];   //[nHits]

   // List of branches
   TBranch        *b_nrun;   //!
   TBranch        *b_nsubrun;   //!
   TBranch        *b_evnum;   //!
   TBranch        *b_nHits;   //!
   TBranch        *b_isLaser;   //!
   TBranch        *b_iDAQ;   //!
   TBranch        *b_iHit;   //!
   TBranch        *b_iRow;   //!
   TBranch        *b_iCol;   //!
   TBranch        *b_SiPM;   //!
   TBranch        *b_iCry;   //!
   TBranch        *b_iMax;   //!
   TBranch        *b_Qval;   //!
   TBranch        *b_Tval;   //!
   TBranch        *b_Vmax;   //!
   TBranch        *b_nSamples;   //!
   TBranch        *b_wave;   //!
   TBranch        *b_tWave;   //!
   TBranch        *b_bline;   //!
   TBranch        *b_lognTime;   //!
   TBranch        *b_lognChi2;   //!
   TBranch        *b_templTime;   //!
   TBranch        *b_templChi2;   //!
   TBranch        *b_templFit;   //!
   TBranch        *b_templErr;   //!

   ana(TTree *tree=0);
   virtual ~ana();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ana_cxx
ana::ana(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../data/in/anarun33_0.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("../data/in/anarun33_0.root");
      }
      f->GetObject("mod0",tree);

   }
   Init(tree);
}

ana::~ana()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ana::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ana::LoadTree(Long64_t entry)
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

void ana::Init(TTree *tree)
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

   fChain->SetBranchAddress("nrun", &nrun, &b_nrun);
   fChain->SetBranchAddress("nsubrun", &nsubrun, &b_nsubrun);
   fChain->SetBranchAddress("evnum", &evnum, &b_evnum);
   fChain->SetBranchAddress("nHits", &nHits, &b_nHits);
   fChain->SetBranchAddress("isLaser", &isLaser, &b_isLaser);
   fChain->SetBranchAddress("iDAQ", iDAQ, &b_iDAQ);
   fChain->SetBranchAddress("iHit", iHit, &b_iHit);
   fChain->SetBranchAddress("iRow", iRow, &b_iRow);
   fChain->SetBranchAddress("iCol", iCol, &b_iCol);
   fChain->SetBranchAddress("SiPM", SiPM, &b_SiPM);
   fChain->SetBranchAddress("iCry", iCry, &b_iCry);
   fChain->SetBranchAddress("iMax", iMax, &b_iMax);
   fChain->SetBranchAddress("Qval", Qval, &b_Qval);
   fChain->SetBranchAddress("Tval", Tval, &b_Tval);
   fChain->SetBranchAddress("Vmax", Vmax, &b_Vmax);
   fChain->SetBranchAddress("nSamples", nSamples, &b_nSamples);
   fChain->SetBranchAddress("wave", wave, &b_wave);
   fChain->SetBranchAddress("tWave", tWave, &b_tWave);
   fChain->SetBranchAddress("bline", bline, &b_bline);
   fChain->SetBranchAddress("lognTime", lognTime, &b_lognTime);
   fChain->SetBranchAddress("lognChi2", lognChi2, &b_lognChi2);
   fChain->SetBranchAddress("templTime", templTime, &b_templTime);
   fChain->SetBranchAddress("templChi2", templChi2, &b_templChi2);
   fChain->SetBranchAddress("templFit", templFit, &b_templFit);
   fChain->SetBranchAddress("templErr", templErr, &b_templErr);
   Notify();
}

Bool_t ana::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ana::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ana::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ana_cxx

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Nov 30 10:37:50 2020 by ROOT version 5.34/14
// from TTree Wavefull/Wavefull
// found on file: ../roottople/provamod0.root
//////////////////////////////////////////////////////////

#ifndef analysis_mod0_h
#define analysis_mod0_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TGraphErrors.h>
#include <iostream>

#include <string>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <sstream>

const int maxNcry    =   51;
const int maxNsipm   =  102;
const int maxNsample =  200; // NB: When changing this parameter, you need
                             // to modify it also in C code, branch definition:
                             // mod0->Branch("wave/tWave...

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class analysis_mod0 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           run;
   Int_t           subrun;
   Int_t           nevt;
   Int_t           nhits;
   Int_t           boardID[20];   //[nhits]
   Int_t           chanID[20];   //[nhits]
   Int_t           reserved1[20];   //[nhits]
   Int_t           reserved2[20];   //[nhits]
   Int_t           errflag[20];   //[nhits]
   Int_t           time[20];   //[nhits]
   Int_t           peakpos[20];   //[nhits]
   Int_t           nofsamples[20];   //[nhits]
   Int_t           firstsample[20];   //[nhits]
   Int_t           nsamples;
   Int_t           boardid[5000];   //[nsamples]
   Int_t           chan[5000];   //[nsamples]
   Int_t           ADC[5000];   //[nsamples]
   Int_t           timens[5000];   //[nsamples]
   
   Long64_t iTrigGlobal;


   // List of branches
   TBranch        *b_run;   //!
   TBranch        *b_subrun;   //!
   TBranch        *b_nevt;   //!
   TBranch        *b_nhits;   //!
   TBranch        *b_boardID;   //!
   TBranch        *b_chanID;   //!
   TBranch        *b_reserved1;   //!
   TBranch        *b_reserved2;   //!
   TBranch        *b_errflag;   //!
   TBranch        *b_time;   //!
   TBranch        *b_peakpos;   //!
   TBranch        *b_nofsamples;   //!
   TBranch        *b_firstsample;   //!
   TBranch        *b_nsamples;   //!
   TBranch        *b_boardid;   //!
   TBranch        *b_chan;   //!
   TBranch        *b_ADC;   //!
   TBranch        *b_timens;   //!

   TBranch *b_iTrigGlobal;

   //analysis_mod0(TTree *tree=0);
    analysis_mod0(TString fName="",TTree *tree=0);

   virtual ~analysis_mod0();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   //virtual void     Loop();
   virtual void     Loop(TString OutputFile="", int evflag=0);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   // Additional user functions
   virtual void  BookOutput(TTree *mod0=0);
   virtual int   GetValues (int jChan, int evflag=0);
   virtual void  getTlogn  (float Time, float Chi2);
   virtual void  getTemplateFit(TGraphErrors* gt);
   virtual int   Get_iCry (int jRow, int jCol);
   virtual int   Get_iSiPM (int jRow, int jCol, int jSiPM);

   //
   // Output nutple
   //

   TString         OutputFile;

   Int_t           nrun;
   Int_t           nsubrun;
   Int_t           evnum;
   Int_t           nHits;
   Int_t           _isLaser;
   Int_t           iDAQ[maxNsipm];
   Int_t           iHit[maxNsipm];
   Int_t           iRow[maxNsipm];
   Int_t           iCol[maxNsipm];
   Int_t           SiPM[maxNsipm];
   Int_t           iCry[maxNsipm];
   Int_t           iMax[maxNsipm];
   Double_t        Qval[maxNsipm];
   Double_t        Tval[maxNsipm];
   Double_t        Vmax[maxNsipm];
   Int_t           nSamples[maxNsipm];
   Double_t        wave[maxNsipm][maxNsample];
   Double_t        tWave[maxNsipm][maxNsample];
   Double_t        bline[maxNsipm];
   Double_t        lognTime[maxNsipm];
   Double_t        lognChi2[maxNsipm];
   Double_t        templTime[maxNsipm];
   Double_t        templChi2[maxNsipm];
   Double_t        templFit[maxNsipm][3];
   Double_t        templErr[maxNsipm][3];
   Int_t           crtBarTop;
   Int_t           crtBarBot;
   Int_t t1, t2, t3, t4;

};

#endif

#ifdef analysis_mod0_cxx
//analysis_mod0::analysis_mod0(TTree *tree) : fChain(0) 
analysis_mod0::analysis_mod0(TString fName,TTree *tree)
{

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fName);
      if (!f || !f->IsOpen()) {
         f = new TFile(fName);
      }
      f->GetObject("tree", tree);

   }

/*
  if (tree == 0) {
     // Get list of files and chain ntuples
     TChain *chain = new TChain("tree","");
     ifstream file(fileList);
     string fileName;
     while( getline(file,fileName) ){
       fileName.append("/tree");
       chain->Add(&fileName[0]);  // string2char by assigning the 1st character 
       cout << fileName << endl;  // of string to a pointer to char
     }
     tree=chain;
  }
*/
  Init(tree);
  



}

analysis_mod0::~analysis_mod0()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t analysis_mod0::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t analysis_mod0::LoadTree(Long64_t entry)
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

void analysis_mod0::Init(TTree *tree)
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

   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("subrun", &subrun, &b_subrun);
   fChain->SetBranchAddress("nevt", &nevt, &b_nevt);
   fChain->SetBranchAddress("nhits", &nhits, &b_nhits);
   fChain->SetBranchAddress("boardID", boardID, &b_boardID);
   fChain->SetBranchAddress("chanID", chanID, &b_chanID);
   fChain->SetBranchAddress("reserved1", reserved1, &b_reserved1);
   fChain->SetBranchAddress("reserved2", reserved2, &b_reserved2);
   fChain->SetBranchAddress("errflag", errflag, &b_errflag);
   fChain->SetBranchAddress("time", time, &b_time);
   fChain->SetBranchAddress("peakpos", peakpos, &b_peakpos);
   fChain->SetBranchAddress("nofsamples", nofsamples, &b_nofsamples);
   fChain->SetBranchAddress("firstsample", firstsample, &b_firstsample);
   fChain->SetBranchAddress("nsamples", &nsamples, &b_nsamples);
   fChain->SetBranchAddress("boardid", boardid, &b_boardid);
   fChain->SetBranchAddress("chan", chan, &b_chan);
   fChain->SetBranchAddress("ADC", ADC, &b_ADC);
   fChain->SetBranchAddress("timens", timens, &b_timens);

   fChain->SetBranchAddress("iTrigGlobal", &iTrigGlobal, &b_iTrigGlobal);

   Notify();
}

Bool_t analysis_mod0::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void analysis_mod0::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t analysis_mod0::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef analysis_mod0_cxx

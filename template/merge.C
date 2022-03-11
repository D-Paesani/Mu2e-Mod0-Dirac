#include <TTree.h>
#include <TFile.h>
#include <TList.h>



void merge(){

  TString inPath =    "../data/in/";
  TString outPath =   "../data/in/";
  TString outName =   "anarun33_all.root";
  TString treeName =  "mod0";

  TFile *files[8] = {  
    
  //new TFile( inPath + "a0.2gs40.root"), 
  new TFile( inPath + "anarun33_0.root"), 
  new TFile( inPath + "anarun33_1.root"), 
  new TFile( inPath + "anarun33_2.root"), 
  new TFile( inPath + "anarun33_3.root"), 
  new TFile( inPath + "anarun33_4.root"), 
  new TFile( inPath + "anarun33_5.root"), 
  new TFile( inPath + "anarun33_6.root"), 
  new TFile( inPath + "anarun33_7.root")
  };

  TFile *outf =        new TFile( outPath + outName, "RECREATE");

  TList *lst = new TList;

  for(auto &f: files){
    TTree *tree = (TTree*)f->Get(treeName);
    lst->Add(tree);
  }

  TTree *newtree = TTree::MergeTrees(lst);
  newtree->SetName(treeName);
  outf->cd();
  newtree->Write();
}

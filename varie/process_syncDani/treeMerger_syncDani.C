#include <TTree.h>
#include <TFile.h>
#include <TList.h>



void treeMerger_syncDani(){

  const int lastFileNo = 960; /////// 960
  const int runNumber = 165;

  const int start = 0;
  
  TString outName =   Form("../data/processed/run%i_SD.root", runNumber); 
  TString treeName =  "tree";

  TFile *files[lastFileNo+1];
  for(int i=start; i <= lastFileNo; i++) {
    std::cout<<"opening:  "<<Form("../data/processed/run_%i_%i_syncDani.root", runNumber, i)<<std::endl;
    files[i] = new TFile(Form("../data/processed/run_%i_%i_syncDani.root", runNumber, i)); 
  }


  TList *lst = new TList;

  for(int i=start; i<= lastFileNo; i++){
    std::cout<<"adding:  "<<Form("run_%i_%i_syncDani.root", runNumber, i)<<std::endl;
    TTree *tree = (TTree*)(files[i])->Get(treeName);
    lst->Add(tree);
  }

  TFile *outf =        new TFile( outName, "RECREATE");
  outf->cd();
  
  TTree *newtree = TTree::MergeTrees(lst);
  newtree->SetName(treeName);
  newtree->Write();
}

int main(){
  treeMerger_syncDani();
}
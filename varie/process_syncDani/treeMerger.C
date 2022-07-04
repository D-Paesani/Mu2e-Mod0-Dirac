#include <TTree.h>
#include <TFile.h>
#include <TList.h>



void treeMerger(){

  const int lastFileNo = 3; ///////
  const int runNumber = 49;

  TString outName =   Form("../data/processed/run%i.root", runNumber); 
  TString treeName =  "tree";

  TFile *files[lastFileNo+1];
  for(int i=0; i <= lastFileNo; i++) {
    cout<<"opening:  "<<Form("../data/processed/run_%i_%i.root", runNumber, i)<<endl;
    files[i] = new TFile(Form("../data/processed/run_%i_%i.root", runNumber, i)); 
  }


  TList *lst = new TList;

  for(int i=0; i<= lastFileNo; i++){
    cout<<"adding:  "<<i<<endl;
    TTree *tree = (TTree*)(files[i])->Get(treeName);
    lst->Add(tree);
  }

  TFile *outf =        new TFile( outName, "RECREATE");
  outf->cd();
  
  TTree *newtree = TTree::MergeTrees(lst);
  newtree->SetName(treeName.Data());
  newtree->Write();
}

int main(){
  treeMerger();
}
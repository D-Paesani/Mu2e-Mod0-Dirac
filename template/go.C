
#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

#include "ana.h"
#include "ana.C"

using namespace std;

#define __runName "33_all" 
#define _splinesName "33_all"
#define __analysisMode "f"
#define _maxEvents 200000000000
#define _inFormat "../data/in/anarun%s.root"
#define _outAnaName "../data/template/ana/%s_an.root"
#define _splineFileName "../data/template/spline/splines_%s.root"
#define _analysisInTreeName "mod0"
#define _splinesFormat "spline/spline_%d"
#define _doMakeNewFile "y"


void go(TString arg1 = "", TString arg2 = "") {

    TString _outName, _analysisMode = arg1.CompareTo("")==0?__analysisMode:arg1, _runName = arg2.CompareTo("")==0?__runName:arg2;

    if (_analysisMode == "g") { _outName = _splineFileName; }
    else if (_analysisMode == "f") {  _outName = _outAnaName; }

    AN.mode = _analysisMode;
    AN.runName = _runName;
    AN.inFileName = Form(_inFormat, _runName.Data());
    AN.outFileName = Form(_outName, _runName.Data());
    AN.splineFileName = Form(_splineFileName, _runName.Data());
    AN.doMakeNewFile = _doMakeNewFile;
    
    cout<<endl<<"-----> runName: "<<AN.runName<<endl;
    cout<<"-----> inFile: "<< AN.inFileName<<endl;
    cout<<"-----> outFile: "<<AN.outFileName<<endl;
    cout<<"-----> splineFile: "<< AN.splineFileName<<endl;
    cout<<"-----> mode: "<<AN.mode<<endl;

    AN.inFile = new TFile(AN.inFileName);  
    AN.inFile->GetObject(_analysisInTreeName, AN.chain);
    AN.outFile = new TFile(AN.outFileName, "RECREATE");

    if(AN.mode == "f") {
        cout<<"-----> Loading splines from "<<AN.splineFileName<<endl;
        AN.splineFile = new TFile(AN.splineFileName);
        for ( int k = 0; k < PRM.chNo; k++) {
            AN.splineFile->GetObject(Form(_splinesFormat, k), AN.splines[k]); 
        }
        cout<<endl;
    }

    cout<<"-----> Chain entries: "<<AN.chain->GetEntriesFast()<<endl;
    AN.etp = min((Long64_t)_maxEvents, (Long64_t)AN.chain->GetEntriesFast());
    cout << "-----> Events to process: "<<AN.etp<<endl<<endl;

    AN.outFile->cd();
    AN.outDirs.specimens = AN.outFile->mkdir("specimens");
    if (AN.mode == "g") {
        AN.outDirs.spline = AN.outFile->mkdir("spline");
        AN.outDirs.splineGraph = AN.outFile->mkdir("splineGraph");
        AN.outDirs.templDraw = AN.outFile->mkdir("templDraw");
        AN.outDirs.profile = AN.outFile->mkdir("profile");
    }

    gRandom->SetSeed();
    gStyle->SetOptFit(1111);

    ana *ANALYSIS = new ana(AN.chain);
    ANALYSIS->Loop();
    
    AN.outFile->Close();

    cout<<endl<<endl;
    cout<<endl<<endl<<" ---->>> open output file here:"<<endl<<"      root "<<AN.outFileName<<endl<<endl;


    
}








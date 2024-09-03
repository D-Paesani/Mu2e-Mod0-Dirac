
#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

#include "ana.h"
#include "ana.C"

using namespace std;



//config  
    #define _runName "run165" 
    #define _analysisMode ""
    #define _maxEvents 1e9
    #define _anaOptions "makeNewFile(off) anaMode(gen) optim(off)"

    #define _splineFileName "../data/template/spline/splines_%s.root"
    #define _splinesFormat "spline/spline_%d"
    #define _splinesName ""

    #define _inFormat "../data/roottople/%s.root"
    #define _outName "../data/template/ana/%s_an.root"
    #define _analysisInTreeName "mod0"
//config  



void goGen() {

    AN.mode = _analysisMode;
    AN.runName = _runName;
    AN.inFileName = Form(_inFormat, _runName);
    AN.outFileName = Form(_splineFileName, _runName);
    AN.splineFileName = "";
    AN.anaOptions = _anaOptions;
    AN.splinesNameFormat = _splinesFormat;
    
    cout<<endl<<"-----> runName: "<<AN.runName<<endl;
    cout<<"-----> inFile: "<< AN.inFileName<<endl;
    cout<<"-----> outFile: "<<AN.outFileName<<endl;
    cout<<"-----> splineFile: "<< AN.splineFileName<<endl;
    cout<<"-----> mode: "<<AN.mode<<endl;
    cout<<"-----> options :"<<AN.anaOptions<<endl;

    AN.inFile = new TFile(AN.inFileName);  
    AN.inFile->GetObject(_analysisInTreeName, AN.chain);
    AN.outFile = new TFile(AN.outFileName, "RECREATE");

    cout<<"-----> Chain entries: "<<AN.chain->GetEntriesFast()<<endl;
    AN.etp = min((Long64_t)_maxEvents, (Long64_t)AN.chain->GetEntriesFast());
    cout << "-----> Events to process: "<<AN.etp<<endl<<endl;

    ana *ANALYSIS = new ana(AN.chain);
    ANALYSIS->Loop();
    
    AN.outFile->Close();

    cout<<endl<<endl;
    cout<<endl<<endl<<" ---->>> open output file here:"<<endl<<"      root "<<AN.outFileName<<endl<<endl;


    
}








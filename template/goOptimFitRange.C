
#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

#include "ana.h"
#include "ana.C"

using namespace std;



//config  
    #define _runName "33all" 
    #define _analysisMode ""
    #define _maxEvents 1e4
    #define _anaOptions "makeNewFile(off) anaMode(fit) optim(fit)"

    #define _splineFileName "../data/template/spline/splines_%s.root"
    #define _splinesFormat "spline/spline_%d"
    #define _splinesName "33all"

    #define _inFormat "../data/roottople/anarun%s.root"
    #define _outAnaName "../data/template/optim/optFitRange_%s_step%d.root"
    #define _analysisInTreeName "mod0"

    #define _optimSteps 5
    #define _rangeStart 18.0
    #define _rangeStep 1.0
//config  

TGraphErrors optimGraph;



void goOptimFitRange() {

    AN.mode = _analysisMode;
    AN.runName = _runName;
    AN.inFileName = Form(_inFormat, _runName);
    AN.splineFileName = Form(_splineFileName, _runName);
    AN.anaOptions = _anaOptions;
    AN.splinesNameFormat = _splinesFormat;

    for (int i = 0; i < _optimSteps; i++) {
  
        AN.outFileName = Form( _outAnaName, _runName, i);
        
        cout<<endl<<"-----> runName: "<<AN.runName<<endl;
        cout<<"-----> inFile: "<< AN.inFileName<<endl;
        cout<<"-----> outFile: "<<AN.outFileName<<endl;
        cout<<"-----> splineFile: "<< AN.splineFileName<<endl;
        cout<<"-----> mode: "<<AN.mode<<endl;
        cout<<"-----> options: "<<AN.anaOptions<<endl;
        cout<<"-----> step: "<<i+1<<" / "<<_optimSteps<<endl<<endl;

        AN.inFile = new TFile(AN.inFileName);  
        AN.inFile->GetObject(_analysisInTreeName, AN.chain);
        AN.outFile = new TFile(AN.outFileName, "RECREATE");

        cout<<"-----> Chain entries: "<<AN.chain->GetEntriesFast()<<endl;
        AN.etp = min((Long64_t)_maxEvents, (Long64_t)AN.chain->GetEntriesFast());
        cout << "-----> Events to process: "<<AN.etp<<endl<<endl;

        AN.teFitStop = _rangeStart + i * _rangeStep;

        ana *ANALYSIS = new ana(AN.chain);
        ANALYSIS->Loop();

        optimGraph.AddPoint(AN.CF, AN.res.optimReso);
        optimGraph.SetPointError(optimGraph.GetN() - 1, 0, AN.res.optimResoErr);

        AN.inFile->Close();

        if (i < _optimSteps - 1) { AN.outFile->Close(); }

    }

    //AN.outDirs.optimisation()->cd();
    AN.outFile->cd();

    optimGraph.Write("CFoptim");

    TCanvas cc;
    cc.cd();
    optimGraph.Draw("AP");

    AN.outFile->Close();

    cout<<endl<<endl;
    cout<<endl<<endl<<" ---->>> open output file here:"<<endl<<"      root "<<AN.outFileName<<endl<<endl;
    
}








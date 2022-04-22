
#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

#include "ana.h"
#include "ana.C"

using namespace std;



//config  
    #define _runName "33all" 
    #define _maxEvents 1e4

    #define _splineFileName "../data/template/optim/optimSpline_%d.root"
    #define _splinesFormat "spline/spline_%d"
    #define _splinesName "33all"

    #define _inFormat "../data/roottople/anarun%s.root"
    #define _outAnaName "../data/template/optim/optFitGenCf_%s_step%d.root"
    #define _analysisInTreeName "mod0"

    #define _optimSteps 6
    #define _rangeStart 0.05
    #define _rangeStep 2
//config  

TGraphErrors optimGraph;



void goOptimFitGenCf() {

    AN.runName = _runName;
    AN.inFileName = Form(_inFormat, _runName);
    AN.splinesNameFormat = _splinesFormat;

    for (int i = 0; i < _optimSteps; i++) {

        //gen
            AN.anaOptions = "makeNewFile(off) anaMode(gen) optim(on)";
            TString currentSpline = Form( _splineFileName, i);
            AN.outFileName = currentSpline;
            
            cout<<endl<<"-----> runName: "<<AN.runName<<endl;
            cout<<"-----> inFile: "<< AN.inFileName<<endl;
            cout<<"-----> outFile: "<<AN.outFileName<<endl;
            cout<<"-----> splineFile: "<< AN.splineFileName<<endl;
            cout<<"-----> mode: "<<AN.mode<<endl;
            cout<<"-----> options: "<<AN.anaOptions<<endl;
            cout<<"-----> step: GEN "<<i+1<<" / "<<_optimSteps<<endl<<endl;

            AN.inFile = new TFile(AN.inFileName);  
            AN.inFile->GetObject(_analysisInTreeName, AN.chain);
            AN.outFile = new TFile(AN.outFileName, "RECREATE");

            cout<<"-----> Chain entries: "<<AN.chain->GetEntriesFast()<<endl;
            AN.etp = min((Long64_t)_maxEvents, (Long64_t)AN.chain->GetEntriesFast());
            cout << "-----> Events to process: "<<AN.etp<<endl<<endl;

            AN.CF = _rangeStart + i * _rangeStep;

            ana *ANALYSIS = new ana(AN.chain);
            ANALYSIS->Loop();

            AN.inFile->Close();
            AN.outFile->Close(); 
        //gen

        //fit
            AN.anaOptions = "makeNewFile(off) anaMode(fit) optim(on)";
            AN.outFileName = Form( _outAnaName, _runName, i);
            AN.splineFileName = currentSpline;
            AN.outFileName = Form( _outAnaName, _runName, i);
        
            cout<<endl<<"-----> runName: "<<AN.runName<<endl;
            cout<<"-----> inFile: "<< AN.inFileName<<endl;
            cout<<"-----> outFile: "<<AN.outFileName<<endl;
            cout<<"-----> splineFile: "<< AN.splineFileName<<endl;
            cout<<"-----> mode: "<<AN.mode<<endl;
            cout<<"-----> options: "<<AN.anaOptions<<endl;
            cout<<"-----> step: FIT "<<i+1<<" / "<<_optimSteps<<endl<<endl;

            AN.inFile = new TFile(AN.inFileName);  
            AN.inFile->GetObject(_analysisInTreeName, AN.chain);
            AN.outFile = new TFile(AN.outFileName, "RECREATE");

            cout<<"-----> Chain entries: "<<AN.chain->GetEntriesFast()<<endl;
            AN.etp = min((Long64_t)_maxEvents, (Long64_t)AN.chain->GetEntriesFast());
            cout << "-----> Events to process: "<<AN.etp<<endl<<endl;

            ANALYSIS = new ana(AN.chain);
            ANALYSIS->Loop();

            optimGraph.AddPoint(AN.CF, AN.res.optimReso);
            optimGraph.SetPointError(optimGraph.GetN() - 1, 0, AN.res.optimResoErr);

            AN.inFile->Close(); 
            if (i < _optimSteps - 1) { AN.outFile->Close(); }
        //fit
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








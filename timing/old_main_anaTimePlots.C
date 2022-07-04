
#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

using namespace std;

#include "anaTimePlots.h"
#include "anaTimePlots.C"

#define _runName "run34" 
#define _anaMode ""
#define _anaOptions "sel(vert) equal(fromTree) closeFile(on) useSlopeZY(on)" 
#define _maxEvents 2e9
#define _inFileFormat "../data/roottople/%s_new.root"
#define _inTreeName "mod0"
#define _outAnaFormat "../data/timing/anaTimePlots/ana_%s.root"
#define _chargeCalFileFormat "../data/calibration/energy/eneCal_%s.txt"
#define _timeCalFileFormat "../data/calibration/time/timOff_%s.txt"

void main_anaTimePlots(TString arg1 = "", TString arg2 = "",  TString arg3 = "") {

    gErrorIgnoreLevel = kFatal;

    TString mode =  arg1.CompareTo("") == 0 ? _anaMode : arg1;
    TString inFileName = arg2.CompareTo("") == 0 ?_runName : arg2;
    TString options = arg3.CompareTo("") == 0 ? _anaOptions : arg2;

    anaTimePlots::_config anaConfig;

    anaConfig.mode = mode;
    anaConfig.options = options;
    anaConfig.evLimit = _maxEvents;
    anaConfig.runName = inFileName;
    anaConfig.inFileName = Form(_inFileFormat, _runName);
    anaConfig.inTreeName = _inTreeName;
    anaConfig.outFileName = Form(_outAnaFormat, _runName);
    anaConfig.chargeCalFileName = Form(_chargeCalFileFormat, _runName);
    anaConfig.timeCalFileName = Form(_timeCalFileFormat, _runName);

    cout<<endl;
    cout<<"-----> anaMode: "<<anaConfig.mode<<endl;
    cout<<"-----> anaOptions: "<<anaConfig.options<<endl;
    cout<<"-----> runName: "<<anaConfig.runName<<endl;
    cout<<"-----> inFile: "<<anaConfig.inFileName<<endl;
    cout<<"-----> outFile: "<<anaConfig.outFileName<<endl;
    cout<<"-----> chargeCalFile: "<<anaConfig.chargeCalFileName<<endl;
    cout<<"-----> timeCalFile: "<<anaConfig.timeCalFileName<<endl;
    
    anaTimePlots ANALYSIS(&anaConfig);
    cout<<"-----> Chain entries: "<<anaConfig.chainEntries<<endl;
    cout<<"-----> Events to process: "<<anaConfig.etp<<endl<<endl;
    
    ANALYSIS.Launch();

}











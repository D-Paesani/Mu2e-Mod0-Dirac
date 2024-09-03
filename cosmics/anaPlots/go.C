using namespace std;

#include <iostream> 
#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

#include "anaPlots.h"
#include "anaPlots.C"
#include "proc.C"
#include "proch.C"
#include "hist.C"
#include "loop.C"

#define _runName "run34" 
#define _addRunTag "" 
#define _anaOptions ""
#define _maxEvents 2e9

#define _inTreeName "mod0"
#define _inFileFormat "../data/roottople/%s_new.root" 
#define _outAnaFormat "../data/anaPlots/ana_%s%s.root"
#define _chargeCalFileFormat "../data/calibration/energy/eneCal_%s.txt"
#define _timeCalFileFormat "../data/calibration/time/timOff_%s.txt"
#define _crtFileFormat "../data/crt/crt_%s.root"

void ConfRun(anaPlots *, TString, TString);

void go(TString arg1 = "", TString arg2 = "") {

    TString runName = arg1.CompareTo("") == 0 ? _runName : arg1;
    TString options = arg2.CompareTo("") == 0 ? _anaOptions : arg2;

    anaPlots *thisAna = new anaPlots();
    anaPlots::AnaVars *thisVars = &(thisAna->VAR);
    anaPlots::AnaConf *thisConf = &(thisVars->CONF);

    thisConf->options = options;
    thisConf->evLimit = _maxEvents;
    thisConf->runName = runName;
    thisConf->inFileName = Form(_inFileFormat, _runName);
    thisConf->inTreeName = _inTreeName;
    thisConf->outFileName = Form(_outAnaFormat, _runName, _addRunTag);
    thisConf->chargeCalFileName = Form(_chargeCalFileFormat, _runName);
    thisConf->timeCalFileName = Form(_timeCalFileFormat, _runName);
    thisConf->crtFileName = Form(_crtFileFormat, runName.Data());

    ConfRun(thisAna, runName, options);
    thisAna->Init();

    cout<<endl;
    cout<<"-----> anaMode: "<<thisConf->mode<<endl;
    cout<<"-----> anaOptions: "<<thisConf->options<<endl;
    cout<<"-----> runName: "<<thisConf->runName<<endl;
    cout<<"-----> inFile: "<<thisConf->runName<<endl;
    cout<<"-----> outFile: "<<thisConf->outFileName<<endl;
    cout<<"-----> chargeCalFile: "<<thisConf->chargeCalFileName<<endl;
    cout<<"-----> timeCalFile: "<<thisConf->timeCalFileName<<endl;
    cout<<"-----> Chain entries: "<<thisConf->chainEntries<<endl;
    cout<<"-----> Events to process: "<<thisConf->etp<<endl;
    cout<<endl;
    
    gErrorIgnoreLevel = kFatal;

    thisAna->Launch();

}

void ConfRun(anaPlots *ana, TString run, TString opt) {

    anaPlots::AnaVars *vars = &(ana->VAR);
    anaPlots::AnaConf *conf = &(vars->CONF);
    anaPlots::AnaCalib *calib = &(vars->CAL);
    
    conf->set_closeOutFile = 1;
    conf->set_useTime = "FIT";
    conf->set_mipFilter = "PV";
    conf->set_useSlopeYZ = 1;
    conf->set_useCRT = 0;

    conf->set_equalFromFile = 1;
    conf->set_equalFromRaw = 0;
    conf->set_equalFileWrite = 0;

    ana->CRT.SetGeometry( 880, 0, 0, true, false, true );
    ana->CRT.SetOrigin( -20, 880/2 - 150, 0 );

    calib->qRef = 5000.0;
    calib->MIP_MPV_eRef = thisAna->PRM.MIP_MPV_eRef;
    calib->qToE = thisCalib->MIP_MPV_eRef/thisCalib->qRef;

    if (run == ""){
        
    } else if(run == "run34" ) { cout<<endl<<"-----> Applying configuration for run: "<<run<<endl<<endl;
        conf->set_mipFilter = "PA";
        conf->set_useSlopeYZ = 1;
        conf->set_equalFromFile = 1;
        conf->set_useCRT = 0;
    } else if(run == "run165") { cout<<endl<<"-----> Applying configuration for run: "<<run<<endl<<endl;
        conf->set_mipFilter = "PV";
        conf->set_useSlopeYZ = 0;
        conf->set_useCRT = 1;        
        conf->evLimit = 210000;
        ana->MIP.minCryNum = 5;
        ana->MIP.cellEneMin = 5;
        ana->MIP.cellEneMax = 60;
    }

}










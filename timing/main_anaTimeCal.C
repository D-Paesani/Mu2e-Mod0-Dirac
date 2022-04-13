
#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

using namespace std;

#include "anaTimeCal.h"
#include "anaTimeCal.C"

#define _runName "33all" 
#define _anaMode ""
#define _anaOptions "sel->vert equal->fromFile useSlopeZ->off" 
#define _maxEvents 20000000
#define _inFileFormat "../data/roottople/anarun%s_new.root"
#define _inList "../data/runXX.list"
#define _inTreeName "mod0"
#define _outAnaFormat "../data/timing/anaTimeCal/timcal_%s_%s.root"
#define _chargeCalFileFormat "../data/calibration/energy/eneCal_%s.txt"
#define _timeCalFileFormat "../data/calibration/time/timOff_%s.txt"
#define _outFileFormat "../data/timing/anaTimeCal/timcal_%s.root"
#define _stepFormat "step%d"
#define _iterations 1+7


void main_anaTimeCal(TString arg1 = "", TString arg2 = "",  TString arg3 = "") {

    TString mode =  arg1.CompareTo("") == 0 ? _anaMode : arg1;
    TString inFileName = arg2.CompareTo("") == 0 ?_runName : arg2;
    TString options = arg3.CompareTo("") == 0 ? _anaOptions : arg2;

    TFile *outFile = new TFile(Form(_outFileFormat, _runName), "recreate");
    TH1F residHisto("residHisto", "resid", 1000, -0.1, 0.1);
    outFile->cd();
    TCanvas c_residMean("residMean"), c_residRms("residRms"), c_useless("useless");
    Style_t colorArray[8] = {kMagenta, kBlack, kGreen, kBlue, kOrange, kPink+3, kRed, kYellow};  
    

    anaTimeCal::_calib anaCalibs[_iterations];

    for (int currentStep = 0; currentStep < _iterations; currentStep++) {
        
        TString currentStepTag = Form(_stepFormat, currentStep);

        anaTimeCal::_config anaConfig;

        anaConfig.mode = mode;
        anaConfig.options = options + (currentStep + 1 == _iterations ? " isLastStep" : "");
        anaConfig.step = currentStep;
        anaConfig.evLimit = _maxEvents;
        anaConfig.runName = inFileName;
        anaConfig.inFileName = Form(_inFileFormat, _runName);
        anaConfig.inTreeName = _inTreeName;
        anaConfig.outFileName = Form(_outAnaFormat, _runName, currentStepTag.Data());
        anaConfig.chargeCalFileName = Form(_chargeCalFileFormat, _runName);
        anaConfig.timeCalOutFileName = currentStep == _iterations - 1 ? Form(_timeCalFileFormat, _runName) : "";

        cout<<endl<<"-----> currentStep: "<<currentStep<<" / "<<_iterations-1<<endl;
        cout<<"-----> anaMode: "<<anaConfig.mode<<endl;
        cout<<"-----> anaOptions: "<<anaConfig.options<<endl;
        cout<<"-----> runName: "<<anaConfig.runName<<endl;
        cout<<"-----> inFile: "<<anaConfig.inFileName<<endl;
        cout<<"-----> outFile: "<<anaConfig.outFileName<<endl;
        cout<<"-----> chargeCalFile: "<<anaConfig.chargeCalFileName<<endl;
        cout<<"-----> timeCalFileOutput: "<<anaConfig.timeCalOutFileName<<endl;
        
        anaTimeCal currentAna(&anaConfig);
        cout<<"-----> Chain entries: "<<anaConfig.chainEntries<<endl;
        cout<<"-----> Events to process: "<<anaConfig.etp<<endl<<endl;

        if ( currentStep > 0 ) {
            for ( int k = 0; k < anaTimeCal::GEO.cryNo; k++) {
                for ( int kk = 0; kk < anaTimeCal::GEO.sidNo; kk++) {
                    currentAna.CAL.timeOff[k][kk] = anaCalibs[currentStep - 1].timeOffOut[k][kk];
                }
            }
        }

        currentAna.Launch();

        for ( int k = 0; k < anaTimeCal::GEO.cryNo; k++) {
            for ( int kk = 0; kk < anaTimeCal::GEO.sidNo; kk++) {

                double firstStep = currentStep == 0 ? currentAna.CAL.timeOffOut[k][kk] : 0;
                double firstStepErr = currentStep == 0 ? currentAna.CAL.timeOffErrOut[k][kk] : 0;
                anaCalibs[currentStep].timeOffOut[k][kk] = currentAna.CAL.timeResidOut[k][kk] + currentAna.CAL.timeOff[k][kk] + firstStep ;
                anaCalibs[currentStep].timeOffErrOut[k][kk] = currentAna.CAL.timeResidErrOut[k][kk] + firstStepErr;
                anaCalibs[currentStep].timeResidOut[k][kk] = currentAna.CAL.timeResidOut[k][kk];
                anaCalibs[currentStep].timeResidErrOut[k][kk] = currentAna.CAL.timeResidErrOut[k][kk];

                if (currentStep + 1 == _iterations && currentAna.RES.isUsed[k][kk]) { 
                    residHisto.Fill(currentAna.CAL.timeResidOut[k][kk]); 
                } 
            }
        }

        if (currentStep > 0) {
            int color = colorArray[_iterations - currentStep - 1];
            currentAna.RES.residMean->SetLineColor(color);
            currentAna.RES.residMean->SetMarkerColor(color);
            currentAna.RES.residRms->SetLineColor(color);
            currentAna.RES.residRms->SetMarkerColor(color);
            c_useless.cd();
            c_residMean.cd();
            currentAna.RES.residMean->Draw(Form("%sP", currentStep > 1 ? "" : "A"));
            c_residMean.ForceUpdate();
            c_useless.cd();
            c_residRms.cd();
            currentAna.RES.residRms->Draw(Form("%sP", currentStep > 1 ? "" : "A"));
            c_residRms.ForceUpdate();
            c_useless.cd();
        }

    }

    outFile->cd();
    c_residMean.BuildLegend();
    c_residRms.BuildLegend();
    c_residMean.Write();
    c_residRms.Write();
    residHisto.Write();















  
}











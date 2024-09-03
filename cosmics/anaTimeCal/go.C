using namespace std;

#include <iostream> 

#include "TChain.h"
#include "TString.h" 
#include "TSystem.h" 

#include "anaTimeCal.h"
#include "anaTimeCal.C"
#include "Processors.C"
#include "ProcessorHelper.C"
#include "Histograms.C"

#define _runName "run34"
#define _addRunTag "" 
#define _anaOptions "" 
#define _maxEvents 1e9
#define _iterations 1+9
#define _iterationsToDisplay 8

#define _inTreeName "mod0"
#define _inFileFormat "../data/roottople/%s_new.root"
#define _chargeCalFileFormat "../data/calibration/energy/eneCal_%s_timcal.txt"
#define _timeCalFileFormat "../data/calibration/time/timOff_%s.txt"
#define _outFileFormat "../data/anaTimeCal/timcal_%s%s.root"
#define _crtFileFormat "../data/crt/crt_%s.root"
#define _stepFormat "step%d"

void go(TString arg1 = "", TString arg2 = "",  TString arg3 = "") {

    // gErrorIgnoreLevel = kFatal;

    TString runName = arg1.CompareTo("") == 0 ?_runName : arg1;
    TString options = arg2.CompareTo("") == 0 ? _anaOptions : arg2;

    TGraphErrors *gmean[_iterations], *grms[_iterations];
    TH1F *hresid[_iterations];
    TCanvas c_residMean("residMean"), c_residRms("residRms"), c_residHist("residHist"), c_useless("useless");
    Style_t colorArray[8] = {kMagenta, kBlack, kGreen, kBlue, kOrange, kPink+3, kRed, kYellow};  
    TGraphErrors *gmeanAll = new TGraphErrors();
    TGraphErrors *grmsAll = new TGraphErrors();
    TGraphErrors *residAll = new TGraphErrors();
    gmeanAll->SetTitle("meanAll");
    grmsAll->SetTitle("grmsAll");
    residAll->SetTitle("residAll");

    TString outFileName = Form(_outFileFormat, _runName, _addRunTag);
    TFile *outFile = new TFile(outFileName.Data(), "recreate");
    TDirectory *allDir = outFile->mkdir("overview");

    anaTimeCal::AnaCalib calibs[_iterations];

    for (int currentStep = 0; currentStep < _iterations; currentStep++) {
        
        TString currentStepTag = Form(_stepFormat, currentStep);
        int isLast = currentStep + 1 == _iterations ? 1 : 0;
        int isFirst = currentStep == 0;

        anaTimeCal* thisAna = new anaTimeCal();
        anaTimeCal::AnaConf *thisConf = &(thisAna->VAR.CONF);
        anaTimeCal::AnaCalib *thisCalib = &(thisAna->VAR.CAL);

        thisConf->options = options;
        thisConf->step = currentStep;
        thisConf->evLimit = _maxEvents;
        thisConf->runName = _runName;
        thisConf->inFileName = Form(_inFileFormat, _runName);
        thisConf->inTreeName = _inTreeName;
        thisConf->chargeCalFileName = Form(_chargeCalFileFormat, _runName);
        thisConf->timeCalOutFileName = isLast ? Form(_timeCalFileFormat, _runName) : "";
        thisConf->outFile = outFile;
        thisConf->outFileName = outFileName;

        thisConf->set_equalFromFile = !isFirst;
        thisConf->set_equalFromRaw = isFirst;
        thisConf->set_mipFilter = "PV";
        thisConf->set_useTime = "FIT";
        thisConf->set_saveTimCalib = isLast;
        thisConf->set_useCRT = 0;
        thisConf->set_useSlopeYZ = 0;
        thisConf->set_saveEneCalib = 0;

        thisAna->MIP.minCryNum = 5;
        thisAna->MIP.cellEneMin = 10;
        thisAna->MIP.cellEneMax = 35;

        thisCalib->qRef = 5000.0;
        thisCalib->MIP_MPV_eRef = thisAna->PRM.MIP_MPV_eRef;
        thisCalib->qToE = thisCalib->MIP_MPV_eRef/thisCalib->qRef;

        thisAna->Init();

        cout<<endl<<"-----> currentStep: "<<currentStep<<" / "<<_iterations-1<<endl;
        cout<<"-----> runName: "<<thisConf->runName<<endl;
        cout<<"-----> inFile: "<<thisConf->inFileName<<endl;
        cout<<"-----> outFile: "<<outFileName<<endl;
        cout<<"-----> chargeCalFile: "<<thisConf->chargeCalFileName<<endl;
        cout<<"-----> timeCalFileOutput: "<<thisConf->timeCalOutFileName<<endl;
        cout<<"-----> Chain entries: "<<thisConf->chainEntries<<endl;
        cout<<"-----> Events to process: "<<thisConf->etp<<endl<<endl;

        if ( currentStep > 0 ) {

            for ( int k = 0; k < anaTimeCal::GEO.cryNo; k++) {
                for ( int kk = 0; kk < anaTimeCal::GEO.sidNo; kk++) {
                    thisCalib->timeOff[k][kk] = calibs[currentStep-1].timeOff[k][kk] + calibs[currentStep-1].timeResidOut[k][kk];
                }
            }

            thisAna->Launch();

            int color = colorArray[_iterations - currentStep - 1];

            TString name = Form("step_%d", currentStep);
            
            gmean[currentStep-1] = (TGraphErrors*)thisAna->VAR.hm->GetGraph("residMean", 0)->Clone(name);
            grms[currentStep-1] = (TGraphErrors*)thisAna->VAR.hm->GetGraph("residRms", 0)->Clone(name);
            hresid[currentStep-1] = (TH1F*)thisAna->VAR.hm->GetHist("residHisto", 0)->Clone(name);
            
            gmean[currentStep-1]->SetTitle(name);
            grms[currentStep-1]->SetTitle(name);
            hresid[currentStep-1]->SetTitle(name);

            // fillGraph(gmeanAll, currentStep, gmean[currentStep-1]->GetMean(2), 0, gmean[currentStep-1]->GetRMS()/TMath::Sqrt(gmean[currentStep-1]->GetN()));
            fillGraph(gmeanAll, currentStep, gmean[currentStep-1]->GetMean(2), 0, gmean[currentStep-1]->GetRMS(2));
            fillGraph(grmsAll, currentStep, grms[currentStep-1]->GetMean(2), 0, grms[currentStep-1]->GetRMS(2));
            // fillGraph(grmsAll, currentStep, grms[currentStep-1]->GetMean(2), 0, grms[currentStep-1]->GetRMS(2)/TMath::Sqrt(grms[currentStep-1]->GetN()));
            fillGraph(residAll, currentStep, hresid[currentStep-1]->GetRMS(), 0, hresid[currentStep-1]->GetRMSError());
            gmeanAll->SetNameTitle("gmeanAll", "gmeanAll");
            grmsAll->SetNameTitle("grmsAll", "grmsAll");
            residAll->SetNameTitle("residAll", "residAll");

            gmean[currentStep-1]->MovePoints(0.02*(double)currentStep, 0);
            grms[currentStep-1]->MovePoints(0.02*(double)currentStep, 0);
            gmean[currentStep-1]->SetLineColor(color);
            gmean[currentStep-1]->SetMarkerColor(color);
            gmean[currentStep-1]->SetMarkerStyle(20);
            gmean[currentStep-1]->SetMarkerSize(0.3);
            grms[currentStep-1]->SetLineColor(color);
            grms[currentStep-1]->SetMarkerColor(color);
            grms[currentStep-1]->SetMarkerStyle(20);
            grms[currentStep-1]->SetMarkerSize(0.3);

            if (currentStep >= _iterations - _iterationsToDisplay) {

                c_useless.cd();
                c_residMean.cd();
                gmean[currentStep-1]->Draw(Form("%sP", currentStep > 1 ? "" : "A"));
                c_residMean.ForceUpdate();
                
                c_useless.cd();
                c_residRms.cd();
                grms[currentStep-1]->Draw(Form("%sP", currentStep > 1 ? "" : "A"));
                c_residRms.ForceUpdate();
                
                c_useless.cd();
                c_residHist.cd();
                hresid[currentStep-1]->SetLineColor(color);
                hresid[currentStep-1]->SetFillColor(color);
                hresid[currentStep-1]->Draw(Form( "%s", currentStep > 1 ? "same" : ""));
                c_residHist.ForceUpdate();

                c_useless.cd();

            }
        
        } else {

            thisAna->Launch();

        }
        
        calibs[currentStep] = *thisCalib;

    }

    allDir->cd();
    c_residMean.BuildLegend();
    c_residRms.BuildLegend();
    c_residHist.BuildLegend();
    c_residMean.Write();
    c_residRms.Write();
    c_residHist.Write();
    gmeanAll->Write();
    grmsAll->Write();
    residAll->Write();

    outFile->Close(); 
    cout<<endl<<endl;
    cout<<endl<<endl<<"----->  output file: "<<endl<<"      root "<<outFileName<<endl<<endl;
  
}











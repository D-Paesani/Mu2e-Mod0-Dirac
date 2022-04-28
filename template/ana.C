#define ana_cxx

#include <fstream>
#include <chrono>
#include <TLine.h>
#include <iostream>
#include <list>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include <iomanip>

#include "TApplication.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "TGraphSmooth.h"
#include "TSpline.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1F.h"
#include "TF1.h"
#include "TButton.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TAxis.h"
#include "TMarker.h"
#include "TRandom.h"
#include "TMath.h"
#include "math.h"

#include "ana.h"
#include "vars.h"
#include "TGraphHelper.h"

using namespace std;

void skipNotAlive(TH1* histObj, int histN, int& histSkipFlag) { if (AN.res.isAlive[histN] == 0) {histSkipFlag=1;} };

void fuzzyTemp_proc(TH1* histObj, int histN, int& histSkipFlag) { 

   TString histName = histObj->GetName();
   if (histName.Contains("Master")) {
      histName = "fuzzyMaster";
   } else {
      if (!AN.res.isAlive[histN]) {  
         histSkipFlag = 1; 
         SplineTypeDef *teSpline = new SplineTypeDef();
         teSpline->SetName(Form("spline_%d", histN));
         AN.outDirs.spline->cd();
         teSpline->Write();
         return;
      }
   } 

   TCanvas *templDraw_can = new TCanvas(histName);
   AN.outDirs.templDraw->cd();  
   histObj->SetTitle(histName);
   histObj->SetDrawOption("zcol");
   histObj->Draw("zcol");
   templDraw_can->SetLogz();
   templDraw_can->Write();

   histName.ReplaceAll("fuzzy", "");
   TCanvas *spline_can = new TCanvas("spline" + histName); 
   spline_can->cd();
   TProfile *teProf = ((TH2*)histObj)->ProfileX(); 
   SplineTypeDef *teSpline = new SplineTypeDef(teProf);
   TGraphErrors *teSplGr = (TGraphErrors*)(((TH2*)histObj)->ProfileX());
   teProf->SetName("profile" + histName);  
   teSpline->SetName("spline" + histName);
   teSpline->SetLineColor(kOrange);
   teProf->Draw();
   teSpline->Draw("L same");
   AN.outDirs.profile->cd();  
   spline_can->Write();

   teSplGr->SetMarkerStyle(8);
   teSplGr->SetMarkerSize(0.5);
   teSplGr->SetMarkerColor(kBlue);
   teSplGr->SetLineColor(kOrange);

   AN.outDirs.splineGraph->cd();
   teSplGr->Write("graph" + histName);

   teSpline->SetName(Form("spline_%d", histN));
   AN.outDirs.spline->cd();
   teSpline->Write();
}

void times_proc(TH1* histObj, int histN, int& histSkipFlag) {

   TString histName = histObj->GetName();
   if (AN.res.isAlive[histN] == 0) {histSkipFlag=1; return;} 
   gStyle->SetOptFit(1111);

   double tpeak = histObj->GetBinCenter(histObj->GetMaximumBin());
   double tsigma = 5;
   double tmax = tpeak + 5, tmin = tpeak - 5;
   TF1 timeFit = TF1("gen", "gaus", tmin, tmax);
   timeFit.SetParameter(1, tpeak); timeFit.SetParameter(2, tsigma);
   histObj->Fit(&timeFit, "REMQ");
   tpeak = timeFit.GetParameter(1); tsigma = timeFit.GetParameter(2);
   timeFit.SetParameter(1, tpeak); timeFit.SetParameter(2, tsigma);
   timeFit.SetRange(tpeak - 4*tsigma, tpeak + 4*tsigma);
   histObj->Fit(&timeFit, "REMQ");
   tpeak = timeFit.GetParameter(1); tsigma = timeFit.GetParameter(2);
   double sigmaErr = timeFit.GetParError(2);
}

void times_proc_cry(TH1* histObj, int histN, int& histSkipFlag) {

   TString histName = histObj->GetName();
   if (histObj->GetEntries() < 5) {histSkipFlag=1; return;} 
   gStyle->SetOptFit(1111);

   double tpeak = histObj->GetBinCenter(histObj->GetMaximumBin());
   double tsigma = 5;
   double tmax = tpeak + 5, tmin = tpeak - 5;
   TF1 timeFit = TF1("gen", "gaus", tmin, tmax);
   timeFit.SetParameter(1, tpeak); timeFit.SetParameter(2, tsigma);
   histObj->Fit(&timeFit, "REMQ");
   tpeak = timeFit.GetParameter(1); tsigma = timeFit.GetParameter(2);
   timeFit.SetParameter(1, tpeak); timeFit.SetParameter(2, tsigma);
   timeFit.SetRange(tpeak - 4*tsigma, tpeak + 4*tsigma);
   histObj->Fit(&timeFit, "REMQ");
   tpeak = timeFit.GetParameter(1); tsigma = timeFit.GetParameter(2);
   double sigmaErr = timeFit.GetParError(2);

   // int cond1 = AN.anaOptions.Contains("optim(ps)") && histName.Contains("psDualReadout");
   // int cond2 = AN.anaOptions.Contains("optim(fit)") && histName.Contains("teDualReadout");
   // int cond3 = AN.anaOptions.Contains("optim(cf)") && histName.Contains("cfDualReadout");
   // if ( cond1 || cond2 || cond3 ) {
   //    fillGraph( &AN.optimGraphs[histN], *AN.optimVar, tsigma, 0, sigmaErr );
   // }

   if( AN.anaOptions.Contains("optim(") && !AN.anaOptions.Contains("optim(off)") ) {

      TGraphErrors *gra = histName.Contains("cfDualReadout") ? &AN.optimGraphs_cf[histN] : &AN.optimGraphs_ps[histN];
      gra = histName.Contains("teDualReadout") ? &AN.optimGraphs_te[histN] : gra;
      fillGraph( gra, *AN.optimVar, tsigma, 0, sigmaErr );
   }

}

void residuals_proc(TH1* histObj, int histN, int& histSkipFlag) {
   TString name = histObj->GetName();
   if (AN.res.isAlive[histN] == 0) {histSkipFlag=1; return;} 
   gStyle->SetOptFit(1111);
   TProfile *prof = ((TH2*)histObj)->ProfileX();
   prof->SetTitle(histObj->GetTitle());
   prof->SetName( name + "_prof");
   prof->Write();
   //histSkipFlag = 1;
}

void ana::Loop() {

   if (fChain == 0) {cout<<"No valid TTree!!!"<<endl; return;}

   //loadSplines
      if(AN.anaOptions.Contains("anaMode(fit)")) {
         cout<<"-----> Loading splines from "<<AN.splineFileName<<endl;
         AN.splineFile = new TFile(AN.splineFileName);
         for ( int k = 0; k < PRM.chNo; k++) {
            AN.splineFile->GetObject(Form(AN.splinesNameFormat, k), AN.splines[k]); 
         }
         cout<<endl;
      }
   //loadSplines

   //makeDirs
      AN.outFile->cd();
      AN.outDirs.specimens = AN.outFile->mkdir("specimens");
      AN.outDirs.optimisation = AN.outFile->mkdir("optimisation");
      if (AN.anaOptions.Contains("anaMode(gen)")) {
         AN.outDirs.spline = AN.outFile->mkdir("spline");
         AN.outDirs.splineGraph = AN.outFile->mkdir("splineGraph");
         AN.outDirs.templDraw = AN.outFile->mkdir("templDraw");
         AN.outDirs.profile = AN.outFile->mkdir("profile");
         AN.outDirs.preliminary = AN.outFile->mkdir("preliminary");
      }
   //makeDirs

   //histManager 
      cout<<"Creating HistBoxes:"<<endl;  
      AN.HM = new HistManager();
      AN.HM->SetOutFile(AN.outFile);
      AN.HM->SetNamerFun(&NamerChannel); 
      AN.HM->SetProcessFun(&skipNotAlive); 
      double eneBins = 800, eneFrom = 0, eneTo = 160;
      AN.HM->AddHistBox("th1f", PRM.chNo, "eneRaw", "e", "E", "MeV", eneBins, eneFrom, eneTo);
      AN.HM->AddHistBox("th1f", PRM.chNo, "chargePreliminary", "Q", "Q", "pC", PRM.qBins, PRM.qFrom, PRM.qTo);
      AN.HM->AddHistBox("th1f", PRM.chNo, "chargeRaw", "Q", "Q", "pC", PRM.qBins, PRM.qFrom, PRM.qTo);
      AN.HM->AddHistBox("th1f", PRM.chNo, "bLineRms", "Baseline rms", "", "mV", 100, 0.0, 5);
      AN.HM->AddHistBox("th1f", PRM.chNo, "bLine", "Baseline", "", "mV", 400, -5, 5); 
      AN.HM->AddHistBox("th1f", 1, "bLineAll", "Baseline", "", "mV", 100, -10, 10, AN.HM->GetProcDef()); 
      AN.HM->AddHistBox("th1f", PRM.chNo, "timesPseudo", "Template timesPseudo", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc);  
      AN.HM->AddHistBox("th1f", PRM.chNo, "timesPk", "Peak times", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc); 
      AN.HM->AddHistBox("th2f", PRM.chNo, "pseudoSlewing", "Pseudo time slewing", "E", "MeV", "T", "ns", eneBins, eneFrom, eneTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo);
      AN.HM->AddHistBox("th2f", PRM.chNo, "teSlewing", "Template time slewing", "E", "MeV", "T", "ns", eneBins, eneFrom, eneTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo); 
      AN.HM->AddHistBox("th1f", PRM.chNo, "timePseudoModBin", "Flatness over bin", "Normalised bin", "",  11, 0, 1.1);
      AN.HM->AddHistBox("th1f", PRM.chNo, "old", "old", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc); 
      if (AN.anaOptions.Contains("anaMode(fit)") || AN.anaOptions.Contains("anaMode(ps)")) {
         AN.HM->AddHistBox("th1f", PRM.chNo, "timesTe", "Template times", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc); 
         AN.HM->AddHistBox("th2f", PRM.chNo, "timesTe_timesPs", "templ vs ps", "Time", "ns", "Time", "ns", PRM.tiBins, PRM.tiFrom, PRM.tiTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo);
         AN.HM->AddHistBox("th2f", PRM.chNo, "newold", "newold", "Time", "ns", "Time", "ns", PRM.tiBins, PRM.tiFrom, PRM.tiTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo);
         AN.HM->AddHistBox("th1f", PRM.chNo, "timeTeModBin", "Flatness over bin", "Normalised bin", "",  11, 0, 1.1);
         AN.HM->AddHistBox("th1f", PRM.chNo, "teChi2","Chi2", "#chi^{2}/NDOF","", 200, 0, 15);
         AN.HM->AddHistBox("th2f", PRM.chNo, "teChi2_t", "Chi2 vs time", "T", "ns", "#chi^{2}/NDOF", "", PRM.tiBins, PRM.tiFrom, PRM.tiTo, 100, 0, 5); 
         AN.HM->AddHistBox("th2f", PRM.chNo, "teChi2_q", "Chi2 vs charge", "E", "MeV", "#chi^{2}/NDOF", "", eneBins, eneFrom, eneTo, 100, 0, 5); 
         AN.HM->AddHistBox("th2f", PRM.chNo, "teResidualsProf", "resid", "T - teT", "ns", "amplitude", "mV", 30, -15*PRM.digiTime, 15*PRM.digiTime, 1000, -100, 100, &residuals_proc); 
         AN.HM->AddHistBox("th1f", PRM.chNo, "teResiduals", "resid", "amp", "mV", 400, -20, 20); 
         AN.HM->AddHistBox("th2f", PRM.chNo, "teResiduals_charge", "resid", "E", "MeV", "res", "", eneBins, eneFrom, eneTo, 400, -20, 20); 
         AN.HM->AddHistBox("th2f", PRM.chNo, "teResiduals_time", "resid", "T", "ns", "res", "", PRM.tiBins, PRM.tiFrom, PRM.tiTo, 400, -20, 20); 
         AN.HM->AddHistBox("th1f", PRM.cryNo, "chargDualReadout", "psT", "E", "MeV", eneBins, eneFrom, eneTo, AN.HM->GetProcDef(), &NamerArray);
         AN.HM->AddHistBox("th1f", PRM.cryNo, "teDualReadout", "teT", "T", "ns", 800, -10, 10, &times_proc_cry, &NamerArray);
         AN.HM->AddHistBox("th1f", PRM.cryNo, "psDualReadout", "teT", "T", "ns", 800, -10, 10, &times_proc_cry, &NamerArray);
         AN.HM->AddHistBox("th1f", PRM.cryNo, "cfDualReadout", "teT", "T", "ns", 800, -10, 10, &times_proc_cry, &NamerArray);
         AN.HM->AddHistBox("th2f", PRM.cryNo, "psDualReadout:charge", "teT", "E", "MeV", "T", "ns", eneBins, eneFrom, eneTo, 800, -10, 10, AN.HM->GetProcDef(), &NamerArray);
         AN.HM->AddHistBox("th2f", PRM.cryNo, "teDualReadout:charge", "teT", "E", "MeV", "T", "ns", eneBins, eneFrom, eneTo, 800, -10, 10, AN.HM->GetProcDef(), &NamerArray);
         AN.HM->AddHistBox("th2f", PRM.chNo, "p0:charge", "p0_smearing", "E", "MeV", "p0", "mV", eneBins, eneFrom, eneTo, 1000, 0, 2000);
         AN.HM->AddHistBox("th2f", PRM.chNo, "p0:pkV", "p0_smearing", "Vpk", "mV", "p0", "mV", 1000, 0, 2000, 1000, 0, 2000);
         AN.HM->AddHistBox("th2f", PRM.chNo, "p0residuals:pkV", "", "Vpk", "mV", "resid", "", 1000, 0, 2000, 1000, -1, 1);
      } else if (AN.anaOptions.Contains("anaMode(gen)")) {
         AN.HM->AddHistBox("th2f", 1, "fuzzyMaster", "Fuzzy master", "Time", "ns", "Normalised Pulse", "", (int)AN.teTiBins, PRM.teTiFrom, PRM.teTiTo, PRM.teAmpBins, -0.1, 1.1, &fuzzyTemp_proc);
         AN.HM->AddHistBox("th2f", PRM.chNo, "fuzzy", "Fuzzy template", "Time", "ns", "Normalised Pulse", "", (int)AN.teTiBins, PRM.teTiFrom, PRM.teTiTo, PRM.teAmpBins, -0.1, 1.1, &fuzzyTemp_proc);
      }
      cout<<endl<<endl;
   //histManager

   //chargeEqual
      for (int i = 0; i < PRM.cryNo; i++) { 
         for (int ii = 0; ii < PRM.sdNo; ii++) {
            int cha = cry2chan(i, ii);
            TH1 *hist = (TH1*)AN.HM->GetHist("chargePreliminary", cha);
            branch2histo1d(hist, AN.chain, "Qval", Form("iCry == %d && SiPM == %d", i, ii)); 
            if (hist->GetEntries() < 10) { continue; }
            double peak = hist->GetBinCenter(hist->GetMaximumBin());
            double qfrom = 2000, qto = 12000;
            double sigma = 500;
            TF1 land = TF1("land", "landau", 3000, 10000);
            land.SetParameters(hist->Integral()/2, peak, sigma);
            hist->Fit(&land, "REMQ");
            land.SetParameters(land.GetParameter(0), peak, sigma);
            land.SetRange(peak - 1.5*sigma, peak + 4.5*sigma);
            hist->Fit(&land, "REMQ"); 
            hist->Fit(&land, "REMQ"); 
            peak = land.GetParameter(1); 
            sigma = land.GetParameter(2);
            AN.res.landauPks[i][ii] = peak;
         }
      }
   //chargeEqual
      
   //newTree
      TTree *_newTree;
      TFile *_newFile;
      const int _nhits = 20;  
      Double_t p_tePar_out[_nhits][3], p_teParErr_out[_nhits][3], p_teChi2_out[_nhits], p_teTcf_out[_nhits], p_teTfi_out[_nhits], p_psT_out[_nhits];
      if ( AN.anaOptions.Contains("anaMode(fit)") && AN.anaOptions.Contains("makeNewFile(on)") ) { 
         TString name = AN.inFileName.ReplaceAll(".root", "_new.root");
         cout<<endl<<"making new file -->    "<<name<<endl<<endl;
         _newFile = new TFile(name, "RECREATE");
         _newFile->cd();
         _newTree = (TTree*)AN.chain->CloneTree(0);
         _newTree->Branch("p_tePar", &p_tePar_out, "p_tePar_out[nHits][3]/D");
         _newTree->Branch("p_teParErr", &p_teParErr_out, "p_teParErr_out[nHits][3]/D");
         _newTree->Branch("p_teChi2", &p_teChi2_out, "p_teChi2_out[nHits]/D");
         _newTree->Branch("p_teTcf", &p_teTcf_out, "p_teTcf_out[nHits]/D");
         _newTree->Branch("p_teTfi", &p_teTfi_out,"p_teTfi_out[nHits]/D");
         _newTree->Branch("p_psT", &p_psT_out,"p_psT_out[nHits]/D");
      }
   //newTree

   //init
      gRandom->SetSeed();
      gStyle->SetOptFit(1111); 
   //init

   Long64_t nbytes = 0, nb = 0; 
   for (Long64_t jentry=0; jentry<AN.etp; jentry++) { 
      Long64_t ientry = LoadTree(jentry); 
      if (ientry < 0) { break;}
      nb = fChain->GetEntry(jentry); 
      nbytes += nb;
      if (!(jentry%1000)) {cout << Form( "     processing evt %lld / %lld  ( %.0f%% )", jentry, AN.etp, (float)(100*jentry/AN.etp) ) << endl;}
      double toss = gRandom->Uniform(0, AN.etp/50);

      double psDualReadoutDiff[2][PRM.cryNo]{0};
      double teDualReadoutDiff[2][PRM.cryNo]{0};
      double cfDualReadoutDiff[2][PRM.cryNo]{0};
      double chargDualReadout[2][PRM.cryNo]{0};

      double IntQ[PRM.chNo]{0},  PkV[PRM.chNo]{0},  PkT[PRM.chNo]{0},  TeT[PRM.chNo]{0}, PsT[PRM.chNo]{0};

      for (int ihit = 0; ihit < nHits; ihit++) { 
         
         int icry = iCry[ihit], irow = iRow[ihit], icol = iCol[ihit], isd = SiPM[ihit], ich = cry2chan(icry, isd);

         if ( (0 || AN.mode.Contains("optim(fit)")) && icry != 25 && icry != 41 && icry != 9) {continue;} //scorciatoia

         IntQ[ich] = Qval[ihit], PkV[ich] = Vmax[ihit], PkT[ich] = Tval[ihit];
         double intQ = Qval[ihit], pkV = Vmax[ihit], pkT = Tval[ihit]; 
         double ene = 21.00 * intQ / AN.res.landauPks[icry][isd];
         int nsam = nSamples[ihit];
         double tmin = tWave[ihit][0], tmax = tWave[ihit][nsam-1]; 
         AN.res.isAlive[ich] = 1;
         TString ctitl = Form("ev%lld_cr%d_sd%d", jentry, icry, isd);
         // add cut on saturated wfs
         AN.res.processedWfs[ich]++; 

         double blTmp{0}, brmsTmp{0};
         const int baseSam = 5;
         for (int k = 0; k < baseSam; k++) {
            double v = wave[ihit][k];
            blTmp += v;
            brmsTmp += v*v;
            AN.HM->Fill1d("bLineAll", 0, v);
         }  
         blTmp = blTmp/baseSam;
         brmsTmp = TMath::Sqrt(TMath::Abs(brmsTmp/baseSam - blTmp*blTmp));
         double ey = TMath::Sqrt(PRM.wfEy*PRM.wfEy + brmsTmp*brmsTmp);

         TGraphErrors waveGr = TGraphErrors(nsam); 
         for (int k = 0; k < nsam; k++) {  
            waveGr.SetPoint(k, tWave[ihit][k], wave[ihit][k]);
            waveGr.SetPointError(k, PRM.wfEx, ey);
         }

         SplineTypeDef waveSp = SplineTypeDef("wsp", &waveGr); 
         auto waveSpFun = [&waveSp](double *x, double *){ return waveSp.Eval(x[0]); }; 
         TF1 waveFitFun = TF1("fitf", waveSpFun, tmin, tmax, 0); 
         pkV = waveFitFun.GetMaximum(tmin, tmax);
         pkT = waveFitFun.GetMaximumX(tmin, tmax); 

         // TF1 peakFit = TF1("prakFit", "gaus", pkT - 16, pkT + 16);
         // peakFit.SetParameter(1, pkT);
         // peakFit.SetParameter(2, 5);
         // waveGr.Fit(&peakFit, "REMQ");
         // double pkFit = peakFit.GetMaximum();
         // pkV = pkFit;

         double thr = pkV*AN.CF;
         double psT = waveFitFun.GetX(thr); 
         p_psT_out[ihit] = psT;

         AN.HM->Fill1d("bLineRms", ich, brmsTmp);
         AN.HM->Fill1d("bLine", ich, blTmp);  
         AN.HM->Fill1d("chargeRaw", ich, intQ);
         AN.HM->Fill1d("eneRaw", ich, ene);
         AN.HM->Fill1d("timePseudoModBin", ich, psT/PRM.digiTime - (int)psT/PRM.digiTime);
         AN.HM->Fill1d("timesPk", ich, pkT - PRM.tiOff);
         AN.HM->Fill1d("timesPseudo", ich, psT - PRM.tiOff);
         AN.HM->Fill2d("pseudoSlewing", ich, ene, psT - PRM.tiOff);
         AN.HM->Fill1d("old", ich, templTime[ihit] - PRM.tiOff);

         if ( toss < 1 ) {
            AN.outDirs.specimens->cd();
            TCanvas cc("pseudo_" + ctitl); cc.cd(); 
            gStyle->SetOptFit(1111);
            waveGr.SetTitle("pseudo_" + ctitl);
            waveGr.SetLineWidth(0); waveGr.SetMarkerStyle(20); waveGr.SetMarkerSize(1); waveGr.SetMarkerColor(kBlue); waveGr.Draw("AP"); 
            waveFitFun.SetLineColor(kTeal); waveFitFun.Draw("same");
            waveSp.SetLineColor(kBlack); waveSp.Draw("same");
            TMarker tp = TMarker(psT, waveSp.Eval(psT), 2); tp.SetMarkerSize(3); tp.SetMarkerColor(kRed); tp.Draw("same"); 
            TLine t1 = TLine(tmin, 0, tmin, pkV); t1.SetLineColor(kRed); t1.Draw("same"); 
            TLine t2 = TLine(tmax, 0, tmax, pkV); t2.SetLineColor(kRed); t2.Draw("same");
            TLine t3 = TLine(tmin, pkV, tmax, pkV); t3.SetLineColor(kPink); t3.Draw("same");
            cc.Write();
         }

         double tref = AN.anaOptions.Contains("usePrevTempl(on)") ? (double)p_teTfi[ihit] - PRM.teOff : psT - PRM.teOff; 
         if ( AN.anaOptions.Contains("anaMode(gen)") && ene < 40 && ene > 10 ) { 
            for (int k = 0; k < nsam; k++) {  
               double wtime =  tWave[ihit][k] - tref;
               double wampl = wave[ihit][k]/pkV; 
               AN.HM->Fill2d("fuzzy", ich, wtime, wampl); 
               AN.HM->Fill2d("fuzzyMaster", 0, wtime, wampl); 
            } 
         }

         double teT = -9999;
         if (AN.anaOptions.Contains("anaMode(fit)")) { 
            auto tempSpFun = [&](Double_t *x, Double_t *par){ return par[0]*(AN.splines[ich]->Eval(x[0]-par[1]))+par[2]; };
            double tstart = tmin, tstop = pkT - AN.teFitStop;
            TF1 tempFun("tempFun", tempSpFun, tstart, tstop, 3); 
            tempFun.SetParameter(0, pkV);
            tempFun.SetParLimits(0, pkV*0.9, pkV*1.05); //controllare
            tempFun.SetParameter(1, psT - PRM.teOff); 
            //tempFun.SetParLimits(1, psT - PRM.teOff - 3, psT - PRM.teOff + 3); 
            tempFun.SetParameter(2,  0*blTmp);
            tempFun.SetParLimits(2, -10, 10); 
            gStyle->SetOptFit(1111);
            int fitr = waveGr.Fit( "tempFun", "REMQ" );  //F
            if (fitr<0) {cout<<"----------> templ fit failed: "<<fitr<<endl;}
            else { 
               for (int i = 0; i < 3; i++) {p_tePar_out[ihit][i] = tempFun.GetParameter(i); p_teParErr_out[ihit][i] = tempFun.GetParError(i);}
               p_teChi2_out[ihit] = tempFun.GetChisquare()/tempFun.GetNDF(); 
               p_teTfi_out[ihit] = tempFun.GetParameter(1) + PRM.teOff;
               p_teTcf_out[ihit] = tempFun.GetX(thr);
               teT = p_teTfi_out[ihit]; // check
               AN.HM->Fill1d("timesTe", ich, teT - PRM.tiOff); 
               AN.HM->Fill1d("timeTeModBin", ich, teT/PRM.digiTime - (int)teT/PRM.digiTime);
               AN.HM->Fill2d("teSlewing", ich, ene, teT - PRM.tiOff);
               AN.HM->Fill2d("teChi2_t", ich, teT - PRM.tiOff, p_teChi2_out[ihit]);
               AN.HM->Fill2d("teChi2_q", ich, ene, p_teChi2_out[ihit]);
               AN.HM->Fill1d("teChi2", ich, p_teChi2_out[ihit]);
               AN.HM->Fill2d("timesTe_timesPs", ich, psT - PRM.tiOff, teT - PRM.tiOff);
               AN.HM->Fill2d("newold", ich, templTime[ihit] - PRM.tiOff, teT - PRM.tiOff);
               AN.res.teGoodFits[ich]++;
               for (int i = 0; i < nsam; i++) {
                  double tim = waveGr.GetPointX(i), amp = waveGr.GetPointY(i), err = waveGr.GetErrorY(i), ampFit = tempFun.Eval(tim);
                  if (tim <= tstart) { continue; }
                  if (tim >= tstop) { break; }
                  double res = (ampFit - amp)/err;
                  AN.HM->Fill2d("teResidualsProf", ich, tim - teT, res);
                  AN.HM->Fill1d("teResiduals", ich, res);
                  AN.HM->Fill2d("teResiduals_time", ich, teT - PRM.tiOff, res);
                  AN.HM->Fill2d("teResiduals_charge", ich, ene, res);
                  AN.HM->Fill2d("p0:charge", ich, ene, tempFun.GetParameter(0));
                  AN.HM->Fill2d("p0:pkV", ich, pkV, tempFun.GetParameter(0));
                  AN.HM->Fill2d("p0residuals:pkV", ich, pkV, ( tempFun.GetParameter(0) - pkV ) / pkV );
               }
            }

            //if(brmsTmp == 0) { 
            //if ( (teT-psT)*(teT-psT) > 5  ) {
            if ( toss < 1 ) {
               AN.outDirs.specimens->cd();
               TCanvas cc("fit_" + ctitl); cc.cd(); 
               gStyle->SetOptFit(1111);
               waveGr.SetTitle("fit_" + ctitl);
               waveGr.SetLineWidth(0); waveGr.SetMarkerStyle(20); waveGr.SetMarkerSize(0.4); waveGr.SetMarkerColor(kBlue); waveGr.Draw("AP"); 
               tempFun.SetLineColor(kRed); tempFun.Draw("same"); 
               TMarker tp = TMarker(teT, tempFun.Eval(teT), 2);
               tp.SetMarkerSize(3); tp.SetMarkerColor(kRed); tp.Draw("same"); 
               TLine t1 = TLine(psT, 0, psT, pkV); t1.SetLineColor(kBlue); t1.Draw("same");
               TLine t2 = TLine(teT, 0, teT, pkV); t2.SetLineColor(kPink); t2.Draw("same");
               cc.Write(); 
            }
         }

         chargDualReadout[isd][icry] = ene;
         psDualReadoutDiff[isd][icry] = psT;
         teDualReadoutDiff[isd][icry] = p_teTfi_out[ihit];
         cfDualReadoutDiff[isd][icry] = p_teTcf_out[ihit];

      } //LOOP HIT 

      if (AN.anaOptions.Contains("anaMode(fit)") && AN.anaOptions.Contains("makeNewFile(on)")) {_newTree->Fill();}

      //addOn
         for (int i = 0; i < PRM.cryNo; i++) {

            if (AN.anaOptions.Contains("anaMode(gen)")) {break;}

            double psL = psDualReadoutDiff[0][i];
            double psR = psDualReadoutDiff[1][i];
            double teL = teDualReadoutDiff[0][i];
            double teR = teDualReadoutDiff[1][i];
            double cfL = cfDualReadoutDiff[0][i];
            double cfR = cfDualReadoutDiff[1][i];
            double chL = chargDualReadout[0][i];
            double chR = chargDualReadout[1][i];

            if ( chL > 30 ) {continue;}
            if ( chR > 30 ) {continue;}
            if ( chL < 15 ) {continue;}
            if ( chR < 15 ) {continue;}

            AN.HM->Fill1d("chargDualReadout", i, chL*0.5 + 0.5*chR);
            
            if ( psL == 0 || psR == 0) {continue;}

            AN.HM->Fill1d("psDualReadout", i, psR - psL);
            AN.HM->Fill2d("psDualReadout:charge", i, chL*0.5 + 0.5*chR, psR - psL);

            if ( teL < -1000 || teR < -1000 ) {continue;}
   
            AN.HM->Fill1d("teDualReadout", i, teR - teL);
            AN.HM->Fill1d("cfDualReadout", i, cfR - cfL);
            AN.HM->Fill2d("teDualReadout:charge", i, chL*0.5 + 0.5*chR, teR - teL);

         }
      //addOn

   } //LOOP EVENT

   //writeNewTree
      if (AN.anaOptions.Contains("anaMode(fit)")  && AN.anaOptions.Contains("makeNewFile(on)")) {
         _newFile->cd();
         _newTree->CloneTree()->Write();
         _newFile->Close();
      } 
   //writeNewTree

   //processHistos
      cout<<endl<<"Processing histograms..."<<endl;
      AN.HM->ProcessBoxes();
   //processHistos

   //printDiag
      cout<<endl;
      for (int i = 0; i < PRM.chNo; i++) {
         if (AN.res.isAlive[i] == 0) {continue;}
         double outeff = (float)(AN.res.teGoodFits[i]/AN.res.processedWfs[i])*100;
         cout << fixed << showpoint;
         cout<<"fitEfficiency["<<i<<"] = "<<std::setprecision(2)<<outeff<<" %"<<endl;
      }
   //printDiag

   //optimisation
      //if( AN.anaOptions.Contains("isLastIteration") && AN.anaOptions.Contains("optim(") && !AN.anaOptions.Contains("optim(off)") ) {
      if( AN.anaOptions.Contains("optim(") && !AN.anaOptions.Contains("optim(off)") ) {
         AN.outDirs.optimisation->cd();
         AN.outFile->cd(); //change here
         for ( int k = 0; k < PRM.cryNo; k++) {
            // if (AN.optimGraphs[k].GetN() < 1) { continue; }
            // AN.optimGraphs[k].Write(Form("optim_%d", k));
            if (AN.optimGraphs_ps[k].GetN() < 1) { continue; }
            AN.optimGraphs_ps[k].Write(Form("optimPs_%d", k));
            if (AN.optimGraphs_te[k].GetN() < 1) { continue; }
            AN.optimGraphs_te[k].Write(Form("optimTe_%d", k));            
            if (AN.optimGraphs_cf[k].GetN() < 1) { continue; }
            AN.optimGraphs_cf[k].Write(Form("optimCf_%d", k));
         }
      }
   //optimisation


}





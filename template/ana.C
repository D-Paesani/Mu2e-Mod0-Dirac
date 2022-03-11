#define ana_cxx

#include <fstream>
#include <chrono>
#include <TLine.h>
#include <iostream>
#include <list>
#include <algorithm>
#include <unistd.h>
#include <stdio.h>

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
#include "ROOT/RDataFrame.hxx"

#include "ana.h"
#include "pars.h"
#include "vars.h"
#include "NumberingHelper.h"
#include "HistManager.h"

using namespace std;
using namespace ROOT::Experimental;

Long64_t isAlive[chNo]{0};

AnaVars AN;
HistManager HM;

void skipNotAlive(TH1* histObj, int histN, int& histSkipFlag) { if (isAlive[histN] == 0) {histSkipFlag=1;} };

void fuzzyTemp_proc(TH1* histObj, int histN, int& histSkipFlag) { 

   TString histName = histObj->GetName();
   if (histName.Contains("Master")) {
      histName = "fuzzyMaster";
   } else {
      if (!isAlive[histN]) {  
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

   if (isAlive[histN] == 0) {histSkipFlag=1; return;} 
   gStyle->SetOptFit(1111);
   double tpeak = histObj->GetBinCenter(histObj->GetMaximumBin());
   double tmax = tpeak + 5, tmin = tpeak - 5;
   TF1 timeFit = TF1("g", "gaus", tmin, tmax); timeFit.SetParameter(1, tpeak); timeFit.SetParameter(2, 5);
   histObj->Fit(&timeFit, "REMQ");
   histObj->Fit(&timeFit, "REMQ");
   cout<<Form("----------->> Processed %s" , histObj->GetName());//<<endl;
   cout<<" -------------------------------------------------------->>>>>>>    "<<timeFit.GetParameter(2)<<", "<<timeFit.GetParError(2)<<endl<<endl;
}

void ana::Loop() {

   if (fChain == 0) return;

   HM.SetOutFile(AN.outFile);
   cout<<"Creating HistBoxes:"<<endl;  
   HM.SetNamerFun(&NamerChannel); 
   HM.SetProcessFun(&skipNotAlive); 
   HM.AddHistBox("th1f", PRM.chNo, "chargeRaw", "Q", "Q", "pC", PRM.qBins, PRM.qFrom, PRM.qTo);
   HM.AddHistBox("th1f", PRM.chNo, "bLineRms", "Base line rms", "", "mV", 100, 0.0, 5);
   HM.AddHistBox("th1f", PRM.chNo, "bLine", "Base line", "", "mV", 100, 5, 5); 
   HM.AddHistBox("th1f", PRM.chNo, "timesPseudo", "Template timesPseudo", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc);  
   HM.AddHistBox("th1f", PRM.chNo, "timesPk", "Peak times", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc); 
   HM.AddHistBox("th2f", PRM.chNo, "pseudoSlewing", "Pseudo time slewing", "Q", "pC", "T", "ns", PRM.qBins, PRM.qFrom, PRM.qTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo);
   HM.AddHistBox("th2f", PRM.chNo, "teSlewing", "Template time slewing", "Q", "pC", "T", "ns", PRM.qBins, PRM.qFrom, PRM.qTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo); 
   HM.AddHistBox("th1f", PRM.chNo, "timePseudoModBin", "Flatness over bin", "Normalised bin", "",  11, 0, 1.1);
   HM.AddHistBox("th1f", PRM.chNo, "old", "old", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc); 
   if (AN.mode == "f") {
      HM.AddHistBox("th1f", PRM.chNo, "timesTe", "Template times", "Time", "ns",  PRM.tiBins, PRM.tiFrom, PRM.tiTo, &times_proc); 
      HM.AddHistBox("th2f", PRM.chNo, "timesTe_timesPs", "templ vs ps", "Time", "ns", "Time", "ns", PRM.tiBins, PRM.tiFrom, PRM.tiTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo);
      HM.AddHistBox("th2f", PRM.chNo, "newold", "newold", "Time", "ns", "Time", "ns", PRM.tiBins, PRM.tiFrom, PRM.tiTo, PRM.tiBins, PRM.tiFrom, PRM.tiTo);
      HM.AddHistBox("th1f", PRM.chNo, "timeTeModBin", "Flatness over bin", "Normalised bin", "",  11, 0, 1.1);
      HM.AddHistBox("th1f", PRM.chNo, "teChi2","Chi2", "#chi^{2}/NDOF","", 200, 0, 15);
      HM.AddHistBox("th2f", PRM.chNo, "teChi2_t", "Chi2 vs time", "T", "ns", "#chi^{2}/NDOF", "", PRM.tiBins, PRM.tiFrom, PRM.tiTo, 100, 0, 5); 
      HM.AddHistBox("th2f", PRM.chNo, "teChi2_q", "Chi2 vs charge", "Q", "pC",  "#chi^{2}/NDOF", "", PRM.qBins, PRM.qFrom, PRM.qTo, 100, 0, 5); 
   } else if (AN.mode == "g") {
      HM.AddHistBox("th2f", 1, "fuzzyMaster", "Fuzzy master", "Time", "ns", "Normalised Pulse", "", PRM.teTiBins, PRM.teTiFrom, PRM.teTiTo, PRM.teAmpBins, -0.1, 1.1, &fuzzyTemp_proc);
      HM.AddHistBox("th2f", PRM.chNo, "fuzzy", "Fuzzy template", "Time", "ns", "Normalised Pulse", "", PRM.teTiBins, PRM.teTiFrom, PRM.teTiTo, PRM.teAmpBins, -0.1, 1.1, &fuzzyTemp_proc);
   }
   cout<<endl<<endl;

   TTree *_newTree;
   TFile *_newFile;
   const int _nhits = 20;  
   Double_t p_tePar[_nhits][3], p_teParErr[_nhits][3], p_teChi2[_nhits], p_teTcf[_nhits], p_teTfi[_nhits], p_psT[_nhits];
   if (AN.mode == "f" && AN.doMakeNewFile == "y") { 
      TString name = AN.inFileName.ReplaceAll(".root", "_new.root");
      cout<<endl<<"making new file -->    "<<name<<endl<<endl;
      _newFile = new TFile(name, "RECREATE");
      _newFile->cd();
      _newTree = (TTree*)AN.chain->CloneTree(0);
      _newTree->Branch("p_tePar", &p_tePar, "p_tePar[nHits][3]/D");
      _newTree->Branch("p_teParErr", &p_teParErr, "p_teParErr[nHits][3]/D");
      _newTree->Branch("p_teChi2", &p_teChi2, "p_teChi2[nHits]/D");
      _newTree->Branch("p_teTcf", &p_teTcf, "p_teTcf[nHits]/D");
      _newTree->Branch("p_teTfi", &p_teTfi,"p_teTfi[nHits]/D");
      _newTree->Branch("p_psT", &p_psT,"p_psT[nHits]/D");
   }

   Long64_t nbytes = 0, nb = 0; 
   for (Long64_t jentry=0; jentry<AN.etp; jentry++) { 
      Long64_t ientry = LoadTree(jentry); 
      if (ientry < 0) {break;}
      nb = fChain->GetEntry(jentry); 
      nbytes += nb;
      if (!(jentry%5000)) {cout << Form( "     processing evt %lld / %lld  ( %.0f%% )", jentry, AN.etp, (float)(100*jentry/AN.etp) ) << endl;}
      double toss = gRandom->Uniform(0, AN.etp/50);

      double IntQ[chNo]{0},  PkV[chNo]{0},  PkT[chNo]{0},  TeT[chNo]{0}, PsT[chNo]{0};

      for (int ihit = 0; ihit < nHits; ihit++) {
         
         AN.res.processedWfs++; 

         int icry = iCry[ihit], irow = iRow[ihit], icol = iCol[ihit], isd = SiPM[ihit], ich = cry2chan(icry, isd);

         IntQ[ich] = Qval[ihit], PkV[ich] = Vmax[ihit], PkT[ich] = Tval[ihit];
         double intQ = Qval[ihit], pkV = Vmax[ihit], pkT = Tval[ihit]; 
         int nsam = nSamples[ihit];
         double tmin = tWave[ihit][0], tmax = tWave[ihit][nsam-1]; 
         isAlive[ich] = 1;
         TString ctitl = Form("ev%lld_cr%d_sd%d", jentry, icry, isd);
         
         double blTmp{0}, brmsTmp{0};
         const int baseSam= 5;
         for (int k = 0; k < baseSam; k++) {
            double v = wave[ihit][k];
            blTmp += v;
            brmsTmp += v*v;
         }  
         blTmp = blTmp/baseSam;
         brmsTmp = TMath::Sqrt(TMath::Abs(brmsTmp/baseSam - blTmp*blTmp));
         double ey = TMath::Sqrt(PRM.wfEy*PRM.wfEy + brmsTmp*brmsTmp);
         HM.Fill1d("bLineRms", ich, brmsTmp);
         HM.Fill1d("bLine", ich, blTmp);  

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
         double thr = pkV*PRM.CF;
         double psT = waveFitFun.GetX(thr); 
         p_psT[ihit] = psT;

         HM.Fill1d("chargeRaw", ich, intQ);
         HM.Fill1d("timePseudoModBin", ich, psT/digiTime - (int)psT/digiTime);
         HM.Fill1d("timesPk", ich, pkT - PRM.tiOff);
         HM.Fill1d("timesPseudo", ich, psT - PRM.tiOff);
         HM.Fill2d("pseudoSlewing", ich, intQ, psT - PRM.tiOff);
         HM.Fill1d("old", ich, templTime[ihit] - PRM.tiOff);

         if ( toss < 1 ) {
            AN.outDirs.specimens->cd();
            TCanvas cc = TCanvas("pseudo_" + ctitl); cc.cd(); 
            gStyle->SetOptFit(1111);
            waveGr.SetTitle("pseudo_" + ctitl);
            waveGr.SetLineWidth(0); waveGr.SetMarkerStyle(20); waveGr.SetMarkerSize(1); waveGr.SetMarkerColor(kBlue); waveGr.Draw(""); 
            waveFitFun.SetLineColor(kTeal); waveFitFun.Draw("same");
            waveSp.SetLineColor(kBlack); waveSp.Draw("same");
            TMarker tp = TMarker(psT, waveSp.Eval(psT), 2); tp.SetMarkerSize(3); tp.SetMarkerColor(kRed); tp.Draw("same"); 
            TLine t1 = TLine(tmin, 0, tmin, pkV); t1.SetLineColor(kRed); t1.Draw("same"); 
            TLine t2 = TLine(tmax, 0, tmax, pkV); t2.SetLineColor(kRed); t2.Draw("same");
            TLine t3 = TLine(tmin, pkV, tmax, pkV); t3.SetLineColor(kPink); t3.Draw("same");
            cc.Write();
         }

         if ( AN.mode == "g" && intQ > PRM.teQFrom && intQ < PRM.teQTo ) { 
            for (int k = 0; k < nsam; k++) {  
               double wtime =  tWave[ihit][k] - psT + PRM.teOff;
               double wampl = wave[ihit][k]/pkV; 
               HM.Fill2d("fuzzy", ich, wtime, wampl); 
               HM.Fill2d("fuzzyMaster", 0, wtime, wampl); 
            } 
         }

         double teT = -9999;
         if (AN.mode == "f") { 
            auto tempSpFun = [&](Double_t *x, Double_t *par){ return par[0]*(AN.splines[ich]->Eval(x[0]-par[1]))+par[2]; };
            TF1 tempFun("tempFun", tempSpFun, tmin, psT + 20, 3); 
            tempFun.SetParameter(0, pkV);
            //tempFun.SetParLimits(0, pkV*0.85, pkV*1.15);
            tempFun.SetParameter(1, psT - PRM.teOff); 
            //tempFun.SetParLimits(1, psT - PRM.teOff - 3, psT - PRM.teOff + 3); 
            tempFun.SetParameter(2,  0);
            tempFun.SetParLimits(2, -10, 10); 
            gStyle->SetOptFit(1111);
            int fitr = waveGr.Fit( "tempFun", "REMQ" );  //F
            if (fitr<0) {cout<<"----------> templ fit failed: "<<fitr<<endl;}
            else { 
               for (int i = 0; i < 3; i++) {p_tePar[ihit][i] = tempFun.GetParameter(i); p_teParErr[ihit][i] = tempFun.GetParError(i);}
               p_teChi2[ihit] = tempFun.GetChisquare()/tempFun.GetNDF(); 
               p_teTcf[ihit] = tempFun.GetParameter(1) + PRM.teOff;
               p_teTfi[ihit] = tempFun.GetX(thr);
               teT = p_teTcf[ihit]; // check
               HM.Fill1d("timesTe", ich, teT - PRM.tiOff); 
               HM.Fill1d("timeTeModBin", ich, teT/digiTime - (int)teT/digiTime);
               HM.Fill2d("teSlewing", ich, intQ, teT - PRM.tiOff);
               HM.Fill2d("teChi2_t", ich, teT - PRM.tiOff, p_teChi2[ihit]);
               HM.Fill2d("teChi2_q", ich, intQ, p_teChi2[ihit]);
               HM.Fill1d("teChi2", ich, p_teChi2[ihit]);
               HM.Fill2d("timesTe_timesPs", ich, psT - PRM.tiOff, teT - PRM.tiOff);
               HM.Fill2d("newold", ich, templTime[ihit] - PRM.tiOff, teT - PRM.tiOff);
               AN.res.teGoodFits++;
               //aggiungere plot residui
            }

            //if(brmsTmp ==0) { 
            //if ( (teT-psT)*(teT-psT) > 5  ) {
            if ( toss < 1 && 0) {
               AN.outDirs.specimens->cd();
               TCanvas cc = TCanvas("fit_" + ctitl); cc.cd(); 
               gStyle->SetOptFit(1111);
               waveGr.SetTitle("fit_" + ctitl);
               waveGr.SetLineWidth(0); waveGr.SetMarkerStyle(20); waveGr.SetMarkerSize(0.4); waveGr.SetMarkerColor(kBlue); waveGr.Draw(); 
               tempFun.SetLineColor(kRed); tempFun.Draw("same"); 
               TMarker tp = TMarker(teT, tempFun.Eval(teT), 2);
               tp.SetMarkerSize(3); tp.SetMarkerColor(kRed); tp.Draw("same"); 
               TLine t1 = TLine(psT, 0, psT, pkV);  t1.SetLineColor(kBlue); t1.Draw("same");
               TLine t2 = TLine(teT, 0, teT, pkV);  t2.SetLineColor(kPink); t2.Draw("same");
               cc.Write(); 
            }
         }
      }   

      if (AN.mode == "f" && AN.doMakeNewFile == "y") {_newTree->Fill();}
      
   } 

   if (AN.mode == "f"  && AN.doMakeNewFile == "y") {
      _newFile->cd();
      _newTree->CloneTree()->Write();
      _newFile->Close();
   } 

   cout<<endl<<"Processing histograms..."<<endl;
   HM.ProcessBoxes();

}





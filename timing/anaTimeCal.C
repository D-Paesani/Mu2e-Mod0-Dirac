#pragma once

#include "anaTimeCal.h"
#define ANA_CLASS_NAME anaTimeCal

using namespace std; 

int ANA_CLASS_NAME::ParseHistVars(int histN, TString hOpt, void *aVars, int &cry, int &sid, int& row, int& col, int& used) {

   AnaVars *vars = (AnaVars*) aVars;
   used = 1; sid = -1;

   if (!hOpt.Contains("num->chan") && !hOpt.Contains("num->cry")) {return 0;}
 
   if (hOpt.Contains("num->chan")) {
      cry = vars->geo->cryCha(histN), sid = vars->geo->sideCha(histN);
      row = vars->geo->rowCry[cry], col = vars->geo->colCry[cry];
      used = vars->res->isUsed[cry][sid];
      return 1;
   } else if (hOpt.Contains("num->cry")) {
      cry = histN; 
      row = vars->geo->rowCry[cry], col = vars->geo->colCry[cry];
      used = vars->res->isUsed[cry][0] || vars->res->isUsed[cry][1];
      return 1;
   }

   return 0;
}

void ANA_CLASS_NAME::ProcessTimes(TH1* histObj, int histN, int& histSkipFlag, void *aVars, TString hOpt) { 

   AnaVars *vars = (AnaVars*) aVars;
   TString name = histObj->GetName(); 
   
   int row{0}, col{0}, cry{0}, sid{0}, used{0};
   ParseHistVars(histN, hOpt, aVars, cry, sid, row, col, used);
   if (used == 0) {histSkipFlag = 1; return;}
   if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
  
   gStyle->SetOptFit(1111);
   histObj->SetStats(111111);
   double peak = histObj->GetBinCenter(histObj->GetMaximumBin()), sigma = histObj->GetRMS();
   TF1 timeFit = TF1("g", "gaus", peak - 100, peak + 100);
   timeFit.SetParameter(1, peak); timeFit.SetParameter(2, min(sigma, 10.0));
   histObj->Fit(&timeFit, "EMQ");
   peak = timeFit.GetParameter(1); sigma = timeFit.GetParameter(2);
   timeFit.SetParameter(1, peak); timeFit.SetParameter(2, sigma);
   timeFit.SetRange(peak - 2.2*sigma, peak + 2.2*sigma); 
   histObj->Fit(&timeFit, "REMQ");
   peak = timeFit.GetParameter(1); sigma = timeFit.GetParameter(2);
   double peakErr = timeFit.GetParError(1), sigmaErr = timeFit.GetParError(2); 

   if ( vars->conf->step == 0 && name.Contains("timSelVert") ) { 
      vars->cal->timeOffOut[cry][sid] = peak;
      vars->cal->timeOffErrOut[cry][sid] = peakErr;
   };

   if ( vars->conf->step > 0 && name.Contains("resid") ) { 
      double nplot = (double)(1 + cry + 0.25 * (2*sid-1)) + 0.025*vars->conf->step;
      vars->cal->timeResidOut[cry][sid] = peak;  
      vars->cal->timeResidErrOut[cry][sid] = peakErr; 
      vars->cal->timeResidRmsOut[cry][sid] = sigma;
      vars->cal->timeResidRmsErrOut[cry][sid] = sigmaErr;
      vars->res->residMean->SetPoint(histN, nplot, peak);
      vars->res->residMean->SetPointError(histN, 0, min(0.5, peakErr));
      vars->res->residRms->SetPoint(histN, nplot, sigma);
      vars->res->residRms->SetPointError(histN, 0, min(1.0, sigmaErr));
      vars->res->residEntries->SetPoint(histN, nplot, (double)histObj->GetEntries()); 
   } 

};

void ANA_CLASS_NAME::SkipUnused(TH1* histObj, int histN, int& histSkipFlag, void *aVars, TString hOpt) { 

   if (hOpt.Contains("write->skip")) { histSkipFlag = 1; return; }
   if (!hOpt.Contains("num->chan") && !hOpt.Contains("num->cry")) {return;}
   int row{0}, col{0}, cry{0}, sid{0}, used{0};
   int flag = ParseHistVars(histN, hOpt, aVars, cry, sid, row, col, used);
   if (used == 0) {histSkipFlag = 1; return;}

};

void ANA_CLASS_NAME::NamerDefault(int histN, TString& hTag, TString& hTitleTag, void *aVars, TString hOpt) { 
    
   if (hOpt.Contains("tag->off")) { hTag = "";  hTitleTag = "";  return; }
   if (hOpt.Contains("tag->arr")) { hTag = Form("_%d", histN); hTitleTag = Form(" [%d]", histN); }

   int row{0}, col{0}, cry{0}, sid{0}, used{0};
   if (ParseHistVars(histN, hOpt, aVars, cry, sid, row, col, used)) {
      if (hOpt.Contains("tag->rcs")) { 
         hTag = Form("_%d%s_r%d_c%d_s%d", cry, sid==0?"L":"R", row, col, sid); 
         hTitleTag = Form(" [%d%s][%d|%d|%s]", cry, sid==0?"L":"R", row, col, sid==0?"L":"R"); 
         return; 
      }
      if (hOpt.Contains("tag->rc")) { 
         hTag = Form("_%d_r%d_c%d", cry, row, col); 
         hTitleTag = Form(" [%d][%d|%d]", cry, row, col); 
      return; 
      }
   }

   hTag = Form("_%d", histN); hTitleTag = Form(" [%d]", histN);

}

void ANA_CLASS_NAME::Begin() {

   //Histograms
      HM.SetOutFile(CONF->outFile);
      cout<<"Creating HistBoxes:"<<endl;  
      HM.SetAnaVariables(&VAR);
      HM.SetUserDefNamer(&(ANA_CLASS_NAME::NamerDefault)); 
      HM.SetUserDefProcess(&(ANA_CLASS_NAME::SkipUnused)); 
      int qBins = 100, tBins = 500;
      double qFrom = 0, qTo = 100, tFrom = 0, tTo = 500;
      HM.AddHistBox("th1f", 3, "timDiffCellTof", "timeDiffCellTof", "T", "ns", 400, -10, 10, "num->arr tag->arr", &ANA_CLASS_NAME::ProcessTimes);
      HM.AddHistBox("th1f", 3, "timDiffCell", "timeDiffCell", "T", "ns", 400, -10, 10, "num->arr tag->arr", &ANA_CLASS_NAME::ProcessTimes);
      HM.AddHistBox("th1f", GEO.chNo, "eneEqPreliminary", "eq", "E", "MeV", 800, 0, 20000, "num->chan tag->rcs");
      HM.AddHistBox("th1f", GEO.chNo, "timSelVert", "t0 vert", "T", "ns", tBins, tFrom, tTo, "num->chan tag->rcs", &ANA_CLASS_NAME::ProcessTimes); //RES.usedChanNo;
      HM.AddHistBox("th1f", GEO.chNo, "timSelDiag", "t0 diag", "T", "ns", tBins, tFrom, tTo, "num->chan tag->rcs", &ANA_CLASS_NAME::ProcessTimes);
      HM.AddHistBox("th1f", GEO.chNo, "timSelAll", "t0 all", "T", "ns", tBins, tFrom, tTo, "num->chan tag->rcs", &ANA_CLASS_NAME::ProcessTimes);
      HM.AddHistBox("th1f", GEO.chNo, "eneSelVert", "en vert", "E", "MeV", qBins, qFrom, qTo, "num->chan tag->rcs");
      HM.AddHistBox("th1f", GEO.chNo, "eneSelDiag", "en diag", "E", "MeV", qBins, qFrom, qTo, "num->chan tag->rcs");
      HM.AddHistBox("th1f", GEO.chNo, "eneSelAll", "en all", "E", "MeV", qBins, qFrom, qTo, "num->chan tag->rcs");
      HM.AddHistBox("th1f", GEO.rowNo, "eneSelLayer", "eneLayer", "E", "MeV", qBins, qFrom, qTo, "num->arr tag->arr");
      HM.AddHistBox("th1f", GEO.cryNo, "timeDiffSel", "timeDiffs", "", "", 400, -10, 10, "num->cry tag->rc", &ANA_CLASS_NAME::ProcessTimes);
      HM.AddHistBox("th2f", GEO.chNo, "timSelAll:ene", "time vs charge", "E", "MeV", "T", "ns", qBins, qFrom, qTo, tBins, tFrom, tTo, "num->chan tag->rcs");
      if (CONF->step > 0) { 
         HM.AddHistBox("th1f", GEO.cryNo, "timeDiffCut", "timeDiffs", "T", "ns", 400, -10, 10, "num->cry tag->rc", &ANA_CLASS_NAME::ProcessTimes);
         HM.AddHistBox("th1f", GEO.chNo, "resid", "residuals", "T", "ns", 250, -10, 10, "num->chan tag->rcs", &ANA_CLASS_NAME::ProcessTimes);
         HM.AddHistBox("th2f", GEO.chNo, "resid:ene", "residuals", "E", "MeV", "T", "ns", qBins, qFrom, qTo, 250, -10, 10, "num->chan tag->rcs");
         HM.AddHistBox("th1f", GEO.cryNo, "eneCry", "eneCry", "E", "MeV", 200, 0, 100, "num->cry tag->rc");
         HM.AddHistBox("th1f", 1, "cosTheta", "cosTheta", "cosine", "", 400, 0.6, 1, "num->arr tag->off");
         HM.AddHistBox("th1f", 1, "cosThetaFit", "cosThetaFit", "cosine", "", 400, 0.6, 1, "num->arr tag->off");
         HM.AddHistBox("th1f", 1, "cosThetaDiff", "cosThetaDiff", "cosineLS-cosineFit", "", 200, -0.1, 0.1, "num->arr tag->off");
         HM.AddHistBox("th1f", 1, "eneCellCut", "eneCell", "E", "MeV", 200, 0, 100, "num->arr tag->off");
         HM.AddHistBox("th1f", 1, "eneAvgCut", "EneMean", "E", "MeV", 200, 0, 100, "num->arr tag->off");
         HM.AddHistBox("th1f", 1, "fitchi2", "trajFitchi2", "chi2", "", 250, 0, 50, "num->arr tag->off");
         HM.AddHistBox("th2f", 1, "resid:eneCell", "residuals", "E", "MeV", "T", "ns", qBins, qFrom, qTo, 250, -10, 10, "num->arr tag->off");
         HM.AddHistBox("th2f", 1, "resid:nCell", "residuals", "nCell", "", "T", "ns", 40, 0, 40, 250, -10, 10, "num->arr tag->off");
         HM.AddHistBox("th2f", 1, "resid:col", "residuals", "col", "", "T", "ns", GEO.colNo, 0, GEO.colNo, 250, -10, 10, "num->arr tag->off");
         HM.AddHistBox("th2f", 1, "resid:row", "residuals", "row", "", "T", "ns", GEO.rowNo, 0, GEO.colNo, 250, -10, 10, "num->arr tag->off");
      }
      cout<<endl<<endl;
   //Histograms

   //energyEqRead
      if ( CONF->chargeCalFileName != "" && CONF->options.Contains("equal->fromFile")) {
         cout<<"Retrieving energy calibration from "<<CONF->chargeCalFileName<<endl;
         ifstream fin;
         fin.open(CONF->chargeCalFileName);
         if(!fin.is_open()) {
            cout<<"ERROR: could not open file"<<endl;
            exit(EXIT_FAILURE); 
         }
         int count = 0;
         for (int i = 0; i < GEO.chNo; i++) {
            int row, col, side, cry;
            float dataIn, dataInErr;
            if (!(fin >> row >> col >> side >> dataIn >> dataInErr)) {break;}
            cry = GEO.cryRC[row][col];
            CAL.chargeEq[cry][side] = dataIn;
            CAL.chargeEqErr[cry][side] = dataInErr; 
            RES.isUsed[cry][side] = 1; 
            RES.readoutType[cry] += 1;
            count++;
         }
         fin.close();  
         RES.usedChanNo = count; 
         cout<<"...done"<<endl<<endl;
      }
   //energyEqRead

   //EneEq 
      if (CONF->options.Contains("equal->fromTree")) {
         cout<<"Performing energy equalisation on MIP peaks "<<endl<<endl;
         for (int i = 0; i < GEO.chNo; i++) {  
            TH1 *hist = (TH1*)HM.GetHist("eneEqPreliminary", i);
            int icry = GEO.cryCha(i), isid = GEO.sideCha(i);
            branch2histo1d(hist, CONF->chain, "Qval", Form("iCry == %d && SiPM == %d", icry, isid)); 
            if (hist->GetEntries() < 10) { continue; }
            double peak = hist->GetBinCenter(hist->GetMaximumBin());
            double qfrom = 2000, qto = 12000;
            double sigma = 500;
            TF1 land = TF1("land", "landau", qfrom, qto);
            land.SetParameters(hist->Integral()/2, peak, sigma);
            hist->Fit(&land, "REMQ"); 
            peak = land.GetParameter(1); 
            sigma = land.GetParameter(2);
            land.SetParameters(land.GetParameter(0), peak, sigma);
            land.SetRange(peak - 1.5*sigma, peak + 4.5*sigma);
            hist->Fit(&land, "REMQ"); 
            CAL.chargeEq[icry][isid] = land.GetParameter(1);
            CAL.chargeEqErr[icry][isid] = land.GetParError(1);
            RES.isUsed[icry][isid] = 1; 
         }
         cout<<"...done"<<endl<<endl;
      }
   //EneEq

   //Plots
      RES.residMean = new TGraphErrors(GEO.chNo);
      RES.residRms = new TGraphErrors(GEO.chNo);
      RES.residEntries = new TGraphErrors(GEO.chNo);
   //Plots

}

void ANA_CLASS_NAME::Terminate() {

   //Histograms
      cout<<endl<<"Processing histograms..."<<endl<<endl<<endl;
      HM.ProcessBoxes();
      cout<<"...done"<<endl<<endl;
   //Histograms

   //saveFinalCalib
      TString fname = CONF->timeCalOutFileName;
      if ( fname != "" ) {
         cout<<"Writing time offsets to "<<fname<<endl;
         ofstream fileOut;
         fileOut.open(fname); 
         for (int i = 0; i < GEO.chNo; i++) {
            int cry = GEO.cryCha(i), sid = GEO.sideCha(i);
            if (!RES.isUsed[cry][sid]) {continue;}
            double val{0}, err{0};
            val = CAL.timeResidOut[cry][sid] + CAL.timeOff[cry][sid];
            err = CAL.timeResidErrOut[cry][sid];
            fileOut <<  std::fixed << setprecision(PRM.csvPrecision);
            fileOut << cry << PRM.csvSeparator; 
            fileOut << sid << PRM.csvSeparator;
            fileOut << val << PRM.csvSeparator;
            fileOut << err << PRM.csvSeparator;
            fileOut << '\n';	 
         }
         fileOut.close(); 
         cout<<"...done"<<endl<<endl;
      } 
   //saveFinalCalib

   //Plots
      if (CONF->mode > 0) {
         CONF->outDirs.calibration->cd();
         RES.residMean->SetLineColor(kBlue);
         RES.residMean->SetMarkerColor(kBlue);
         RES.residMean->SetMarkerStyle(20);
         RES.residMean->GetYaxis()->SetTitle("time [ns]");
         RES.residMean->GetXaxis()->SetTitle("1 + iCry + 0.25*( 2*iSide - 1)");
         RES.residMean->SetNameTitle(Form("TimeCalibMean_step%d", CONF->step), Form("step%d", CONF->step));
         RES.residMean->Write();
         RES.residRms->SetLineColor(kBlue);
         RES.residRms->SetMarkerColor(kBlue);
         RES.residRms->SetMarkerStyle(20);
         RES.residRms->GetYaxis()->SetTitle("time [ns]");
         RES.residRms->GetXaxis()->SetTitle("1 + iCry + 0.25*( 2*iSide - 1)");
         RES.residRms->SetNameTitle(Form("TimeCalibRms_step%d", CONF->step), Form("step%d", CONF->step));
         RES.residRms->Write();
         RES.residEntries->SetLineColor(kBlue);
         RES.residEntries->SetMarkerColor(kBlue);
         RES.residEntries->SetMarkerStyle(20);
         RES.residEntries->GetYaxis()->SetTitle("# of processed hits");
         RES.residEntries->GetXaxis()->SetTitle("1 + iCry + 0.25*( 2*iSide - 1)");
         RES.residEntries->SetNameTitle(Form("TimeCalibEntries_step%d", CONF->step), "TimeCalibEntries");
         RES.residEntries->Write();
      }
   //Plots

   //anaFile
      CONF->outFile->Close();
      cout<<endl<<endl;
      cout<<endl<<endl<<"----->  output file: "<<endl<<"      root "<<CONF->outFileName<<endl<<endl;
   //anaFile

}

void ANA_CLASS_NAME::LoopEntries(Long64_t entry) { 

   MIP.FilterDiagVertInit();
   MIP.SelectionInit();

   double hitMax = nHits;
   if (hitMax == 0) {return;} 
   Event eve(hitMax);

   for (int ihit = 0; ihit < hitMax; ihit++) { // parse event

      int cry = iCry[ihit], row = iRow[ihit], col = iCol[ihit], sid = SiPM[ihit];
      int read = RES.readoutType[cry];

      if (sid < 0 || sid > 1) {continue;}

      double ene = Qval[ihit]*PRM.pC2MeV/CAL.chargeEq[cry][sid]; 
      double tim = p_psT[ihit];
      double timCorr = tim - CAL.timeOff[cry][sid];  

      int idx = eve.AddHit( cry, sid, 0, ene, 0, 0, tim, timCorr ); 

      RES.processedHits++; 
   } 

   int hitNo = eve.cryNo;

   for (int hit = 0; hit < hitNo; hit++) { // prepare cuts

      MIP.FilterDiagVertAdd(eve.cry[hit], eve.eneMean[hit]);
      MIP.SelectionAdd(hit, eve.cry[hit], eve.eneMean[hit], eve.timMean[hit], eve.timCorrMean[hit]);

   } 

   TString cosmicType = MIP.FilterDiagVertGetAnsw();
   if( cosmicType  == "no" && CONF->step == 0 ) { return; } 

   double eneLayer[GEO.rowNo]{0};
   for (int hit = 0; hit < hitNo; hit++) {  // fill layers
      for (int ilayer = 0; ilayer < GEO.rowNo; ilayer++) {
         if (eve.row[hit] == ilayer) { eneLayer[ilayer] += eve.eneMean[hit]; }
      }
   }   
   for (int ilayer = 0; ilayer < GEO.rowNo; ilayer++) { 
      HM.Fill1d("eneSelLayer", ilayer, eneLayer[ilayer]); 
   }  

   for (int hit = 0; hit < hitNo; hit++) { // fill selection
      int cha = eve.chan[hit], cry = eve.cry[hit];
      double ene =  eve.eneMean[hit], tim = eve.timMean[hit];

      HM.Fill1d("eneSelAll", cha, ene); 
      HM.Fill2d("timSelAll:ene", cha, ene, tim); 
      HM.Fill1d("timSelAll", cha, tim);
      if (cosmicType == "v") { HM.Fill1d("eneSelVert", cha, ene); HM.Fill1d("timSelVert", cha, tim); } 
      if (cosmicType == "d") { HM.Fill1d("eneSelDiag", cha, ene); HM.Fill1d("timSelDiag", cha, tim); } 

      if (eve.side[hit] == 2) {

         HM.Fill1d("timeDiffSel", cry, eve.timCorr[1][hit] - eve.timCorr[0][hit]);

      }
   } 
   
   if (CONF->step == 0) {return;}
   if( cosmicType  == "no" && CONF->options.Contains("sel->all")) { return; } 
   if ( (cosmicType != "v" && CONF->options.Contains("sel->vert")) || (cosmicType == "d" && CONF->options.Contains("sel->diag")) ) {return;}
   if (!MIP.SelectionCheckNum()) {return;}

   int selNo = MIP.sel.count;
   
   MIP.SelectionComputeCosTheta();
   MIP.SelectionComputeWeightedSum();
   MIP.FitTrajectory();

   HM.Fill1d("cosTheta", 0, MIP.sel.cosTheta);
   HM.Fill1d("cosThetaFit", 0, MIP.sel.fitCosTheta);
   HM.Fill1d("cosThetaDiff", 0, MIP.sel.fitCosTheta - MIP.sel.cosTheta);
   HM.Fill1d("fitchi2", 0, MIP.sel.fitChi2);

   if( !MIP.SelectionCutCosTheta() ) {return;}
   if( !MIP.SelectionCutCosThetaFit() ) {return;}
   if (!MIP.FitChi2Cut()) {return;}

   for (int hit = 0; hit < hitNo; hit++) { // fill cut
      if (eve.side[hit] == 2) {
         HM.Fill1d("timeDiffCut", eve.cry[hit], eve.timCorr[1][hit] - eve.timCorr[0][hit]);
      }
   } 

   for (int i = 0; i < selNo; i++) { // fill ene cell
      HM.Fill1d("eneCellCut", 0, MIP.sel.E[i]);
      HM.Fill1d("eneCry", MIP.sel.cry[i], MIP.sel.E[i]);
   }

   double toff = MIP.SelectionGetTimeWeightEvent();
   double costheta = MIP.sel.cosTheta;

   HM.Fill1d("eneAvgCut", 0, MIP.sel.etot / selNo);

   for (int iSel = 0; iSel < selNo; iSel++) { // fill resid
      
      int cry = MIP.sel.cry[iSel], row = MIP.sel.row[iSel], col = MIP.sel.col[iSel];
      double ene = MIP.sel.E[iSel];
      int hit = MIP.sel.hit[iSel];

      for (int sid = 0; sid < GEO.sidNo; sid++) {

         if (eve.side[hit] != 2 && eve.side[hit] != sid) {continue;}

         int cha = GEO.chaCry(cry, sid);

         double resid = eve.timCorr[sid][hit] + MIP.sel.y[iSel] / ( PRM.lightSpeed * costheta ) - toff;

         HM.Fill1d("resid", cha, resid);
         HM.Fill2d("resid:ene", cha, eve.ene[sid][hit], resid);     
         HM.Fill2d("resid:eneCell", 0, ene, resid);     
         HM.Fill2d("resid:row", 0, row, resid);      
         HM.Fill2d("resid:col", 0, col, resid);      
         HM.Fill2d("resid:nCell", 0, selNo, resid);      
      }

   }



   //prova
      double t9{-9999}, t25{-9999}, t41{-9999};
      for (int hit = 0; hit < hitNo; hit++) { 
         
         t9 = eve.cry[hit] == 9 ? eve.timCorrMean[hit] : t9;
         t25 = eve.cry[hit] == 25 ? eve.timCorrMean[hit] : t25;
         t41 = eve.cry[hit] == 41 ? eve.timCorrMean[hit] : t41;

         if ( t9 > -1000 && t25 > - 1000 && t41 > -1000 ) {

            HM.Fill1d("timDiffCell", 0, t25-t9);
            HM.Fill1d("timDiffCell", 1, t41-t9);
            HM.Fill1d("timDiffCell", 2, t41-t25);

            double t25c = t25 + GEO.cryY[GEO.rowCry[25]][GEO.colCry[25]] / ( PRM.lightSpeed * costheta ) - toff;
            double t41c = t41 + GEO.cryY[GEO.rowCry[41]][GEO.colCry[41]] / ( PRM.lightSpeed * costheta ) - toff;
            double t9c = t9 + GEO.cryY[GEO.rowCry[9]][GEO.colCry[9]] / ( PRM.lightSpeed * costheta ) - toff;

            HM.Fill1d("timDiffCellTof", 0, t25c-t9c);
            HM.Fill1d("timDiffCellTof", 1, t41c-t9c);
            HM.Fill1d("timDiffCellTof", 2, t41c-t25c);
         }
      } 
   //prova

   









   






}









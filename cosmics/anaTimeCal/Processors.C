#pragma once
using namespace std; 

#include "anaTimeCal.h"
#define ANA_CLASS_NAME anaTimeCal

void ANA_CLASS_NAME::ProcessLandau(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   AnaVars *vars = (AnaVars*) anaVars;
   TString name = histObj->GetName(); 
   
   int row{0}, col{0}, cry{0}, sid{0}, used{0};
   ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
   
   if (used == 0) {histSkipFlag = 1; return;}
   if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}

   double peak, sigma, peakErr, sigmaErr;
   fitEneLandau(histObj, peak, peakErr, sigma, sigmaErr, "none");

   if ( name.Contains("eneSelMip") ) { 
      vars->CAL.eneEq[cry][sid] = peak;
      vars->CAL.eneEqErr[cry][sid] = peakErr;
   }
}

void ANA_CLASS_NAME::ProcessTimes(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   AnaVars *vars = (AnaVars*) anaVars;
   TString name = histObj->GetName(); 
   
   int row{0}, col{0}, cry{0}, sid{0}, used{0};
   ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
   
   if (used == 0) {histSkipFlag = 1; return;}
   if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
  
   gStyle->SetOptFit(1111111);
   histObj->SetStats(1111111);

   double peak, sigma, peakErr, sigmaErr;
   fitTimeGausStandard(histObj, peak, peakErr, sigma, sigmaErr, "none");

   if ( vars->CONF.step == 0 && name.Contains("timSelVert") ) { 
      vars->CAL.timeOffOut[cry][sid] = peak;
      vars->CAL.timeOffErrOut[cry][sid] = peakErr;
   }

   if ( vars->CONF.step > 0 && name.Contains("resid") ) { 
      double nplot = 1 + (double)cry + 0.20 * (2*(double)sid-1);
      vars->CAL.timeResidOut[cry][sid] = peak;  
      vars->CAL.timeResidErrOut[cry][sid] = peakErr; 
      vars->CAL.timeResidRmsOut[cry][sid] = sigma;
      vars->CAL.timeResidRmsErrOut[cry][sid] = sigmaErr;

      vars->hm->FillGraph("residMean", 0, nplot, peak, 0, min(0.5, peakErr) ); //togliere min dopo aver tolto sergio
      vars->hm->FillGraph("residRms", 0, nplot, sigma, 0, min(1.0, sigmaErr) );
      vars->hm->FillGraph("residEntries", 0, nplot, (double)histObj->GetEntries(), 0, 0 );
      vars->hm->Fill1d("residHisto", 0, peak);

   } 
}

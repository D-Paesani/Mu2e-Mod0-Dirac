#pragma once
using namespace std; 

#include "anaPlots.h"
#define ANA_CLASS_NAME anaPlots

//void ANA_CLASS_NAME::  (TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { }

void ANA_CLASS_NAME::ProcessSliceChargeZ(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 
   
   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;} 
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}  
   
      gStyle->SetOptFit(1);
      histObj->SetStats(1111111); 
   //preliminary

   //slicingPrep
      TDirectory *curr = gDirectory->GetDirectory(0);
      TDirectory *newdir = curr->mkdir("slices", "", true);
      TDirectory *newdir2 = curr->mkdir("ene.z", "", true);
      TDirectory *newdir3 = curr->mkdir("eneRes.z", "", true);
      TDirectory *newdir4 = curr->mkdir("th2", "", true);
      TDirectory *newdir5 = curr->mkdir("profile", "", true);

      newdir4->cd();
      histObj->Write();
      histSkipFlag = 1;
      newdir5->cd();
      ((TH2F*)histObj)->ProfileX()->Write();
      newdir->cd();

      Ssiz_t ss = 0;
      TString targetName =  "";
      TString me = name(0, name.Length());
      me.Tokenize(targetName, ss, "_");
      TString targetName2 = "res" + targetName; 
   //slicingPrep

   int bn = histObj->GetNbinsX();
   for (int i = 0; i < bn; i++) {

      histObj->GetXaxis()->SetRange(i+1, i+1);

      TH1F* islice = (TH1F*)((TH2F*)histObj)->ProjectionY();
      islice->SetNameTitle(Form(name + "_slice_%d", i), Form(name + "_slice_%d", i));
      if(islice->Integral() < 50) { continue; }
       
      double peak, sigma, peakErr, sigmaErr;
      int res = fitEneLandau(islice, peak, peakErr, sigma, sigmaErr, "none");
      if (!res) { continue; }
      if (TMath::Abs(peakErr/peak) > 0.1) {continue;}
      islice->Write();

      double zval = histObj->GetXaxis()->GetBinCenter(i+1);
      double zvale = histObj->GetXaxis()->GetBinWidth(i+1)/3.6;
      double cryPos = histN + 0.5 * (((double)i+1)/(double)bn - 0.5);
      double cryPosE = 0;

      vars->hm->FillGraph(targetName.Data(), histN, zval, peak, zvale, peakErr);
      vars->hm->FillGraph(targetName2.Data(), histN, zval, sigma/peak, zvale, (sigma/peak)*TMath::Sqrt(sigmaErr*sigmaErr/(sigma*sigma) + peakErr*peakErr/(peak*peak)) );
      vars->hm->FillGraph("all_ene.z", 0, cryPos, peak, cryPosE, 0*peakErr);
      vars->hm->FillGraph("all_eneRes.z", 0, cryPos, sigma/peak, cryPosE, 0*(sigma/peak)*TMath::Sqrt(sigmaErr*sigmaErr/(sigma*sigma) + peakErr*peakErr/(peak*peak)) );
   }

   newdir2->cd();
   vars->hm->GetGraph(targetName.Data(), histN)->Write();
   newdir3->cd();
   vars->hm->GetGraph(targetName2.Data(), histN)->Write();
   
}

void ANA_CLASS_NAME::ProcessDtSlicesZeta(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
   
      gStyle->SetOptFit(1);
      histObj->SetStats(1111111);
   //preliminary

}

void ANA_CLASS_NAME::ProcessDtSlices2(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
   
      gStyle->SetOptFit(1);
      histObj->SetStats(1111111);
   //preliminary

   //slicingPrep
      TDirectory *curr = gDirectory->GetDirectory(0);
      TDirectory *newdir = curr->mkdir("slices", "", true);
      TDirectory *newdir2 = curr->mkdir("graph", "", true);
      TDirectory *newdir3 = curr->mkdir("profiles", "", true);
      TDirectory *newdir4 = curr->mkdir("th2", "", true);

      newdir4->cd();
      histObj->Write();
      auto *prof = ((TH2F*)histObj)->ProfileX();
      newdir3->cd();
      prof->Write();
      newdir->cd();
      histSkipFlag = 1;

      Ssiz_t ss = 0;
      TString targetName =  "";
      TString me = name(0, name.Length());
      me.Tokenize(targetName, ss, "_");
   //slicingPrep

   double emin = histObj->GetXaxis()->GetXmin();
   double emax = histObj->GetXaxis()->GetXmax();

   int minEntriesPerSlice = 1000;
   double minSlice = 4;
   double sliceIncrement = 0.5;
   double estart = emin, slice = 0, estop = 0;
   int i{0}, breakFlag{0};

   while (1) { 
      
      estop = estart + slice;
      histObj->SetAxisRange(estart, estop);
      TH1F* islice = (TH1F*)((TH2F*)histObj)->ProjectionY();
      
      if (estop >= emax) {breakFlag = 100;}
      if ( (slice < minSlice || islice->Integral() < minEntriesPerSlice) && breakFlag == 0) {slice += sliceIncrement; continue;} 
      if (islice->Integral() < 0.5*minEntriesPerSlice) {break;}

      islice->SetNameTitle(Form(name + "_slice_%d", i), Form(name + "_slice_%d", i));
      TH1F* ixslice = (TH1F*)((TH2F*)histObj)->ProjectionX();
      double enval = ixslice->GetMean();
      double envalErr = ixslice->GetMeanError();

      double peak, sigma, peakErr, sigmaErr;
      int res = fitTimeGausStandard(islice, peak, peakErr, sigma, sigmaErr, "none");
      islice->Write();
      if (!res) {continue;}

      vars->hm->FillGraph(targetName.Data(), histN, enval/1000, sigma/2, envalErr/1000, sigmaErr/2);

      if (breakFlag > 0) {break;}
      estart += slice;
      slice = 0;
      i++;
   }

   TF1 resoFit("resoFit", "sqrt([0]*[0]/(x) + [1]*[1])");
   resoFit.SetParLimits(0, 0, 100);
   resoFit.SetParLimits(1, 0, 100);

   vars->hm->GetGraph(targetName.Data(), histN)->Fit(&resoFit, "EMQ");
   newdir2->cd();
   vars->hm->GetGraph(targetName.Data(), histN)->Write();
}

void ANA_CLASS_NAME::ProcessDtSlices(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
      
      histObj->SetStats(1111111);
      gStyle->SetOptStat(11111111);
      gStyle->SetOptFit(1);
   //preliminary

   //slicingPrep
      TDirectory *curr = gDirectory->GetDirectory(0);
      TDirectory *newdir = curr->mkdir("slices", "", true);
      TDirectory *newdir2 = curr->mkdir("graph", "", true);
      TDirectory *newdir3 = curr->mkdir("profiles", "", true);
      TDirectory *newdir4 = curr->mkdir("th2", "", true);

      newdir4->cd();
      histObj->Write();
      auto *prof = ((TH2F*)histObj)->ProfileX();
      newdir3->cd();
      prof->Write();
      newdir->cd();
      histSkipFlag = 1;

      Ssiz_t ss = 0;
      TString targetName =  "";
      TString me = name(0, name.Length());
      me.Tokenize(targetName, ss, "_");
   //slicingPrep
   
   double emin = 10;
   double emax = 35;
   int sliceNo = 4;
   double slice = (emax-emin)/sliceNo;

   for (int i = 0; i < sliceNo; i++) {
      histObj->SetAxisRange(emin + slice*i, emin + slice*(i+1));
      TH1F* islice = (TH1F*)((TH2F*)histObj)->ProjectionY();
      islice->SetNameTitle(Form(name + "_slice_%d", i), Form(name + "_slice_%d", i));
      // islice->Rebin(4); 
      TH1F* ixslice = (TH1F*)((TH2F*)histObj)->ProjectionX();
      double enval = ixslice->GetMean();
      double envalErr = ixslice->GetMeanError();

      double peak, sigma, peakErr, sigmaErr;
      int res = fitTimeGausStandard(islice, peak, peakErr, sigma, sigmaErr, "none");
      islice->Write();
      if (!res) {continue;}

      vars->hm->FillGraph(targetName.Data(), histN, enval/1000, sigma/2, envalErr/1000, sigmaErr/2);
   }

   TF1 resoFit("resoFit", "sqrt([0]*[0]/(x) + [1]*[1])"); 
   resoFit.SetParLimits(0, 0, 100);
   resoFit.SetParLimits(1, 0, 100);

   vars->hm->GetGraph(targetName.Data(), histN)->Fit(&resoFit, "EMQ");
   newdir2->cd();
   vars->hm->GetGraph(targetName.Data(), histN)->Write();
}

void ANA_CLASS_NAME::ProcessProfileXFitPol1(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
      
      histObj->SetStats(1111111);
      gStyle->SetOptStat(11111111);
      gStyle->SetOptFit(1);
   //preliminary

   // TDirectory *curr = gDirectory->GetDirectory(0);
   // TDirectory *newdir = curr->mkdir("profile", "", true);
   // newdir->cd();

   auto *prof = ((TH2F*)histObj)->ProfileX();
   TF1 ll("ll", "[0]+[1]*x");
   prof->Fit(&ll, "EMQ");
   prof->Write(); 
}

void ANA_CLASS_NAME::ProcessLandau(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
      
      histObj->SetStats(1111111);
      gStyle->SetOptStat(11111111);
      gStyle->SetOptFit(1);
   //preliminary

   double peak, sigma, peakErr, sigmaErr;
   fitEneLandau(histObj, peak, peakErr, sigma, sigmaErr, "none");

   double pos = (double)cry + 0.25 * (2*(double)sid-1);
   if (name.Contains("eneSelMip")) {
      vars->hm->FillGraph("mpvMip", 0, pos, peak, 0, peakErr);
      vars->hm->FillGraph("sigmaMip", 0, pos, sigma, 0, sigmaErr);
      vars->hm->Fill1d("histMpvMip", 0, peak);
      vars->hm->Fill1d("histSigmaMip", 0, sigma);
      vars->RES.disp.mpvMip->FillCrySide(cry, sid, peak);
      vars->CAL.eneEq[cry][sid] = peak;
      vars->CAL.eneEqErr[cry][sid] = peakErr;
   } else if (name.Contains("eneSelPathMip")) { 
      vars->hm->FillGraph("mpvMipPath", 0, pos, peak, 0, peakErr);
      vars->hm->FillGraph("sigmaMipPath", 0, pos, sigma, 0, sigmaErr);
      vars->hm->Fill1d("histMpvMipPath", 0, peak);
      vars->hm->Fill1d("histSigmaMipPath", 0, sigma);
      vars->RES.disp.mpvMipPath->FillCrySide(cry, sid, peak);
      vars->CAL.eneEq[cry][sid] = peak;
      vars->CAL.eneEqErr[cry][sid] = peakErr;
   }

}

void ANA_CLASS_NAME::ProcessTimes(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 10) {histSkipFlag = 1; return;}
      
      histObj->SetStats(1111111);
      gStyle->SetOptStat(1111);
      gStyle->SetOptFit(1);
   //preliminary

   double peak, sigma, peakErr, sigmaErr;
   fitTimeGausStandard(histObj, peak, peakErr, sigma, sigmaErr, "none");
}

void ANA_CLASS_NAME::ProcessStabilityCharge(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

   //preliminary
      AnaVars *vars = (AnaVars*) anaVars;
      TString name = histObj->GetName(); 
      
      int row{0}, col{0}, cry{0}, sid{0}, used{0};
      ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
      
      // if (used == 0) {histSkipFlag = 1; return;}
      if (histObj->GetEntries() < 50) {histSkipFlag = 1; return;}
      
      histObj->SetStats(1111111);
      gStyle->SetOptStat(11111111);
      gStyle->SetOptFit(1);
   //preliminary 

   TDirectory *curr = gDirectory->GetDirectory(0);
   TDirectory *newdir = curr->mkdir("slices", "", true);
   TDirectory *newdir2 = curr->mkdir("graph", "", true);
   TDirectory *newdir3 = curr->mkdir("profiles", "", true);
   TDirectory *newdir4 = curr->mkdir("th2", "", true);

   newdir4->cd();
   histObj->Write();
   auto *prof = ((TH2F*)histObj)->ProfileX();
   newdir3->cd();
   prof->Write();
   newdir->cd();
   histSkipFlag = 1;

   Ssiz_t ss = 0;
   TString targetName =  "";
   TString me = name(0, name.Length());
   me.Tokenize(targetName, ss, "_");
   
   int sliceNo = histObj->Integral() / 800;
   double sliceSize = histObj->GetXaxis()->GetXmax() / (double)sliceNo; 

   for (int i = 0; i < sliceNo; i++) {

      histObj->SetAxisRange( i*sliceSize, (i+1)*sliceSize );
      TH1F* islice = (TH1F*)((TH2F*)histObj)->ProjectionY();
      islice->SetNameTitle(Form(name + "_slice_%d", i), Form(name + "_slice_%d", i));
      //cout<<name<<"   --> slice "<<i<<" from  "<<i*sliceSize<<"  "<<(i+1)*sliceSize<<"   intergral:: "<<islice->Integral()<<endl;

      double peak, sigma, peakErr, sigmaErr;
      int res = fitChargeRawLandau(islice, peak, peakErr, sigma, sigmaErr, "default");
      if (!res) {continue;}

      vars->hm->FillGraph(targetName.Data(), histN, (i+0.5)*sliceSize, peak, sliceSize/3.6, peakErr); 
      islice->Write();
   } 

   newdir2->cd();
   vars->hm->GetGraph(targetName.Data(), histN)->Fit("pol0", "REMQ");
   vars->hm->GetGraph(targetName.Data(), histN)->Write();

}

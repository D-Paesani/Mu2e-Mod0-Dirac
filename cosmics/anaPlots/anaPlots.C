#pragma once
using namespace std; 

#include "anaPlots.h"
#define ANA_CLASS_NAME anaPlots

void ANA_CLASS_NAME::Begin() {

   VAR.RES.nevTrim =  VAR.CONF.etp - VAR.CONF.etp%1000;
   BookHistograms();

   //timeCalRead
      if ( VAR.CONF.timeCalFileName != "" ) {
         cout<<"Retrieving time calibration from "<<VAR.CONF.timeCalFileName<<endl;
         auto *disp = VAR.RES.disp.timOffs;
         ifstream fin;
         fin.open(VAR.CONF.timeCalFileName);
         if(!fin.is_open()) {
            cout<<"ERROR: could not open file"<<endl;
            exit(EXIT_FAILURE); 
         }
         for (int i = 0; i < GEO.chNo; i++) {
            int row, col, side, cry;
            float dataIn, dataInErr;
            if (!(fin >> cry >> side >> dataIn >> dataInErr)) {break;}
            VAR.CAL.timeOff[cry][side] = dataIn;
            VAR.CAL.timeOffErr[cry][side] = dataInErr;
            disp->FillCrySide(cry, side, dataIn);
         }
         fin.close();  
         VAR.CONF.outFile->cd();
         gDirectory->cd("calibration");
         cout<<"...done"<<endl<<endl;
      }
   //timeCalRead

   //EneEqualRaw 
      if (VAR.CONF.set_equalFromRaw) {
         cout<<"Performing energy equalisation on MIP peaks "<<endl<<"[ 0% ";
         auto *disp = VAR.RES.disp.mpvRaw;
         for (int i = 0; i < GEO.chNo; i++) {         

            if ( i%(GEO.chNo/10) == 0 ) {cout<<"-";}                                                  
            TH1F *hist = (TH1F*)HM.GetHist("eneEqPreliminary", i);
            int icry = GEO.cryCha(i), isid = GEO.sideCha(i);            
            
            if ( (VAR.CONF.runName.Contains("run145") || //togliere!!!
                  VAR.CONF.runName.Contains("run114") ||
                  VAR.CONF.runName.Contains("run157") ||
                  VAR.CONF.runName.Contains("run165")  )
            && icry == 41 && isid == 1) {continue;} //togliere!!!
               
            branch2histo1d(hist, VAR.CONF.chain, "Qval", Form("iCry == %d && SiPM == %d && evnum < %lld", icry, isid, VAR.CONF.nEntriesEqual));
            if (hist->GetEntries() < 10) { continue; }
            double peak, sigma, peakErr, sigmaErr;
            fitChargeRawLandau(hist, peak, peakErr, sigma, sigmaErr, "none");

            double nplot = 1 + (double)icry + 0.25 * (2*(double)isid-1);
            HM.FillGraph("mpvRaw", 0, nplot, peak, 0, peakErr);
            disp->FillCrySide(icry, isid, peak);
      
            if ( VAR.CONF.set_equalFromFile ) { continue; }
            VAR.CAL.chargeEq[icry][isid] = peak;
            VAR.CAL.chargeEqErr[icry][isid] = peakErr;
            VAR.RES.isUsed[icry][isid] = 1; 
         }
         cout<<"> 100% ]"<<endl;
         // VAR.CONF.outFile->cd();                                        
         // gDirectory->cd("calibration");
         cout<<"...done"<<endl<<endl;
      } 
   //EneEqualRaw

   //EneEqualLoad
      if ( VAR.CONF.set_equalFromFile ) {
         cout<<"Retrieving energy calibration from "<<VAR.CONF.chargeCalFileName<<endl;
         ifstream fin;
         fin.open(VAR.CONF.chargeCalFileName);
         if(!fin.is_open()) {
            cout<<"ERROR: could not open file"<<endl;
            exit(EXIT_FAILURE); 
         }
         int count = 0;
         for (int i = 0; i < GEO.chNo; i++) {
            int row, col, side, cry;
            float dataIn, dataInErr;
            if (!(fin >> cry >> side >> dataIn >> dataInErr)) {break;}
            VAR.CAL.chargeEq[cry][side] = dataIn;
            VAR.CAL.chargeEqErr[cry][side] = dataInErr; 
            VAR.RES.isUsed[cry][side] = 1; 
            VAR.RES.readoutType[cry] += 1;
            count++;
         }
         fin.close();  
         VAR.RES.usedChanNo = count; 
         cout<<"...done"<<endl<<endl;
      }
   //EneEqualLoad

   //event
      EVE.SetChargeEqual( &(VAR.CAL.chargeEq) );
      EVE.SetTimeOffset( &(VAR.CAL.timeOff) );
      EVE.SetQRef(VAR.CAL.qRef);
      EVE.SetQtoE(VAR.CAL.qToE);
   //event

   //crtParser
      if(VAR.CONF.set_useCRT) {
         cout<<endl<<"Initializing CRT tree from "<<VAR.CONF.crtFileName<<endl;
         CRT.InitTree(0, VAR.CONF.crtFileName);
         CRT.trigOffset = 1;
      }
   //crtParser

}

void ANA_CLASS_NAME::Terminate() {

   //Histograms
      cout<<endl<<"Processing histograms..."<<endl<<endl<<endl;
      HM.ProcessBoxesGroup(0);
      HM.ProcessBoxesGroup(1);
      cout<<"...done"<<endl<<endl;
   //Histograms

   //DispWrite
      VAR.CONF.outFile->cd();
      gDirectory->cd("calibration");
      VAR.RES.disp.WriteAll();
   //DispWrite

   //saveEneCalib
      if ( VAR.CONF.set_equalFileWrite ) {
         cout<<"Writing mip charge cal to "<<VAR.CONF.chargeCalFileName<<endl;
         ofstream fileOut;
         fileOut.open( VAR.CONF.chargeCalFileName ); 
         for (int i = 0; i < GEO.chNo; i++) {
            int cry = GEO.cryCha(i), sid = GEO.sideCha(i);
            if (!VAR.RES.isUsed[cry][sid]) {continue;}
            double val{0}, err{0};
            val = VAR.CAL.chargeEq[cry][sid];
            err = VAR.CAL.chargeEqErr[cry][sid]; 
            val = VAR.CONF.set_equalFileWrite == 1 ? val : val * VAR.CAL.eneEq[cry][sid] / VAR.CAL.MIP_MPV_eRef;
            err = VAR.CONF.set_equalFileWrite == 1 ? err : err * VAR.CAL.eneEqErr[cry][sid] / VAR.CAL.MIP_MPV_eRef;
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
   //saveEneCalib

   //crtParser
      if(VAR.CONF.set_useCRT) {
         CRT.Terminate();
      }
   //crtParser

   //stats
      cout<<endl<<"----->  processed hits: "<<VAR.RES.processedHits<<endl;
      cout<<"----->  processed evts: "<<VAR.RES.processedEvents<<endl<<endl;
   //stats

   //anaFile
      if (VAR.CONF.set_closeOutFile) {
         VAR.CONF.outFile->Close();
         cout<<endl<<endl;
         cout<<endl<<endl<<"----->  output file: "<<endl<<"      root "<<VAR.CONF.outFileName<<endl<<endl;
      }
   //anaFile

}









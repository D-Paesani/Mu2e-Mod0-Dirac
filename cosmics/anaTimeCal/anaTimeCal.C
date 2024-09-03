#pragma once

#include "anaTimeCal.h"
#define ANA_CLASS_NAME anaTimeCal

using namespace std; 

void ANA_CLASS_NAME::Begin() {

   BookHistograms();

   //EneEqualRaw 
      if (VAR.CONF.set_equalFromRaw) {
         cout<<"Performing energy equalisation on MIP peaks "<<endl<<"[ 0% ";

         for (int i = 0; i < GEO.chNo; i++) {        

            if ( i%(GEO.chNo/10) == 0 ) {cout<<"-";}                                                  
            TH1 *hist = (TH1*)HM.GetHist("eneEqPreliminary", i);
            int icry = GEO.cryCha(i), isid = GEO.sideCha(i);     
            
            if ( (VAR.CONF.runName.Contains("run145") || //togliere!!!
                  VAR.CONF.runName.Contains("run114") ||
                  VAR.CONF.runName.Contains("run157") ||
                  VAR.CONF.runName.Contains("run165")  )
            && icry == 41 && isid == 1) { continue;} //togliere!!!
               
            branch2histo1d(hist, VAR.CONF.chain, "Qval", Form("iCry == %d && SiPM == %d && evnum < %lld", icry, isid, VAR.CONF.nEntriesEqual));
            if (hist->GetEntries() < 10) { continue; }
            double peak, sigma, peakErr, sigmaErr;
            fitChargeRawLandau(hist, peak, peakErr, sigma, sigmaErr, "none");
      
            if ( VAR.CONF.set_equalFromFile ) { continue; }
            VAR.CAL.chargeEq[icry][isid] = peak;
            VAR.CAL.chargeEqErr[icry][isid] = peakErr;
            VAR.RES.isUsed[icry][isid] = 1; 
         }
         cout<<"> 100% ]"<<endl;
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

   //saveFinalCalib
      TString fname = VAR.CONF.timeCalOutFileName;
      if ( fname != "" && VAR.CONF.set_saveTimCalib ) {
         cout<<"Writing time offsets to "<<fname<<endl;
         ofstream fileOut;
         fileOut.open(fname); 
         for (int i = 0; i < GEO.chNo; i++) {
            int cry = GEO.cryCha(i), sid = GEO.sideCha(i);
            if (!VAR.RES.isUsed[cry][sid]) {continue;}
            double val{0}, err{0};
            val = VAR.CAL.timeResidOut[cry][sid] + VAR.CAL.timeOff[cry][sid];
            err = VAR.CAL.timeResidErrOut[cry][sid];
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

   //saveEneCalib
      fname = VAR.CONF.chargeCalFileName;
      if ( VAR.CONF.step == 0 ) {
         cout<<"Writing charge cal to "<<fname<<endl;
         ofstream fileOut;
         fileOut.open(fname); 
         for (int i = 0; i < GEO.chNo; i++) {
            int cry = GEO.cryCha(i), sid = GEO.sideCha(i);
            if (!VAR.RES.isUsed[cry][sid]) {continue;}
            double val{0}, err{0};
            val = VAR.CAL.chargeEq[cry][sid];
            val = VAR.CONF.options.Contains("equal(mip)") ? val * VAR.CAL.MIP_MPV_eRef/VAR.CAL.eneEq[cry][sid] : val; //check this line 
            err = VAR.CAL.chargeEqErr[cry][sid];
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

}

void ANA_CLASS_NAME::LoopEntries(Long64_t entry) { 

   int hitMax = read->nHits;
   Long64_t evnum = read->evnum;
   if (hitMax == 0) {return;} 
   EVE.Init(hitMax);
   MIP.Init(hitMax);
   Event::Cell *cell = EVE.cell; 

   for (int ihit = 0; ihit < hitMax; ihit++) { // Event parser

      int cry = read->iCry[ihit], row = read->iRow[ihit], col = read->iCol[ihit], sid = read->SiPM[ihit];

      double tim = VAR.CONF.set_useTime == "PS" ? read->p_psT[ihit] : read->p_teTfi[ihit];

      if (sid < 0 || sid > 1) {continue;}

      //edit!
      if ( (VAR.CONF.runName.Contains("run145") || 
            VAR.CONF.runName.Contains("run114") ||
            VAR.CONF.runName.Contains("run157") ||
            VAR.CONF.runName.Contains("run165")  )
            && cry == 41 && sid == 1) { 
               if (hitMax == 1) {return;}
               continue;} 
      //edit!

      int idx = EVE.AddHit(cry, sid, read->Qval[ihit], tim);
   
      VAR.RES.processedHits++; 
   } 

   VAR.RES.processedEvents++; 
   int hitNo = EVE.numCell; 

   for (int hit = 0; hit < hitNo; hit++) { // Fill selectors

      MIP.AddHit(hit, cell[hit].cry, cell[hit].eneMean, cell[hit].timMean, cell[hit].side);
   } 

   int selNo = MIP.sel.count;
   MIP.GetCosmicType(); 
   TString cosmicType = MIP.sel.cosmicType;   
   if(cosmicType == "NO") {return;}   

   for (int i = 0; i < GEO.rowNo; i++) {  //Fill layer
      HM.Fill1d("eneSelLayer", i, MIP.sel.eneLayer[i]); 
   }   

   for (int hit = 0; hit < hitNo; hit++) { // Fill histos selection

      int cha = cell[hit].chan, cry = cell[hit].cry, side = cell[hit].side;
      double ene =  cell[hit].eneMean, tim = cell[hit].timMean;

      if (side == 2) {   
         HM.Fill1d("timeDiffSel", cry, cell[hit].tim[1] - cell[hit].tim[0]);
      }

      for (int i = 0; i < GEO.sidNo; i++) {

         int thisChan = GEO.chaCry(cry, i);
         if (side < 2 && thisChan != cha) {continue;}

         ene =  cell[hit].ene[i];
         tim = cell[hit].tim[i];
         
         HM.Fill1d("eneSelAll", thisChan, ene); 
         HM.Fill2d("timSelAll:ene", thisChan, ene, tim); 
         HM.Fill1d("timSelAll", thisChan, tim);
         if (cosmicType == "PV") { HM.Fill1d("eneSelVert", thisChan, ene); HM.Fill1d("timSelVert", thisChan, tim); } 
         if (cosmicType == "PD") { HM.Fill1d("eneSelDiag", thisChan, ene); HM.Fill1d("timSelDiag", thisChan, tim); } 
      } 
   } 

   if ( cosmicType != "PV" && cosmicType != "PD" && VAR.CONF.options.Contains("sel(pa)") ) { return; } // Mip selection
   if ( cosmicType != "PV" && VAR.CONF.options.Contains("sel(pv)") ) {return;}
   if ( cosmicType != "PD" && VAR.CONF.options.Contains("sel(pd)") ) {return;}

   for (int hit = 0; hit < hitNo; hit++) { //Time differences after selection
      if (cell[hit].side == 2) {
         HM.Fill1d("eneSelMip", GEO.chaCry(cell[hit].cry, 0), cell[hit].ene[0]);
         HM.Fill1d("eneSelMip", GEO.chaCry(cell[hit].cry, 1), cell[hit].ene[1]);
      } else {
         HM.Fill1d("eneSelMip", cell[hit].chan, cell[hit].eneMean);
      }
   } 

   if (VAR.CONF.step == 0) {return;}

   int res = MIP.FitTrajectoryYX(); 
   if (res < 0) {return;} 

   HM.Fill1d("thetaXY", 0, MIP.sel.thetaYX);
   HM.Fill1d("cosTheta", 0, MIP.sel.cosThetaYX);
   HM.Fill1d("fitchi2", 0, MIP.sel.fitYXchi2);

   MIP.ComputeWeightSum();

   if (VAR.CONF.options.Contains("useSlopeZ(on)")) { } //Slope YZ

   for (int hit = 0; hit < hitNo; hit++) { // Fill histos after trajectory cut
      if (cell[hit].side == 2) {
         for (int i = 0; i < selNo; i++) {

            if ( cell[hit].cry != MIP.cell[i].cry ) {continue;}
            HM.Fill1d("timeDiffCut", cell[hit].cry, cell[hit].tim[1] - cell[hit].tim[0]);
            break;
         }
      }
   } 

   for (int i = 0; i < selNo; i++) { 

      HM.Fill1d("eneCellCut", 0, MIP.cell[i].ene);
      HM.Fill1d("eneCry", MIP.cell[i].cry, MIP.cell[i].ene);
   }

   HM.Fill1d("eneAvgCut", 0, MIP.sel.eneTot / selNo);

   double tEvent = MIP.sel.timeCentroid;
   double costheta = MIP.sel.cosThetaYX;

   for (int iSel = 0; iSel < selNo; iSel++) { // Fill time residuals
      
      int cry = MIP.cell[iSel].cry, row = MIP.cell[iSel].row, col = MIP.cell[iSel].col;
      double ene = MIP.cell[iSel].ene;
      int hit = MIP.cell[iSel].hit;

      for (int sid = 0; sid < GEO.sidNo; sid++) {

         if ( cell[hit].side != 2 && cell[hit].side != sid ) {continue;}

         int cha = GEO.chaCry(cry, sid);

         double resid = cell[hit].tim[sid] + MIP.cell[iSel].y / ( PRM.lightSpeed * costheta ) - tEvent; 

         HM.Fill1d("resid", cha, resid);
         HM.Fill2d("resid:ene", cha, cell[hit].ene[sid], resid);     
         HM.Fill2d("resid:eneCell", 0, ene, resid);     
         HM.Fill2d("resid:row", 0, row, resid);      
         HM.Fill2d("resid:col", 0, col, resid);      
         HM.Fill2d("resid:nCell", 0, selNo, resid);      
      }

   }

}









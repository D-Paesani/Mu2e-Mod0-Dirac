#pragma once
using namespace std; 

#include "anaPlots.h"
#define ANA_CLASS_NAME anaPlots

void ANA_CLASS_NAME::LoopEntries(Long64_t entry) { 

   int hitMax = read->nHits;
   Long64_t evnum = read->evnum;
   if (hitMax == 0) {return;} 
   EVE.Init(hitMax);
   MIP.Init(hitMax);
   Event::Cell *cell = EVE.cell; 
      
   if( VAR.CONF.set_useCRT ) { // CrtParser
      if( CRT.SetEntry(evnum) < 0 ) {return;} // fare 

      HM.Fill1d("crtZTop", 0, CRT.zTop);
      HM.Fill1d("crtZBtm", 0, CRT.zBtm);
      HM.Fill1d("crtXTop", 0, CRT.xTop);
      HM.Fill1d("crtXBtm", 0, CRT.xBtm);
      HM.Fill1d("crtThetaYX", 0, CRT.thetaYX);
      HM.Fill1d("crtThetaYZ", 0, CRT.thetaYZ);
   }  

   for (int ihit = 0; ihit < hitMax; ihit++) { // EventParser

      int cry = read->iCry[ihit], row = read->iRow[ihit], col = read->iCol[ihit], sid = read->SiPM[ihit];

      double tim = VAR.CONF.set_useTime == "PS" ? read->p_psT[ihit] : read->p_teTfi[ihit];
      double charge = read->Qval[ihit];

      if (sid < 0 || sid > 1) {continue;}

      //togliere!!!
      if ( (VAR.CONF.runName.Contains("run145") || 
            VAR.CONF.runName.Contains("run114") ||
            VAR.CONF.runName.Contains("run157") ||
            VAR.CONF.runName.Contains("run165")  )
            && cry == 41 && sid == 1) { 
               if (hitMax == 1) {return;}
               continue;} 
      //togliere!!!

      int idx = EVE.AddHit(cry, sid, charge, tim);

      HM.Fill2d("stabMpvRaw", GEO.chaCry(cry, sid), entry, charge);
      HM.Fill1d("occupancy", GEO.chaCry(cry, sid), entry);
      HM.Fill1d("trigRaw", 0, (double)cry - 0.2*(2*sid-1));
      VAR.RES.disp.hitRaw->FillCrySide(cry, sid);

      VAR.RES.processedHits++; 
   } 

   VAR.RES.processedEvents++; 
   int hitNo = EVE.numCell; 

   for (int hit = 0; hit < hitNo; hit++) { // Fill selectors

      MIP.AddHit(hit, cell[hit].cry, cell[hit].eneMean, cell[hit].timMean, cell[hit].side);
   }

   int selNo = MIP.sel.count;
   HM.Fill1d("ncells", 0, selNo);

   MIP.GetCosmicType(); 
   TString cosmicType = MIP.sel.cosmicType;
   if( VAR.CONF.set_mipFilter != "OFF" && cosmicType == "NO") {return;} //Mip selection

   for (int i = 0; i < GEO.rowNo; i++) {  //Fill layer
      HM.Fill1d("eneSelLayer", i, MIP.sel.eneLayer[i]); 
   }   

   for (int hit = 0; hit < hitNo; hit++) { // Fill histos all mip selection

      int cha = cell[hit].chan, cry = cell[hit].cry, side = cell[hit].side;
      double ene =  cell[hit].eneMean, tim = cell[hit].timMean;

      if (side == 2) {   
         HM.Fill1d("timeDiffSel", cry, cell[hit].tim[1] - cell[hit].tim[0]);
         HM.Fill1d("timeDiffRaw", cell[hit].cry, cell[hit].tim[1] - cell[hit].tim[0]);
         HM.Fill2d("timeDiffRaw.ene", cell[hit].cry, cell[hit].eneMean, cell[hit].tim[1] - cell[hit].tim[0]);
      }

      for (int i = 0; i < GEO.sidNo; i++) {

         int thisChan = GEO.chaCry(cry, i);
         if (side < 2 && thisChan != cha) {continue;}

         ene =  cell[hit].ene[i];
         tim = cell[hit].tim[i];
         
         HM.Fill1d("eneSelAll", thisChan, ene); 
         HM.Fill2d("timSelAll.ene", thisChan, ene, tim); 
         HM.Fill1d("timSelAll", thisChan, tim);
         if (cosmicType == "PV") { HM.Fill1d("eneSelVert", thisChan, ene); HM.Fill1d("timSelVert", thisChan, tim); } 
         if (cosmicType == "PD") { HM.Fill1d("eneSelDiag", thisChan, ene); HM.Fill1d("timSelDiag", thisChan, tim); } 
      } 
   }  

   if ( MIP.FilterCosmic(VAR.CONF.set_mipFilter) == 0 ) { return; }  //Mip selection
   
   for (int hit = 0; hit < hitNo; hit++) { //Fill histos after mip filter selection

      int cha = cell[hit].chan, cry = cell[hit].cry, side = cell[hit].side;

      if (side == 2) {

         HM.Fill1d("timeDiffSel", cry, cell[hit].tim[1] - cell[hit].tim[0]);
         HM.Fill2d("timeDiffSel.ene", cry, cell[hit].eneMean, cell[hit].tim[1] - cell[hit].tim[0]);
      
      } else { }
   
      for (int s = 0; s < GEO.sidNo; s++) {

         int thisCha = GEO.chaCry(cry, s);
         if (side <  2 && thisCha != cha) {continue;}
         
         HM.Fill2d("stabMpvMip", thisCha, entry, cell[hit].ene[s]);
         // HM.Fill1d("eneSelMip", thisCha, cell[hit].ene[s]);
      }

   }

   int res = MIP.FitTrajectoryYX(); //Fit trajectory
   if (res < 0) {return;} 

   MIP.ComputeWeightSum(); //Compute mip 
   MIP.CoumputePaths();
   double tEvent = MIP.sel.timeCentroid;
   double costheta = MIP.sel.cosThetaYX;

   HM.Fill1d("thetaXY", 0, MIP.sel.thetaYX);
   HM.Fill1d("fitchi2", 0, MIP.sel.fitYXchi2);
   HM.Fill1d("ncellsCut", 0, selNo);

   HM.Fill2dIf(VAR.CONF.set_useCRT, "thetaYX.crt", 0, CRT.thetaYX, MIP.sel.thetaYX);  

   //edit!!
      if(VAR.CONF.set_useCRT) {

         double topcalo = MIP.sel.fitYX->Eval(CRT.yTop); //edit
         if (1 || CRT.barBtm == CRT.barTop) {


         HM.Fill2d("crtXtop.Calo", 0, topcalo, CRT.xTop);
         HM.Fill1d("crtXtop.Calo_1", 0, topcalo - CRT.xTop);


            double sigma2 = (MIP.sel.tanThetaErrYX*MIP.sel.tanThetaErrYX + 0.001);
            double w = 1/(sigma2*sigma2);
            HM.Fill2d("tanthetaYX.crt", 0, CRT.tanThetaYX, MIP.sel.tanThetaYX, w);  

            HM.Fill1d("thetaYX_mod0crtDiff", 0, CRT.thetaYX - MIP.sel.thetaYX);
            HM.Fill1d("thetaYX_mod0crtRatio", 0, CRT.thetaYX/MIP.sel.thetaYX);
            HM.Fill1d("thetaYX_mod0crtAsymm", 0, (CRT.thetaYX - MIP.sel.thetaYX)/(CRT.thetaYX + MIP.sel.thetaYX), w);

            HM.Fill2d("thetaYX_mod0crtDiff_vs_thetaYX_mod0", 0, CRT.thetaYX - MIP.sel.thetaYX, MIP.sel.thetaYX);
            HM.Fill2d("thetaYX_mod0crtDiff_vs_thetaYX_crt", 0, CRT.thetaYX - MIP.sel.thetaYX, CRT.thetaYX);
            double tcrt = CRT.crtRead->T[0] + CRT.crtRead->T[1] + CRT.crtRead->T[2] + CRT.crtRead->T[3];
            tcrt *= 0.25;
            HM.Fill1d("timesDiffCrtMod0", 0, tcrt - tEvent); //fare tempo di volo
         }
      }
   //edit!!

   //YX display
      if (gRandom->Uniform(0, 100) < 1) { //displayXY

         TCanvas *cc;

         if(VAR.CONF.set_useCRT) {

            CaloDisplay disp("dispYX", Form("CRTtrack_%lld_t%db%d", entry, CRT.barTop, CRT.barBtm), 
            "E [MeV]", -550, 550, CRT.yBtm - 50, CRT.yTop + 50, "zcol0", false);

            for (int i = 0; i < selNo; i++) {
               disp.Fill(MIP.sel.yxGraph->GetPointY(i), MIP.sel.yxGraph->GetPointX(i), MIP.cell[i].ene);
            }

            cc = disp.GetCanvas();

            TLine trackCrt( CRT.xTop, CRT.yTop, CRT.xBtm, CRT.yBtm );
            TLine top(-500, CRT.yTop, 500, CRT.yTop);
            TLine btm(-500, CRT.yBtm, 500, CRT.yBtm);
            TMarker tt(CRT.xTop, CRT.yTop, 2);
            TMarker bb(CRT.xBtm, CRT.yBtm, 2);

            trackCrt.SetLineColor(kRed); trackCrt.Draw("same");
            top.SetLineColor(kGray); top.SetLineWidth(5); top.Draw("same");
            btm.SetLineColor(kGray); btm.SetLineWidth(5); btm.Draw("same");
            tt.SetMarkerColor(kBlack); tt.SetMarkerSize(3); tt.Draw("same");
            bb.SetMarkerColor(kBlack); bb.SetMarkerSize(3); bb.Draw("same");

            TLine lmod0(MIP.sel.fitYX->Eval(0.8*CRT.yTop), 0.8*CRT.yTop, MIP.sel.fitYX->Eval(0.8*CRT.yBtm), 0.8*CRT.yBtm);
            cc->cd();
            lmod0.SetLineColor(kGreen);
            lmod0.Draw("same");

            VAR.CONF.outFile->cd();
            gDirectory->cd("reconstruction");
            gDirectory->mkdir("displayXY", "", true)->cd();
            cc->Write(Form("c_dispYX_%lld", entry));

         } else {

            CaloDisplay disp("dispYX", Form("event_%lld", entry), 
            "E [MeV]", -550, 550, -250, 250, "zcol0", false);
            cc = disp.GetCanvas();

            for (int i = 0; i < selNo; i++) {
               disp.Fill(MIP.sel.yxGraph->GetPointY(i), MIP.sel.yxGraph->GetPointX(i), MIP.cell[i].ene);
               
               double x = MIP.cell[i].x;
               double y = MIP.cell[i].y;
               double path = MIP.cell[i].path;
               if(path>0) {
                  TPaveText *pt = new TPaveText(x-10, y-10, x+10, y+10, "NB");
                  pt->SetFillColor(0);
                  pt->SetFillStyle(0);
                  pt->SetBorderSize(0);
                  pt->AddText(Form("%2.3f", path));
                  cc->cd();
                  pt->Draw( "same" );
               }
            }
   
            TLine lmod0(MIP.sel.fitYX->Eval(220), 220, MIP.sel.fitYX->Eval(-220), -220);
            cc->cd();
            lmod0.SetLineColor(kBlue);
            lmod0.Draw("same");

            VAR.CONF.outFile->cd();
            gDirectory->cd("reconstruction");
            gDirectory->mkdir("displayXY", "", true)->cd();
            cc->Write(Form("c_dispYX_%lld", entry));
            // cc->SaveAs(Form("disp_%lld.png", entry));

         }
       
      }
   //YX display

   //Slope YZ
      if (VAR.CONF.set_useSlopeYZ) {
         
         int res = MIP.ComputeSlopeYZ();
         if (res > -15) { 
            HM.Fill1d("zetaChi2", 0, MIP.sel.fitYZchi2); 
            HM.Fill2d("zetaChi2:nZeta", 0, MIP.sel.layerCnt, MIP.sel.fitYZchi2); 
         }
         if (res < 0) {return;}

         // if (MIP.sel.yzGraph->GetN() > 6) {      
         if (gRandom->Uniform(0, 100) < 1) {      
            TGraphErrors *gra = MIP.sel.yzGraph;
            gra->SetMinimum(-5);
            gra->SetMaximum(5);
            gra->SetTitle(Form("zySlope %4.4f | ev %lld | npts %d",  MIP.sel.thetaYZ, entry, gra->GetN()));
            gra->SetMarkerStyle(20);
            TCanvas cc(Form("c_zetaGr_%lld", entry));
            cc.cd();
            gra->Draw("APS");

            VAR.CONF.outFile->cd();
            gDirectory->cd("reconstruction/zeta");
            gDirectory->mkdir("zetaFit", "", true)->cd();
            cc.Write(Form("c_zetaGr_%lld", entry));
         }

         HM.Fill1d("firstLayer", 0,  MIP.sel.rowTop);
         HM.Fill1d("cosThetaYZ", 0, MIP.sel.cosThetaYZ);
         HM.Fill1d("thetaYZ", 0,  MIP.sel.thetaYZ);
         HM.Fill2d("thetaYZ.nLayer", 0,  MIP.sel.thetaYZ, MIP.sel.layerCnt);
      
         HM.Fill2d("slope.slopeErr", 0, MIP.sel.fitYZpar, MIP.sel.fitYZparErr);
         HM.Fill1d("slopeFitYZ", 0, MIP.sel.fitYZpar);
         HM.Fill2dIf(VAR.CONF.set_useCRT, "thetaYZ.crt", 0, CRT.thetaYZ, MIP.sel.thetaYZ);

      }
   //Slope YZ

   for (int hit = 0; hit < hitNo; hit++) { // Fill histos after trajectory cut and other cuts loop hit

      int side = cell[hit].side;

      if (side == 2) {

         for (int i = 0; i < selNo; i++) {

            if ( cell[hit].cry != MIP.cell[i].cry ) {continue;}
            HM.Fill1d("timeDiffCut", cell[hit].cry, cell[hit].tim[1] - cell[hit].tim[0]);
            HM.Fill2d("timeDiffCut.ene", cell[hit].cry, cell[hit].eneMean, cell[hit].tim[1] - cell[hit].tim[0]);
            break; 
         }
      } 

      for (int s = 0; s < GEO.sidNo; s++) {

         if (side != 2 && s != side) {continue;}

         HM.Fill1d("trigMip", 0, (double)cell[hit].cry - 0.2*(2*s-1) );
         VAR.RES.disp.hitMip->FillCrySide(cell[hit].cry, s);
      }

   } 

   for (int i = 0; i < selNo; i++) { // Fill histos after trajectory cut and other cuts loop sel

      int cry = MIP.cell[i].cry;

      HM.Fill1d("eneCellCut", 0, MIP.cell[i].ene);
      HM.Fill1d("eneCry", cry, MIP.cell[i].ene);

      double path = MIP.cell[i].path;
      if(path > 0) {
         HM.Fill2d("ene.pathAll", 0, path, cell[MIP.cell[i].hit].eneMean);
         HM.Fill1d("pathLenghts", cry, path); 
         HM.Fill1d("pathLenghtsAll", 0, path); 
         HM.Fill2d("pathLenghts.layer", 0, MIP.cell[i].row, path); 
         if (path > 34 && path < 36) { //////////////
            double corr = GEO.cryL / path;
            HM.Fill1d("eneSelPathMip", GEO.chaCry(cry, 0), corr * cell[MIP.cell[i].hit].ene[0]);
            HM.Fill1d("eneSelPathMip", GEO.chaCry(cry, 1), corr * cell[MIP.cell[i].hit].ene[1]);
            HM.Fill1d("eneSelMip", GEO.chaCry(cry, 0), cell[MIP.cell[i].hit].ene[0]);
            HM.Fill1d("eneSelMip", GEO.chaCry(cry, 1), cell[MIP.cell[i].hit].ene[1]);
         } 
      }

      if (VAR.CONF.set_useCRT == 0) {continue;}

      double zval = CRT.GetZCryY(MIP.cell[i].y);

      HM.Fill2d("hitsYZ", 0, zval, MIP.cell[i].row);
      HM.Fill2dIf(CRT.barBtm == CRT.barTop, "hitsYZvert", 0, zval, MIP.cell[i].row);
      HM.Fill2d("tim.hitYZ", 0, zval, MIP.cell[i].tim);
      HM.Fill2d("ene.z", cry, zval, MIP.cell[i].ene); 

   }

   HM.Fill1d("eneAvgCut", 0, MIP.sel.eneTot / selNo);

   //AllPairs
      for (int i = 0; i < EVE.numCell; i++) {
         for (int ii = i + 1; ii < EVE.numCell; ii++) {
      
            // if (cell[i].eneMean < MIP.cellEneMin) {continue;}
            // if (cell[ii].eneMean < MIP.cellEneMin) {continue;}            
            
            if ( !MIP.CheckEnergy(cell[i].eneMean) ) {continue;}
            if ( !MIP.CheckEnergy(cell[ii].eneMean) ) {continue;}
            
            double sign = - 2.0 * (double)(cell[i].row > cell[ii].row) + 1; //check
            double tdiffRaw = cell[i].timRaw[0] - cell[ii].timRaw[0];
            double tdiff = cell[i].tim[0] - cell[ii].tim[0];
            // double tdiff = cell[i].timMean - cell[ii].timMean;
            double dist = cell[ii].y - cell[i].y;
            double tdiffTof3 = tdiff - MIP.GetTofCorrection3d(dist);
            double tdiffTof2 = tdiff - MIP.GetTofCorrection2d(dist);
         
            HM.Fill1d("allPairs", 0, sign*tdiff);
            HM.Fill1d("allPairsRaw", 0, sign*tdiffRaw);
            HM.Fill1d("allPairsTof", 0, sign*tdiffTof2);
            HM.Fill1d("allPairsTof3", 0, sign*tdiffTof3);

            // cout<<cell[i].row<<"  "<<cell[ii].row<<"   "<<MIP.GetTofCorrection2d(dist)<<"    "<<MIP.GetTofCorrection3d(dist)<<endl;

            //prova layer
               int layt = max(cell[i].row, cell[ii].row);
               int layb = min(cell[i].row, cell[ii].row);
               int k;
               if (layt != layb) {
                  for(k = 0; k < 21; k++) {
                     if ( VAR.RES.layerDiffList[k] == Form("%d-%d", layt, layb) ) {break;}
                  } 
                  HM.Fill1d("timeDiffLayer", k, sign*tdiff);
                  HM.Fill1d("timeDiffLayerToF2d", k, sign*tdiffTof2);
                  HM.Fill1d("timeDiffLayerToF3d", k, sign*tdiffTof3);
               }
            //prova layer
            
            //prova DualReadout
               tdiff = cell[i].timMean - cell[ii].timMean;
               tdiffTof3 = tdiff - MIP.GetTofCorrection3d(dist);
               tdiffTof2 = tdiff - MIP.GetTofCorrection2d(dist);
               if ( cell[i].cry == 9 && cell[ii].cry == 25) { 
                  HM.Fill1d("timDiffCell", 0, tdiff);
                  HM.Fill1d("timDiffCellTof", 0, tdiffTof2);
                  HM.Fill1d("timDiffCellTof3", 0, tdiffTof3);
               } else if ( cell[i].cry == 9 && cell[ii].cry == 41 ) {
                  HM.Fill1d("timDiffCell", 1, tdiff);
                  HM.Fill1d("timDiffCellTof", 1, tdiffTof2);
                  HM.Fill1d("timDiffCellTof3", 1, tdiffTof3);
               } else if ( cell[i].cry == 25 && cell[ii].cry == 41 ) {
                  HM.Fill1d("timDiffCell", 2, tdiff);
                  HM.Fill1d("timDiffCellTof", 2, tdiffTof2);
                  HM.Fill1d("timDiffCellTof3", 2, tdiffTof3);
               }
            //prova DualReadout

         } 
      } 
   //AllPairs





   









}









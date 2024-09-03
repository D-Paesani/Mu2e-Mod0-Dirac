#pragma once
using namespace std; 

#include "anaPlots.h"
#define ANA_CLASS_NAME anaPlots

void ANA_CLASS_NAME::BookHistograms() {

  //LayerTimeDiffLabel
    int k = 0;
    for (int i = 0; i < GEO.rowNo; i++) {
        for (int ii = GEO.rowNo - 1; ii > i; ii--) {
          VAR.RES.layerDiffList[k] = Form("%d-%d", ii, i); 
          k++;
        } 
    } 
  //LayerTimeDiffLabel

  cout<<"Creating HistBoxes:"<<endl;  

  HM.SetOutFile(VAR.CONF.outFile);
  HM.SetAnaVariables(&VAR);
  HM.SetUserDefNamer(&(ANA_CLASS_NAME::NamerDefault)); 
  HM.SetUserDefProcess(&(ANA_CLASS_NAME::SkipUnused)); 
  HM.SetUserDefGraphProcess(&(ANA_CLASS_NAME::SkipUnusedGraph)); 
  HM.SetGroup(0);
  TDirectory *tmpDir;
    
  //energy
    HM.SetOutDir( VAR.CONF.outFile->mkdir("energy") );
    HM.SetUseFolders(1);
    int qBins = 200;
    double qFrom = 0, qTo = 100;

    HM.AddHistBox("th1f", GEO.chNo, "eneEqPreliminary", "", "Q", "pC", 400, 0, 20000, "num(chan) tag(rcs)");
    HM.AddHistBox("th1f", GEO.chNo, "eneSelMip", "en", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessLandau);
    HM.AddHistBox("th1f", GEO.chNo, "eneSelPathMip", "en", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessLandau);
    HM.AddHistBox("th1f", GEO.chNo, "eneSelVert", "en vert", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)");
    HM.AddHistBox("th1f", GEO.chNo, "eneSelDiag", "en diag", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)");
    HM.AddHistBox("th1f", GEO.chNo, "eneSelAll", "en all", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)");
    HM.AddHistBox("th1f", GEO.cryNo, "eneCry", "eneCry", "E", "MeV", 200, 0, 100, "num(cry) tag(rc)");
    HM.AddHistBox("th1f", GEO.rowNo, "eneSelLayer", "eneLayer", "E", "MeV", qBins, qFrom, qTo, "num(arr) tag(arr)");
    HM.AddHistBox("th2f", 1, "ene.pathAll", "ene.pathAll", "dX", "cm", "E", "MeV", 500, 0, 50, 100, 0, 50, "num(off) tag(off)", &ANA_CLASS_NAME::ProcessProfileXFitPol1); 
  //energy

  //timing
    HM.SetOutDir( VAR.CONF.outFile->mkdir("timing") );
    HM.SetUseFolders(1);
    int tBins = 500;
    double tFrom = -100, tTo = 400;

    HM.AddHistBox("th1f", 3, "timDiffCellTof", "timeDiffCellTof2D", "T", "ns", 400, -10, 10, "num(arr) tag(arr)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", 3, "timDiffCellTof3", "timeDiffCellTof3D", "T", "ns", 400, -10, 10, "num(arr) tag(arr)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", 3, "timDiffCell", "timeDiffCell", "T", "ns", 400, -10, 10, "num(arr) tag(arr)", &ANA_CLASS_NAME::ProcessTimes);
    
    HM.AddHistBox("th1f", 21, "timeDiffLayer", "timeDiffLayer", "T", "ns", 500, -3, 3, "", &ANA_CLASS_NAME::ProcessTimes, &ANA_CLASS_NAME::NamerLayerDiff);
    HM.AddHistBox("th1f", 21, "timeDiffLayerToF2d", "diffLayerToF2d", "T", "ns", 500, -3, 3, "", &ANA_CLASS_NAME::ProcessTimes, &ANA_CLASS_NAME::NamerLayerDiff);
    HM.AddHistBox("th1f", 21, "timeDiffLayerToF3d", "diffLayerToF3d", "T", "ns", 500, -3, 3, "", &ANA_CLASS_NAME::ProcessTimes, &ANA_CLASS_NAME::NamerLayerDiff);

    HM.AddHistBox("th1f", GEO.chNo, "timSelVert", "t0 vert", "T", "ns", tBins, tFrom, tTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", GEO.chNo, "timSelDiag", "t0 diag", "T", "ns", tBins, tFrom, tTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", GEO.chNo, "timSelAll", "t0 all", "T", "ns", tBins, tFrom, tTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th2f", GEO.chNo, "timSelAll.ene", "time vs charge", "E", "MeV", "T", "ns", qBins, qFrom, qTo, tBins, tFrom, tTo, "num(chan) tag(rcs)");

    HM.AddHistBox("th1f", GEO.cryNo, "timeDiffRaw", "timeDiffs", "td", "ns", 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", GEO.cryNo, "timeDiffCut", "timeDiffs", "td", "ns", 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", GEO.cryNo, "timeDiffSel", "timeDiffs", "td", "ns", 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessTimes);

    HM.AddHistBox("th2f", GEO.cryNo, "timeDiffRaw.ene", "tdene raw", "E", "MeV", "T", "ns", 100, 0, 100, 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessDtSlices2);
    HM.AddHistBox("th2f", GEO.cryNo, "timeDiffSel.ene", "tdene sel", "E", "MeV", "T", "ns", 100, 0, 100, 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessDtSlices2);
    HM.AddHistBox("th2f", GEO.cryNo, "timeDiffCut.ene", "tdene cut", "E", "MeV", "T", "ns", 100, 0, 100, 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessDtSlices);
    HM.SetUseFolders(0);
    HM.AddGraphBox("tge", GEO.cryNo, "timeDiffRaw.ene", "reso raw", "E", "MeV", "T", "ns", "num(cry) tag(rc) writeSkip(on)");
    HM.AddGraphBox("tge", GEO.cryNo, "timeDiffSel.ene", "reso sel", "E", "MeV", "T", "ns", "num(cry) tag(rc) writeSkip(on)");
    HM.AddGraphBox("tge", GEO.cryNo, "timeDiffCut.ene", "reso cut", "E", "MeV", "T", "ns", "num(cry) tag(rc) writeSkip(on)");
    HM.SetUseFolders(1);

    HM.SetOutDir( HM.GetOutDir()->mkdir("allPairs") );
    HM.SetUseFolders(0);
    HM.AddHistBox("th1f", 1, "allPairsRaw", "allPairsRaw", "T", "ns", 500, -100, 100, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "allPairs", "allPairs", "T", "ns", 500, -3, 3, "num(arr) tag(off)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", 1, "allPairsTof", "allPairsToF", "T", "ns", 500, -3, 3, "num(arr) tag(off)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", 1, "allPairsTof3", "allPairsToF3", "T", "ns", 500, -3, 3, "num(arr) tag(off)", &ANA_CLASS_NAME::ProcessTimes);
    HM.SetUseFolders(1);
  //timing

  //calibration
    HM.SetOutDir( VAR.CONF.outFile->mkdir("calibration") );
    HM.SetUseFolders(0);

    HM.AddHistBox("th1f", 1, "trigRaw", "trigRaw", "channel [iCry+0.25*(2*iSide-1)]", "", "hits", "", 4*(51+2), -1+0.25, 0.25+52, 1, 1, 1, "num(off) tag(off)");
    HM.AddHistBox("th1f", 1, "trigMip", "trigMip", "channel [iCry+0.25*(2*iSide-1)]", "", "hits", "", 4*(51+2), -1+0.25, 0.25+52, 1, 1, 1, "num(off) tag(off)");

    HM.AddGraphBox("tge", 1, "mpvMip", "mpvMip", "chanID", "", "E", "MeV", "num(off) tag(off)");
    HM.AddGraphBox("tge", 1, "mpvMipPath", "mpvMipPath", "chanID", "", "E", "MeV", "num(off) tag(off)");
    HM.AddGraphBox("tge", 1, "sigmaMip", "sigmaMip", "chanID", "", "E", "MeV", "num(off) tag(off)");
    HM.AddGraphBox("tge", 1, "sigmaMipPath", "sigmaMipPath", "chanID", "", "E", "MeV", "num(off) tag(off)");
    HM.AddGraphBox("tge", 1, "mpvRaw", "mpvRaw", "chanID", "", "Q", "AU", "num(off) tag(off)");

    HM.SetGroup(1);
    HM.AddHistBox("th1f", 1, "histMpvMip", "", "E", "MeV", 200, 15, 25, "num(off) tag(off)");
    HM.AddHistBox("th1f", 1, "histMpvMipPath", "", "E", "MeV", 200, 15, 25, "num(off) tag(off)");   
    HM.AddHistBox("th1f", 1, "histSigmaMip", "", "E", "MeV", 200, 0, 10, "num(off) tag(off)");
    HM.AddHistBox("th1f", 1, "histSigmaMipPath", "", "E", "MeV", 200, 0, 10, "num(off) tag(off)");
    HM.SetGroup(0);
  //calibration

  //reconstruction
    tmpDir = VAR.CONF.outFile->mkdir("reconstruction");
    HM.SetOutDir(tmpDir);
    HM.SetUseFolders(0);

    HM.AddHistBox("th1f", 1, "ncells", "ncells", "ncells", "", 40, 0, 40, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "ncellsCut", "ncellsCut", "ncellsCut", "", 40, 0, 40, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "thetaXY", "ang", "", "", 200, -PRM.pi/2, PRM.pi/2, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "thetaXYsel", "ang", "", "", 200, -PRM.pi/2, PRM.pi/2, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "eneCellCut", "eneCell", "E", "MeV", 200, 0, 100, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "eneAvgCut", "EneMean", "E", "MeV", 200, 0, 100, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "fitchi2", "trajFitchi2", "chi2", "", 250, 0, 50, "num(arr) tag(off)");

    HM.SetOutDir(tmpDir->mkdir("paths"));
    HM.SetUseFolders(1);
    HM.AddHistBox("th1f", GEO.cryNo, "pathLenghts", "pathLenghts", "dx", "cm", 10000, 0, 50, "num(cry) tag(rc)"); 
    HM.SetUseFolders(0);
    HM.AddHistBox("th1f", 1, "pathLenghtsAll", "pathLenghtsAll", "dx", "cm", 10000, 0, 50, "num(off) tag(off)"); 
    HM.AddHistBox("th2f", 1, "pathLenghts.layer", "pathLenghtsAll", "layer", "", "dx", "cm", GEO.rowNo, 0, GEO.rowNo, 10000, 0, 50, "num(off) tag(off)"); 

    if (VAR.CONF.set_useSlopeYZ) { 
      HM.SetUseFolders(0);

      HM.SetOutDir(tmpDir->mkdir("zeta"));

      HM.AddHistBox("th2f", 1, "thetaYZ.nLayer", "", "ang", "", "nlay", "",  200, -PRM.pi/2, PRM.pi/2, GEO.rowNo, 0, GEO.rowNo, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "thetaYZ", "", "", "", 200, -PRM.pi/2, PRM.pi/2, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "cosThetaYZ", "", "", "", 400, 0.6, 1, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "zetaChi2", "", "", "", 150, 0, 30, "num(arr) tag(off)");
      HM.AddHistBox("th2f", 1, "zetaChi2:nZeta", "zetaChi2:nZeta", "nPointZetaFit", "", "chi2", "", GEO.colNo, 0, GEO.colNo + 1, 150, 0, 30, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "firstLayer", "", "row", "", GEO.rowNo, 0, GEO.rowNo, "num(arr) tag(off)");
      HM.AddHistBox("th2f", 1, "slope.slopeErr", "", "slope", "", "sloperr", "", 500, -1, 1, 500, 0, 1, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "slopeFitYZ", "", "", "", 500, -1, 1, "num(arr) tag(off)");

      HM.AddHistBox("th2f", GEO.cryNo, "timeDiffCut.zeta", "timeDiffCut.zeta", "E", "MeV", "Z", "", 8, 0, 8, 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessDtSlicesZeta);     
      HM.SetUseFolders(0);
      HM.AddGraphBox("tge", GEO.cryNo, "timeDiffCut.zeta", "timeDiffCut.zeta", "E", "MeV", "T", "ns", "num(cry) tag(rc) writeSkip(on)");
      HM.SetUseFolders(1);
    }

    if (VAR.CONF.set_useCRT){
      
      HM.SetOutDir(tmpDir->mkdir("crt"));
      HM.SetUseFolders(0);

      HM.AddHistBox("th1f", 1, "crtXTop", "x", "", "", 500, -1000, 1000, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "crtXBtm", "x", "", "", 500, -1000, 1000, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "crtZTop", "z", "", "", 10, -25*5, 25*5, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "crtZBtm", "z", "", "", 10, -25*5, 25*5, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "crtThetaYZ", "crtThetaYZ", "", "",  15, -0.2, 0.2, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "crtThetaYX", "crtThetaYX", "", "", 200, -PRM.pi/2, PRM.pi/2, "num(off) tag(off)");
      HM.AddHistBox("th2f", 1, "thetaYX.crt", "thetaYX.crt", "theta_CRT", "", "theta_Mod0", "", 200, -PRM.pi/2, PRM.pi/2, 200, -PRM.pi/2, PRM.pi/2, "num(off) tag(off)", &ANA_CLASS_NAME::ProcessProfileXFitPol1);
      HM.AddHistBox("th2f", 1, "thetaYZ.crt", "thetaYZ.crt", "theta_CRT", "", "theta_Mod0", "", 15,  -0.2, 0.2, 200, -PRM.pi/2, PRM.pi/2, "num(off) tag(off)", &ANA_CLASS_NAME::ProcessProfileXFitPol1);
      
      HM.AddHistBox("th2f", 1, "tanthetaYX.crt", "tanthetaYX.crt", "tantheta_CRT", "", "tantheta_Mod0", "", 1000, -10, 10, 1000, -10, 10, "num(off) tag(off)");

      HM.AddHistBox("th2f", 1, "hitsYZ", "", "Z - Zmid", "mm", "layer", "", 80, -GEO.cryD, GEO.cryD, GEO.rowNo, 0, GEO.rowNo, "num(off) tag(off)");
      HM.AddHistBox("th2f", 1, "hitsYZvert", "", "Z - Zmid", "mm", "layer", "", 80, -GEO.cryD, GEO.cryD, GEO.rowNo, 0, GEO.rowNo, "num(off) tag(off)");
      HM.AddHistBox("th2f", 1, "tim.hitYZ", "", "Z - Zmid", "mm", "tim", "ns", 80, -GEO.cryD, GEO.cryD, 500, -50, 50, "num(off) tag(off)");

      HM.AddHistBox("th1f", 1, "thetaYX_mod0crtDiff", "", "", "", 100, -1, 1, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "thetaYX_mod0crtRatio", "", "", "", 1000, -10, 10, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "thetaYX_mod0crtAsymm", "", "", "", 1000, -10, 10, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "thetaYX_mod0crtChi2", "", "", "", 10000, 0, 100, "num(off) tag(off)");

      HM.AddHistBox("th2f", 1, "thetaYX_mod0crtDiff_vs_thetaYX_mod0", "", "", "", "", "", 100, -1, 1, 100, -1, 1, "num(off) tag(off)");
      HM.AddHistBox("th2f", 1, "thetaYX_mod0crtDiff_vs_thetaYX_crt", "", "", "", "", "", 100, -1, 1, 100, -1, 1, "num(off) tag(off)");

      HM.AddHistBox("th2f", 1, "thetaYX_mod0crtDiff_vs_thetaYX_mod0", "", "", "", "", "", 100, -1, 1, 100, -1, 1, "num(off) tag(off)");
      HM.AddHistBox("th2f", 1, "thetaYX_mod0crtDiff_vs_thetaYX_crt", "", "", "", "", "", 100, -1, 1, 100, -1, 1, "num(off) tag(off)");

      HM.AddHistBox("th1f", 1, "timesDiffCrtMod0", "", "", "", 500, -50, 50, "num(off) tag(off)");

      HM.AddHistBox("th2f", 1, "xcrtyop_calo_2", "", "", "", "", "", 1000, -500, 500, 1000, -500, 500, "num(off) tag(off)");
      HM.AddHistBox("th1f", 1, "xcrtyop_calo", "", "", "", 1000, -500, 500, "num(off) tag(off)");
  //edit!!


    }
  //reconstruction

  //uniformity
    HM.SetOutDir( VAR.CONF.outFile->mkdir("uniformity") );

    HM.SetUseFolders(0);
    HM.AddGraphBox("tge", 1, "all_ene.z", "", "cryID + pos", "", "E", "MeV", "num(off) tag(off)", nullptr, nullptr, 2, kBlack, 1, 1, 1, 0);
    HM.AddGraphBox("tge", 1, "all_eneRes.z", "", "cryID + pos", "", "s/p", "", "num(off) tag(off)", nullptr, nullptr, 2, kBlack, 1, 1, 1, 0);

    HM.SetUseFolders(1);
    HM.AddHistBox("th2f", GEO.cryNo, "ene.z", "", "Z", "mm", "E", "MeV", 16, -GEO.cryD, GEO.cryD, 100, 0, 100, "num(cry) tag(rc)", ANA_CLASS_NAME::ProcessSliceChargeZ);
    HM.SetUseFolders(0);
    HM.AddGraphBox("tge", GEO.cryNo, "ene.z", "", "Z", "mm", "E", "cm", "num(cry) tag(rc) writeSkip(on)");
    HM.AddGraphBox("tge", GEO.cryNo, "resene.z", "", "Z", "mm", "s/p", "", "num(cry) tag(rc) writeSkip(on)");
  //uniformity

  //stability
    HM.SetOutDir( VAR.CONF.outFile->mkdir("stability") );
    HM.SetUseFolders(1);
    Long64_t nevtt = VAR.RES.nevTrim;

    HM.AddHistBox("th1f", GEO.chNo, "occupancy", "occupancy", "hits", "trig", nevtt/1000, 0, nevtt, "num(chan) tag(rcs)");
    HM.AddHistBox("th2f", GEO.chNo, "stabMpvRaw", "", "trig", "", "Q", "pC",  200, 0, nevtt, 200, 0, 20000, "num(chan) tag(rcs)", &ProcessStabilityCharge);
    HM.AddHistBox("th2f", GEO.chNo, "stabMpvMip", "", "trig", "", "E", "MeV",  200, 0, nevtt, qBins, qFrom, qTo, "num(chan) tag(rcs)", &ProcessStabilityCharge);
    HM.SetUseFolders(0); 
    HM.AddGraphBox("tge", GEO.chNo, "stabMpvRaw", "", "trig", "", "charge", "AU", "num(chan) tag(rcs) writeSkip(on)");
    HM.AddGraphBox("tge", GEO.chNo, "stabMpvMip", "", "trig", "", "charge", "AU", "num(chan) tag(rcs) writeSkip(on)");
  //stability

  cout<<endl<<endl;
}


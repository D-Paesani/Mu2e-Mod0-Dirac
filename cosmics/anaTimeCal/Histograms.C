#pragma once
using namespace std; 

#include "anaTimeCal.h"
#define ANA_CLASS_NAME anaTimeCal

void ANA_CLASS_NAME::BookHistograms() {

  cout<<"Creating HistBoxes:"<<endl;  

  HM.SetOutFile(VAR.CONF.outFile);
  HM.SetAnaVariables(&VAR);
  HM.SetUserDefNamer(&(ANA_CLASS_NAME::NamerDefault)); 
  HM.SetUserDefProcess(&(ANA_CLASS_NAME::SkipUnused)); 
  HM.SetUserDefGraphProcess(&(ANA_CLASS_NAME::SkipUnusedGraph)); 
  HM.SetGroup(0);
  TDirectory *stepDir, *tmpDir;

  int qBins = 80, tBins = 500;
  double qFrom = 0, qTo = 100, tFrom = 0, tTo = 500;

  HM.SetOutDir(VAR.CONF.outFile->mkdir(Form("step_%d", VAR.CONF.step)));
  stepDir = HM.GetOutDir();

  HM.SetOutDir( stepDir->mkdir("energy") );
  HM.SetUseFolders(1);
  HM.AddHistBox("th1f", GEO.chNo, "eneEqPreliminary", "", "", "AU", 160, 0, 16000, "num(chan) tag(rcs)");
  HM.AddHistBox("th1f", GEO.chNo, "eneSelMip", "", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessLandau);
  HM.AddHistBox("th1f", GEO.chNo, "timSelVert", "", "T", "ns", tBins, tFrom, tTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes); //RES.usedChanNo;
  HM.AddHistBox("th1f", GEO.chNo, "timSelDiag", "", "T", "ns", tBins, tFrom, tTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes);
  HM.AddHistBox("th1f", GEO.chNo, "timSelAll", "", "T", "ns", tBins, tFrom, tTo, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes);
  HM.AddHistBox("th1f", GEO.chNo, "eneSelVert", "", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)");
  HM.AddHistBox("th1f", GEO.chNo, "eneSelDiag", "", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)");
  HM.AddHistBox("th1f", GEO.chNo, "eneSelAll", "", "E", "MeV", qBins, qFrom, qTo, "num(chan) tag(rcs)");
  HM.AddHistBox("th1f", GEO.rowNo, "eneSelLayer", "", "E", "MeV", qBins, qFrom, qTo, "num(arr) tag(arr)");
  HM.AddHistBox("th1f", GEO.cryNo, "timeDiffSel", "", "", "", 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessTimes);
  HM.AddHistBox("th2f", GEO.chNo, "timSelAll:ene", "", "E", "MeV", "T", "ns", qBins, qFrom, qTo, tBins, tFrom, tTo, "num(chan) tag(rcs)");

  if (VAR.CONF.step > 0) { 

    HM.SetOutDir( stepDir->mkdir("analysis") );

    HM.SetUseFolders(1);
    HM.AddHistBox("th1f", GEO.cryNo, "timeDiffCut", "timeDiffs", "T", "ns", 400, -10, 10, "num(cry) tag(rc)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th1f", GEO.chNo, "resid", "residuals", "T", "ns", 500, -40, 40, "num(chan) tag(rcs)", &ANA_CLASS_NAME::ProcessTimes);
    HM.AddHistBox("th2f", GEO.chNo, "resid:ene", "residuals", "E", "MeV", "T", "ns", qBins, qFrom, qTo, 250, -10, 10, "num(chan) tag(rcs)");
    HM.AddHistBox("th1f", GEO.cryNo, "eneCry", "eneCry", "E", "MeV", 200, 0, 100, "num(cry) tag(rc)");

    HM.SetUseFolders(0);
    HM.AddHistBox("th1f", 1, "thetaXY", "ang", "", "", 200, -PRM.pi/2, PRM.pi/2, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "cosTheta", "cosTheta", "cosine", "", 400, 0.6, 1, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "cosThetaFit", "cosThetaFit", "cosine", "", 400, 0.6, 1, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "cosThetaDiff", "cosThetaDiff", "cosineLS-cosineFit", "", 1000, -0.1, 0.1, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "eneCellCut", "eneCell", "E", "MeV", 200, 0, 100, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "eneAvgCut", "EneMean", "E", "MeV", 200, 0, 100, "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "fitchi2", "trajFitchi2", "chi2", "", 250, 0, 50, "num(arr) tag(off)");
    HM.AddHistBox("th2f", 1, "resid:eneCell", "residuals", "E", "MeV", "T", "ns", qBins, qFrom, qTo, 250, -10, 10, "num(arr) tag(off)");
    HM.AddHistBox("th2f", 1, "resid:nCell", "residuals", "nCell", "", "T", "ns", 40, 0, 40, 250, -10, 10, "num(arr) tag(off)");
    HM.AddHistBox("th2f", 1, "resid:col", "residuals", "col", "", "T", "ns", GEO.colNo, 0, GEO.colNo, 250, -10, 10, "num(arr) tag(off)");
    HM.AddHistBox("th2f", 1, "resid:row", "residuals", "row", "", "T", "ns", GEO.rowNo, 0, GEO.colNo, 250, -10, 10, "num(arr) tag(off)");

    if (VAR.CONF.set_useSlopeYZ) { 
      HM.SetOutDir(HM.GetOutDir()->mkdir("slopeZY"));

      HM.SetUseFolders(0);
      HM.AddHistBox("th1f", 1, "cosThetaYZ", "cosThetaYZ", "", "", 400, 0.6, 1, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "cosThetaYZsign", "distribution", "", "sign(tanThetaYZ)*(cosThetaYZ - 1)", 400, -1, 1, "num(arr) tag(off)");
      HM.AddHistBox("th1f", 1, "zetaChi2", "trajFitchi2", "", "", 200, 0, 100, "num(arr) tag(off)");
    }

  }

  if(VAR.CONF.step > 0) {

    HM.SetOutDir(stepDir->mkdir("results"));
    HM.SetUseFolders(0);
    HM.SetGroup(1);

    HM.AddGraphBox("tge", 1, "residMean", "", "1 + iCry + 0.25*(2*iSide - 1)", "", "time", "ns", "num(arr) tag(off)");
    HM.AddGraphBox("tge", 1, "residRms", "", "1 + iCry + 0.25*(2*iSide - 1)", "", "time", "ns", "num(arr) tag(off)");
    HM.AddGraphBox("tge", 1, "residEntries", "", "1 + iCry + 0.25*( 2*iSide - 1)", "", "# of processed hits", "", "num(arr) tag(off)");
    HM.AddHistBox("th1f", 1, "residHisto", "", "time", "ns", 2000, -1, 1, "num(arr) tag(off)");
  }
  
  cout<<endl<<endl;
}


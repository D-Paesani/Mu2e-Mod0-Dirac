
#include "pars.h"

#include "NumberingHelper.h"
#include "branch2histo.h"
#include "HistManager.h"


struct AnaVars {

    double CF = PRM.CF_def;
    double teFitStart = PRM.teFitStart_def;
    double teFitStop = PRM.teFitStop_def;

    TString anaOptions;

    TString runName, inFileName, outFileName, splineFileName;
    TFile *inFile, *outFile, *splineFile;
    TString mode;
    Long64_t etp{0};
    TTree *chain;

    struct OutDirs {
        TDirectory *spline, *splineGraph, *specimens, *templDraw, *templResampDraw, *profile, *efficiency, *preliminary, *optimisation;
    } outDirs;


    struct AnaRes {

        Long64_t teGoodFits[PRM.chNo]{0};
        Long64_t processedWfs[PRM.chNo]{0};
        Long64_t isAlive[PRM.chNo]{0}; 
        double reso[PRM.chNo]{0}, resoErr[PRM.chNo]{0};
        double landauPks[PRM.cryNo][PRM.sdNo]{0}; 
        
        double optimReso{0}, optimResoErr{0};

    } res;
    
    SplineTypeDef *splines[PRM.chNo];
    TString splinesNameFormat;

    HistManager *HM;

} AN;


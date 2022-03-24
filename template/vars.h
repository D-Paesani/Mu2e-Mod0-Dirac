//#include "HistManager.h"
#include "pars.h"




struct AnaVars {

    TString runName, inFileName, outFileName, splineFileName;
    TFile *inFile, *outFile, *splineFile;
    TString mode;
    Long64_t etp{0};
    TTree *chain;


    struct OutDirs {
        TDirectory *spline, *splineGraph, *specimens, *templDraw, *templResampDraw, *profile, *efficiency;
    } outDirs;


    struct AnaRes {
        Long64_t teGoodFits[PRM.chNo]{0};
        Long64_t processedWfs[PRM.chNo]{0};
        double reso[PRM.chNo]{0};
    } res;

    
    SplineTypeDef *splines[PRM.chNo];
    TString doMakeNewFile;

    
};


//#include "HistManager.h"
#include "pars.h"


struct OutDirs {
    TDirectory *spline, *splineGraph, *specimens, *templDraw, *templResampDraw, *profile;
};


struct AnaRes {
    Long64_t teGoodFits{0};   
    Long64_t processedWfs{0};
    double reso[PRM.chNo]{0};
};


struct AnaVars {

    TString runName, inFileName, outFileName, splineFileName;
    TFile *inFile, *outFile, *splineFile;
    TString mode;
    Long64_t etp{0};
    TTree *chain;
    
    SplineTypeDef *splines[PRM.chNo];
    OutDirs outDirs;
    AnaRes res;
    TString doMakeNewFile;

    
};


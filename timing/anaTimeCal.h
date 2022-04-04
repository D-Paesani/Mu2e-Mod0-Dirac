#pragma once
using namespace std;

#define ANA_CLASS_NAME anaTimeCal

#include "anaClass/anaClass.h"
#define TREE_CLASS_NAME ana

#include "includes/AnaPars.h"
#include "includes/Geom.h"
#include "includes/HistManager.h"
#include "includes/CosmicsTiming.h"
#include "includes/RootIncludes.h"
#include "includes/Event.h"
#include "includes/Branch2Histo.h"

class ANA_CLASS_NAME: public TREE_CLASS_NAME
{

public:

  static const Geom GEO;
  static const AnaPars PRM;
  HistManager HM;
  CosmicsTiming MIP;

  struct _config {

    TString runName, inFileName, outFileName, chargeCalFileName, inTreeName;
    TString timeCalFileName, timeCalOutFileName, timeResidFileName, timeResidOutFileName;
    TFile *inFile, *outFile;
    TTree *chain = nullptr;
    TString mode, options;
    Long64_t etp{0}, evLimit{0}, chainEntries{0};
    int step;

    struct _outDirs {
        TDirectory *calibration;
    } outDirs;
  };

  struct _calib {

    double chargeEq[GEO.cryNo][GEO.sidNo]{0}, chargeEqErr[GEO.cryNo][GEO.sidNo]{0}; 
    double timeOff[GEO.cryNo][GEO.sidNo]{0}, timeOffErr[GEO.cryNo][GEO.sidNo]{0};
    double timeOffOut[GEO.cryNo][GEO.sidNo]{0}, timeOffErrOut[GEO.cryNo][GEO.sidNo]{0};
    double timeResidOut[GEO.cryNo][GEO.sidNo]{0}, timeResidErrOut[GEO.cryNo][GEO.sidNo]{0};
    double timeResidRmsOut[GEO.cryNo][GEO.sidNo]{0}, timeResidRmsErrOut[GEO.cryNo][GEO.sidNo]{0};
  };

  struct _results {

    Long64_t processedHits{0};
    int isAlive[GEO.cryNo][GEO.sidNo]{0}; 
    int isUsed[GEO.cryNo][GEO.sidNo]{0}; 
    int readoutType[GEO.cryNo]{0};
    int usedChanNo{0};

    TGraphErrors *residMean, *residRms, *residEntries;
  };

  struct _anavars {

    _results *res;
    _calib *cal;
    _config *conf;
    const Geom *geo = new Geom;
  };

  _config *CONF = new _config;
  _calib CAL;
  _results RES;
  _anavars VAR;
  typedef struct _anavars AnaVars;

  static HistManager::ProcessFunction SkipUnused;
  static HistManager::ProcessFunction ProcessTimes;
  static HistManager::NamerFunction NamerDefault;
  static int ParseHistVars(int, TString, void*, int&, int&, int&, int&, int&);

  void Begin();
  void Terminate();
  void Loop() override; 
  void LoopEntries(Long64_t);  
  TTree* Configurator(_config*);

  ANA_CLASS_NAME(_config* cfg): TREE_CLASS_NAME(Configurator(cfg)) {
   
    CONF = cfg;
    VAR.cal = &CAL;
    VAR.res = &RES;
    VAR.conf = CONF;

    CONF->outFile = new TFile(CONF->outFileName, "RECREATE");
    CONF->outFile->cd();
    CONF->outDirs.calibration = CONF->outFile->mkdir("calibration");

    CONF->chainEntries = CONF->chain->GetEntriesFast();
    CONF->etp = min((Long64_t)CONF->evLimit, (Long64_t)CONF->chainEntries); 

  };
  
  void Launch() {
    Begin(); 
    Loop();
    Terminate();
  }

};

void ANA_CLASS_NAME::Loop() {  
   
  if (fChain == 0) {return;}
  Long64_t nbytes = 0, nb = 0; 

  for (Long64_t jentry=0; jentry<CONF->etp; jentry++) { 
    Long64_t ientry = LoadTree(jentry); 
    if (ientry < 0) {break;}
    nb = fChain->GetEntry(jentry); 
    nbytes += nb;
    if (!(jentry%5000)) { cout << Form( "    processing evt %lld / %lld  ( %.0f%% )", jentry, CONF->etp, (float)(100*jentry/CONF->etp) ) << endl; }
    LoopEntries(jentry);
  }
}

TTree* ANA_CLASS_NAME::Configurator(_config* conf) {

  if (conf->chain == nullptr) {
    conf->inFile = new TFile(conf->inFileName); 
    conf->inFile->GetObject(conf->inTreeName, conf->chain);  
  }
 
  return conf->chain;
}

#undef ANA_CLASS_NAME
#undef TREE_CLASS_NAME
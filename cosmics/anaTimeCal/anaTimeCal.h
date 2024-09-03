#pragma once
using namespace std;

#define ANA_CLASS_NAME anaTimeCal

#include "../includes/anaClass/anaClass.h"
#define TREE_CLASS_NAME ana

#include "../includes/AnaPars.h"
#include "../includes/Geom.h"
#include "../includes/HistManager.h"
#include "../includes/CosmicsTiming.h"
#include "../includes/RootIncludes.h"
#include "../includes/Event.h"
#include "../includes/Branch2Histo.h"
#include "../includes/TGraphHelper.h"
#include "../includes/Mod0FitHelper.h"
#include "../includes/CrtParser.h"

class ANA_CLASS_NAME
{

public:

  TREE_CLASS_NAME *read; 
  static const Geom GEO;
  static const AnaPars PRM;
  HistManager HM;
  CosmicsTiming MIP;
  Event EVE;
  CrtParser CRT;

  struct _anavars {

    HistManager *hm;

    struct _config {

      TString runName, inFileName, outFileName, chargeCalFileName, inTreeName, crtFileName;
      TString timeCalFileName, timeCalOutFileName, timeResidFileName, timeResidOutFileName;
      TFile *inFile{nullptr}, *outFile{nullptr};
      TTree *chain = nullptr;
      TString mode, options;
      Long64_t etp{0}, evLimit{0}, chainEntries{0};
      int step;

      Long64_t nEntriesEqual = 2e5;

      TString set_mipFilter = "PV";
      TString set_useTime = "FIT";
      int set_closeOutFile = 1;
      int set_useSlopeYZ = 0;
      int set_equalFromFile = 0;
      int set_equalFromRaw = 1;
      int set_useCRT = 0;
      int set_saveTimCalib = 0;
      int set_saveEneCalib = 0;

    } CONF;

    struct _calib {

      Geom::TypeCryMatrixDouble chargeEq{0}, chargeEqErr{0}; 
      Geom::TypeCryMatrixDouble eneEq{0}, eneEqErr{0}; 
      Geom::TypeCryMatrixDouble timeOff{0}, timeOffErr{0};
      Geom::TypeCryMatrixDouble timeOffOut{0}, timeOffErrOut{0};
      Geom::TypeCryMatrixDouble timeResidOut{0}, timeResidErrOut{0};
      Geom::TypeCryMatrixDouble timeResidRmsOut{0}, timeResidRmsErrOut{0};

      double qRef{0}, qToE{0};
      double MIP_MPV_eRef = PRM.MIP_MPV_eRef;

    } CAL;

    struct _results {

      Long64_t processedHits{0}, processedEvents{0};
      Geom::TypeCryMatrixInt isAlive{0}; 
      Geom::TypeCryMatrixInt isUsed{0};
      Geom::TypeCryArrayDouble readoutType{0};
      int usedChanNo{0};
      
    } RES;

  } VAR;

  typedef struct _anavars AnaVars;
  typedef struct _anavars::_config AnaConf;
  typedef struct _anavars::_calib AnaCalib;

  static HistManager::ProcessFunction SkipUnused;
  static HistManager::ProcessGraphFunction SkipUnusedGraph;
  static HistManager::NamerFunction NamerDefault;
  static HistManager::ProcessFunction ProcessLandau;
  static HistManager::ProcessFunction ProcessTimes;

  static int ParseHistVars(int, TString, void*, int&, int&, int&, int&, int&);

  ANA_CLASS_NAME() {};
  void Init();
  void Begin();
  virtual void BookHistograms();
  void Terminate();
  void Loop();
  void LoopEntries(Long64_t);  
  void Launch() {
    Begin(); 
    Loop();
    Terminate();
  }

};

void ANA_CLASS_NAME::Init() {  

  VAR.hm = &HM;

  if (VAR.CONF.chain == nullptr) {
    VAR.CONF.inFile = new TFile(VAR.CONF.inFileName); 
    VAR.CONF.inFile->GetObject(VAR.CONF.inTreeName, VAR.CONF.chain);  
  }

  read = new TREE_CLASS_NAME(VAR.CONF.chain);

  VAR.CONF.chainEntries = VAR.CONF.chain->GetEntriesFast();
  VAR.CONF.etp = VAR.CONF.evLimit == - 1 ? (Long64_t)VAR.CONF.chainEntries : min((Long64_t)VAR.CONF.evLimit, (Long64_t)VAR.CONF.chainEntries); 

  if (VAR.CONF.outFile == nullptr) {
    VAR.CONF.outFile = new TFile(VAR.CONF.outFileName, "RECREATE");
  }

}

void ANA_CLASS_NAME::Loop() {  
   
  if (read->fChain == 0) {return;}
  Long64_t nbytes = 0, nb = 0; 

  for (Long64_t jentry=0; jentry<VAR.CONF.etp; jentry++) { 
    Long64_t ientry = read->LoadTree(jentry); 
    if (ientry < 0) {break;}
    nb = read->fChain->GetEntry(jentry); 
    nbytes += nb;
    if (!(jentry%10000)) { cout << Form( "    processing evt %lld / %lld  ( %.0f%% )", jentry, VAR.CONF.etp, (float)(100*jentry/VAR.CONF.etp) ) << endl; }
    LoopEntries(jentry);
  }
}

#undef ANA_CLASS_NAME
#undef TREE_CLASS_NAME
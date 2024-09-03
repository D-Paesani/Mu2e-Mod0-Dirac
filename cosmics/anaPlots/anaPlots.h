#pragma once
using namespace std;

#define ANA_CLASS_NAME anaPlots
#define TREE_CLASS_NAME ana

#include "../includes/anaClass/anaClass.h"
#include "../includes/Geom.h"
#include "../includes/HistManager.h"
#include "../includes/CosmicsTiming.h"
#include "../includes/RootIncludes.h"
#include "../includes/Event.h"
#include "../includes/Branch2Histo.h"
#include "../includes/TGraphHelper.h"
#include "../includes/CrtParser.h"
#include "../includes/Mod0FitHelper.h"
#include "../includes/AnaPars.h"
#include "../includes/CaloDisplay.h"

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

  //anaVars
    struct _anavars {

      HistManager *hm;

      struct _config {

        TString runName, inFileName, outFileName, chargeCalFileName, inTreeName;
        TString timeCalFileName, timeCalOutFileName, timeResidFileName, timeResidOutFileName, crtFileName;

        TFile *inFile{nullptr}, *outFile{nullptr}, *crtFile{nullptr};
        TTree *chain = nullptr;

        TString mode, options;
        Long64_t etp{0}, evLimit{0}, chainEntries{0};

        Long64_t nEntriesEqual = 2e5;

        TString set_mipFilter = "PV";
        TString set_useTime = "FIT";
        int set_closeOutFile = 1;
        int set_useSlopeYZ = 0;
        int set_equalFromFile = 0;
        int set_equalFromRaw = 1;
        int set_equalFileWrite = 0;
        int set_useCRT = 0;

      } CONF;

      struct _calib {

        Geom::TypeCryMatrixDouble chargeEq{0}, chargeEqErr{0}; 
        Geom::TypeCryMatrixDouble eneEq{0}, eneEqErr{0}; 
        Geom::TypeCryMatrixDouble timeOff{0}, timeOffErr{0};

        double qRef{0}, qToE{0};
        double MIP_MPV_eRef = PRM.MIP_MPV_eRef;
        
      } CAL;

      struct _results {

        Long64_t processedHits{0}, processedEvents{0};
        Geom::TypeCryMatrixInt isAlive{0}; 
        Geom::TypeCryMatrixInt isUsed{0};
        Geom::TypeCryArrayDouble readoutType{0};
        int usedChanNo{0};

        TString layerDiffList[21];

        Long64_t nevTrim{0};

        struct _disp{
          
          CaloDisplay *useless = new CaloDisplay("useless", "", "Q [pC]", -200, 200, -150, 150, "zcol0", false, true);
          CaloDisplay *mpvRaw = new CaloDisplay("disp_mpvRaw", "", "Q [pC]", -200, 200, -150, 150, "zcol0", false, true);
          CaloDisplay *mpvMip = new CaloDisplay("disp_mpvMip", "", "E [MeV]", -200, 200, -150, 150, "zcol0", false, true);
          CaloDisplay *mpvMipPath = new CaloDisplay("disp_mpvMipPath", "", "E [MeV]", -200, 200, -150, 150, "zcol0", false, true);
          CaloDisplay *hitRaw = new CaloDisplay("disp_hitRaw", "", "Q [pC]", -200, 200, -150, 150, "zcol0", false, true);
          CaloDisplay *hitMip = new CaloDisplay("disp_hitMip", "", "Q [pC]", -200, 200, -150, 150, "zcol0", false, true);
          CaloDisplay *timOffs = new CaloDisplay("disp_timOffs", "", "T [ns]", -200, 200, -150, 150, "text90zcol0", false, true);

          void WriteAll() {
            mpvRaw->GetCanvas()->Write();
            mpvMip->GetCanvas()->Write();
            mpvMipPath->GetCanvas()->Write();
            hitRaw->GetCanvas()->Write();
            hitMip->GetCanvas()->Write();
            timOffs->GetCanvas()->Write();
          }

        } disp;
        
      } RES;

    } VAR;

    typedef struct _anavars AnaVars;
    typedef struct _anavars::_config AnaConf;
    typedef struct _anavars::_calib AnaCalib;
  //anaVars

  //HMProcessors
    static HistManager::ProcessFunction SkipUnused;
    static HistManager::ProcessFunction ProcessLandau;
    static HistManager::ProcessFunction ProcessTimes;
    static HistManager::ProcessFunction ProcessDtSlices;
    static HistManager::ProcessFunction ProcessDtSlices2;
    static HistManager::ProcessFunction ProcessDtSlicesZeta;
    static HistManager::ProcessFunction ProcessProfileXFitPol1;
    static HistManager::ProcessFunction ProcessSliceChargeZ;
    static HistManager::ProcessFunction ProcessStabilityCharge;
    static HistManager::ProcessGraphFunction SkipUnusedGraph;
    static HistManager::NamerFunction NamerDefault;
    static HistManager::NamerFunction NamerLayerDiff;
    static int ParseHistVars(int, TString, void*, int&, int&, int&, int&, int&);
  //HMProcessors

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
   
  if ( read->fChain == 0) {return;}
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
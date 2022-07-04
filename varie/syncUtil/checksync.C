#include "../includes/RootIncludes.h"

#define dataFile "../data/data/run_%d_%d.data"
#define procFile "../data/processed/run_%d_%d_syncDani.root"

#define runNo 165
#define fragNo 1 + 960


void checksync() {

    Long64_t global = 0;
    Long64_t lastMax = 0;

    for (int i = 0; i < fragNo; i++)
    {

        TString datafn = Form( dataFile, runNo, i);
        TString procfn = Form( procFile, runNo, i);

        TString cmd = Form( " cat %s | grep -a 'N' | wc -l ", datafn.Data());
        TString countGrep = gSystem->GetFromPipe(cmd);
        int cGrep = countGrep.Atoi();
        global += cGrep;

        TTree *tt;
        TFile ff(procfn);
        ff.GetObject("tree", tt);  
        int entries = tt->GetEntriesFast();
        int tmin = tt->GetMinimum("iTrigGlobal");    
        int tmax = tt->GetMaximum("iTrigGlobal");   
        int diff = tmax - tmin; 
        diff =  diff + (i>0); 

        cout<<
        "   file: "<<runNo<<"_"<<i<<
        "   grepped: "<<cGrep<<
        "   diff: "<<diff<<
        "   evts: "<<entries<<
        "   from: "<<tmin<<
        "   to: "<<tmax<<
        "   totGrep: "<<global<<
        "   gdisc: "<<global - tmax<<
        "   fdisc: "<<cGrep - diff<<
        (cGrep == diff ? "" : "  GREPMISMATCH!!")<<
        ( tmin - lastMax == 1 ? "" : "  DOESNOTCONTINUE!!")<<
        ( i > 0 ? "" : "(ma è normale)")<<
        endl;

        lastMax = tmax;

    }

    /*
        se GREPMISMATCH --> prima o ultima entry skippata
        se DOESNOTCONTINUE --> è stata skippata la prima entry wrt file precedente
         GREPMISMATCH e !DOESNOTCONTINUE --> no

    */










}


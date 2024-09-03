#define HM_VERSION 3.4 
//+ filder option, trofile, TGraphErrors, FillIf, deft, fill2dw
//aggiugnere: nullptr per def funcitons FATTO
//aggiungere: processGroup for dependencies FATTO
//redesign: processor + boxConfig ptr
//now obsolete -> moving to RPM.C

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <vector>
#include <iostream>
#include <TApplication.h>
#include <TH1.h>
#include <stdlib.h>
#include <TFrame.h>
#include <unordered_map>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TProfile.h>
#include <TObjString.h>
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TF1.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <vector>
#include <TString.h>
#include <TTimer.h>
#include <TLatex.h>
#include <TGClient.h>
#include <TRootCanvas.h>
#include <TDatime.h>
#include <time.h>
#include <TH2Poly.h>

using namespace std;

class HistManager 
{

  private: 

    typedef void (*_ProcessFunction)(TH1*, int, int&, void*, TString);
    typedef void (*_ProcessGraphFunction)(TGraphErrors*, int, int&, void*, TString);
    typedef void (*_NamerFunction)(int, TString&, TString&, void*, TString);

    static void processFunc_def(TH1* hObj, int hN, int& hSkip, void* anaVars = nullptr, TString hOpt = "") {};
    static void processGraphFunc_def(TGraphErrors* hObj, int hN, int& hSkip, void* anaVars = nullptr, TString hOpt = "") {};

    static void namerFunc_def(int n, TString& hTag, TString& hTitleTag, void* anaVars = nullptr, TString hOpt = "") { 
      hTag = Form("_%d", n); 
      hTitleTag = Form(" [%d]", n); 
    };

    _ProcessFunction proc_def = &processFunc_def;
    _ProcessFunction procUser_def = &processFunc_def; 
    _ProcessGraphFunction procGraphUser_def = &processGraphFunc_def; 
    _ProcessGraphFunction procGraph_def = &processGraphFunc_def;
    _NamerFunction namerUser_def = &namerFunc_def;
    _NamerFunction namer_def = &namerFunc_def;

  public: 

    typedef void (ProcessFunction) (TH1* hObj, int hN, int& hSkip, void* anaVars, TString hOpt);
    typedef void (ProcessGraphFunction) (TGraphErrors* hObj, int hN, int& hSkip, void* anaVars, TString hOpt);
    typedef void (NamerFunction) (int n, TString& hTag, TString& hTitleTag, void* anaVars, TString hOpt);
  
  private:

    class HistBox
    {        
      private:

        TString _label_form = "%s [%s]";
        TString _entries_form = "Entries / %.2f %s";
        TString _entriesNoUnit_form = "Entries";

        TDirectory  *_histosDir, *_outdir;
        TH1 **_histosObj;
        TString _histType, _label1, _label2, _histosTitle, _histosName, hTag, hTitleTag;
        int _histN, hSkip, _useFolder, _useIndividualFolder;
        void *_anaVars;
        TString _options;
        int _group = 0;

        _ProcessFunction _processFunction; 
        _NamerFunction _namerFunction;

      public:
      
        HistBox() {}
        ~HistBox() {}

        HistBox(
          TDirectory *outdir, 
          int useFolder, int useIndividualFolder, int grp,
          TString histType, int histN,
          TString histosName, TString histosTitle, TString label1, TString label2, TString label3, TString label4,
          int xBins, double xMin, double xMax, 
          int yBins, double yMin, double yMax,
          _ProcessFunction processFunc,
          _NamerFunction namerFunc,
          void * anavars = nullptr,
          TString options = ""
        ):  
          _namerFunction(namerFunc),
          _processFunction(processFunc),
          _histosTitle(histosTitle),
          _histType(histType),
          _histN(histN),
          _histosName(histosName),
          _outdir(outdir),
          _useFolder(useFolder),
          _useIndividualFolder(useIndividualFolder),
          _group(grp),
          _anaVars(anavars),
          _options(options)
        { 

          cout<<"-----> HistManager: HistBox constructor: ["<<_histType<<"] ["<<_histN<<"] ["<<_histosName<<"] ["<<_histosTitle<<"]"<<endl;

          _histosObj = new TH1*[_histN];

          if (_histosTitle == "") { _histosTitle = _histosName; }

          if (_histType == "th1f") { 

            if (label2 == "") {
              _label2 = _entriesNoUnit_form;
              _label1 = label1;
            } else {
              _label2 = Form(_entries_form, ((xMax-xMin)/(double)xBins), label2.Data());
              _label1 = Form(_label_form, label1.Data(), label2.Data());
            }

            for(int k=0; k<_histN; k++){
              _namerFunction(k, hTag, hTitleTag, _anaVars, _options);
              _histosObj[k] = new TH1F(_histosName.Data() + hTag, _histosTitle.Data() + hTitleTag, xBins, xMin, xMax);
            }

          } else if (histType == "th2f") { 

            if (label2 == "") { _label1 = label1; } 
            else { _label1 = Form(_label_form, label1.Data(), label2.Data()); }
            
            if (label4 == "") { _label2 = label3; } 
            else { _label2 = Form(_label_form, label3.Data(), label4.Data()); }

            for(int k=0; k<_histN; k++){
              _namerFunction(k, hTag, hTitleTag, _anaVars, _options);  
              _histosObj[k] = new TH2F(_histosName.Data() + hTag, _histosTitle.Data() + hTitleTag, xBins, xMin, xMax, yBins, yMin, yMax); 
            }      
          
          } else if (histType == "tprofile") { 

            if (label2 == "") {
              _label2 = _entriesNoUnit_form;
              _label1 = label1;
            } else {
              _label2 = Form(_entries_form, ((xMax-xMin)/(double)xBins), label2.Data());
              _label1 = Form(_label_form, label1.Data(), label2.Data());
            }

            _label2 = label3;

            for(int k=0; k<_histN; k++){
              _namerFunction(k, hTag, hTitleTag, _anaVars, _options);  
              _histosObj[k] = new TProfile(_histosName.Data() + hTag, hTitleTag + _histosTitle.Data(), xBins, xMin, xMax);
            }

          } else {
            cout<<"-----> HistManager: HistBox error: histType not supported"<<endl; 
            exit(EXIT_FAILURE); 
          }
        }

        TH1** GetHistosObj(){ return _histosObj; }
        TH1* GetHistObj(int k){ return _histosObj[k]; }
        int GetHistosN() { return _histN; }
        TString GetOptions() { return _options; }
        TDirectory* GetDirectory(){ return _histosDir; }
        int GetGroup(){ return _group; }

        void ProcessBox(){
          
          _outdir->cd(); 

          TDirectory *dir, *individualdir;

          if (_useFolder == 1) {
            _histosDir = _outdir->mkdir(_histosName.Data(), "recreate"); 
            _histosDir->cd();
            dir = _histosDir;
          } else {
            dir = _outdir;
          }

          for (int k = 0; k < _histN; k++) {

            _histosObj[k]->GetXaxis()->SetTitle(_label1);
            _histosObj[k]->GetYaxis()->SetTitle(_label2);

            if (_useIndividualFolder) { 
              TString thisname = _histosObj[k]->GetName();
              individualdir = dir->mkdir(thisname.Data()); 
              individualdir->cd();
            } else {
              dir->cd();           
            }

            hSkip = 0; 
            _processFunction(_histosObj[k], k, hSkip, _anaVars, _options);
            if (hSkip) {continue;}  

            if (_useIndividualFolder) { 
              individualdir->cd();
            } else {
              dir->cd();           
            }         

            _histosObj[k]->Write();
          } 
        }

    };

    class GraphBox
    {

      private: 

        TString _label_form = "%s [%s]";

        TDirectory  *_graphsDir, *_outdir;      
        TMultiGraph *_multiObj = nullptr;
        TGraphErrors **_graphsObj = nullptr;
        TString _graphType, _label1, _label2, _graphsTitle, _graphsName, hTag, hTitleTag;
        int _graphN, hSkip, _useFolder, _useMulti, _useIndividualFolder;
        void *_anaVars;
        TString _options;
        int _group;

        _ProcessGraphFunction _processGraphFunction; 
        _NamerFunction _namerFunction;

      public:

        GraphBox() {}
        ~GraphBox() {}

        GraphBox(
          TDirectory *outdir, 
          int useFolder, int useIndividualFolder, int grp,
          TString graphType, int graphN,
          TString graphsName, TString graphsTitle, TString label1, TString label2, TString label3, TString label4,
          _ProcessGraphFunction processFunc,
          _NamerFunction namerFunc,
          void * anavars = nullptr,
          TString options = "", 
          int useMulti = 0,
          int markStyle = 1, int markColor = kBlack, int markSize = 1, int lineStyle = 1, int lineColor = kBlack, int lineSize = 1
        ):  
          _namerFunction(namerFunc),
          _processGraphFunction(processFunc),
          _graphsTitle(graphsTitle),
          _graphType(graphType),
          _graphN(graphN),
          _graphsName(graphsName),
          _outdir(outdir),
          _useFolder(useFolder),
          _useIndividualFolder(useIndividualFolder),
          _group(grp),
          _useMulti(useMulti),
          _anaVars(anavars),
          _options(options)
        { 

          cout<<"-----> graphManager: GraphBox constructor: ["<<_graphType<<"] ["<<_graphN<<"] ["<<_graphsName<<"] ["<<_graphsTitle<<"]"<<endl;

          if (_useMulti) {_multiObj = new TMultiGraph();}
          if (_graphsTitle == "") {_graphsTitle = _graphsName;}

          if (_graphType == "tge") { 

          _graphsObj = new TGraphErrors*[_graphN];

          if (label2 == "") { _label1 = label1; } 
          else { _label1 = Form(_label_form, label1.Data(), label2.Data()); }
          if (label4 == "") { _label2 = label3; } 
          else { _label2 = Form(_label_form, label3.Data(), label4.Data()); }

          for(int k=0; k<_graphN; k++){
            _namerFunction(k, hTag, hTitleTag, _anaVars, _options);  
            _graphsObj[k] = new TGraphErrors();
            
            _graphsObj[k]->SetName(_graphsName.Data() + hTag);
            _graphsObj[k]->SetTitle(_graphsTitle.Data() + hTitleTag);
            _graphsObj[k]->SetMarkerStyle(markStyle);
            _graphsObj[k]->SetMarkerColor(markColor);
            _graphsObj[k]->SetLineStyle(lineStyle);
            _graphsObj[k]->SetLineColor(lineColor);
            _graphsObj[k]->SetLineWidth(lineSize);
            _graphsObj[k]->GetXaxis()->SetTitle(_label1);
            _graphsObj[k]->GetYaxis()->SetTitle(_label2);

            if (_useMulti) {_multiObj->Add(_graphsObj[k]);}
          }    

          } else {
            cout<<"-----> graphManager: GraphBox error: graphType not supported"<<endl; 
            exit(EXIT_FAILURE); 
          }
        }

        TGraphErrors** GetGraphsObj(){ return _graphsObj; }
        TGraphErrors* GetGraphObj(int k){ return _graphsObj[k]; }
        TMultiGraph* GetMultiGraphObj() {return _multiObj;}
        int GetGraphsN() { return _graphN; }
        TString GetOptions() { return _options; }
        TDirectory* GetDirectory(){ return _graphsDir; }
        int GetGroup() {return _group;}

        void ProcessBox(){
          
          _outdir->cd(); 

          TDirectory *dir, *individualdir;

          if (_useFolder == 1) {
            _graphsDir = _outdir->mkdir(_graphsName.Data(), "recreate"); 
            _graphsDir->cd();
            dir = _graphsDir;
          } else {
            dir = _outdir;
          }

          for (int k = 0; k < _graphN; k++) {

            if (_useIndividualFolder) { 
              TString thisname = _graphsObj[k]->GetName();
              individualdir = dir->mkdir(thisname.Data()); 
              individualdir->cd();
            } else {
              dir->cd();           
            }

            hSkip = 0; 
            _processGraphFunction(_graphsObj[k], k, hSkip, _anaVars, _options);
            if (hSkip) {continue;}  

            if (_useIndividualFolder) { 
              individualdir->cd();
            } else {
              dir->cd();           
            }         
            _graphsObj[k]->Write();
          } 
        }

    };

  private:

    void *anaVars = nullptr;

    TFile *outFile = nullptr;
    TDirectory *outDir = nullptr;
    
    int useFolders{1}, useIndividualFolders{0}, useGraphMulti{0}, boxGroup{0};

  public:

    HistManager() {}
    ~HistManager() {}

    unordered_map<string, HistBox *> HistBoxes;
    unordered_map<string, GraphBox *> GraphBoxes;
    
    void SetUserDefNamer(_NamerFunction namerFunc) { namerUser_def = namerFunc; }
    _NamerFunction GetUserDefNamer() { return namerUser_def; }
    _NamerFunction GetDefNamer() { return namer_def; }

    void SetUserDefProcess(_ProcessFunction processFunc) { procUser_def = processFunc; }
    _ProcessFunction GetUserDefProcess() { return procUser_def;}
    _ProcessFunction GetDefProcess() { return proc_def;}

    void SetUserDefGraphProcess(_ProcessGraphFunction processFunc) { procGraphUser_def = processFunc; }
    _ProcessGraphFunction GetUserDefGraphProcess() { return procGraphUser_def;}
    _ProcessGraphFunction GetDefGraphProcess() { return procGraph_def;}

    void SetGraphUseMulti(int s) { useGraphMulti = s; }

    void SetUseIndividualFolders(int i) { useIndividualFolders = i; }
    void SetUseFolders(int i) { useFolders = i; }
    void SetOutDir(TDirectory *d) { outDir = d; }
    void SetOutFile(TFile *f) { outFile = f; outDir = f; }

    void SetGroup(int g) { boxGroup = g; }
    int GetGroup() { return boxGroup; }

    TFile *GetOutFile() { return outFile; }
    TDirectory *GetOutDir() { return outDir; }
    void CloseOutFile() { outFile->Close(); }

    void SetAnaVariables(void * v) { anaVars = v; }

    int RemoveBox(TString name) { return 1; }
    int RemoveGraphBox(TString name) { return 1; }

    void AddHistBox(TString, int, TString, TString, TString, TString, TString, TString, int, double, double, int, double, double, TString, _ProcessFunction, _NamerFunction);
    void AddHistBox(TString, int, TString, TString, TString, TString, TString, TString, int, double, double, int, double, double, TString, _ProcessFunction);
    void AddHistBox(TString, int, TString, TString, TString, TString, TString, TString, int, double, double, int, double, double, TString);
    void AddHistBox(TString, int, TString, TString, TString, TString, int, double, double, TString, _ProcessFunction, _NamerFunction);
    void AddHistBox(TString, int, TString, TString, TString, TString, int, double, double, TString, _ProcessFunction);
    void AddHistBox(TString, int, TString, TString, TString, TString, int, double, double, TString);

    void AddGraphBox(TString, int, TString, TString, TString, TString, TString, TString, TString, _ProcessGraphFunction, _NamerFunction, int, int, int, int, int, int);
    void AddGraphBox(TString, int, TString, TString, TString, TString, TString, TString, TString, _ProcessGraphFunction, _NamerFunction);
    void AddGraphBox(TString, int, TString, TString, TString, TString, TString, TString, TString);

    TH1 **GetHistos(string name) { return (TH1**)HistBoxes[name]->GetHistosObj(); }
    TH1 *GetHist(string name, int n) { return (TH1*)HistBoxes[name]->GetHistObj(n); }
    HistBox *GetBox(string name) {return HistBoxes[name];}

    void Fill2d(string name, int n, double val1, double val2, double w = 1) { ((TH2F*)GetHist(name, n))->Fill(val1, val2, w); }
    void Fill1d(string name, int n, double val1, double w = 1) { ((TH1F*)GetHist(name, n))->Fill(val1); }
    void Fill2dIf(int enable, string name, int n, double val1, double val2) { 
      if (!enable) {return;}
      ((TH2F*)GetHist(name, n))->Fill(val1, val2); 
    }
    void Fill1dIf(int enable, string name, int n, double val1) { 
      if (!enable) {return;}
      ((TH1F*)GetHist(name, n))->Fill(val1); 
    }

    TGraphErrors* GetGraph(string name, int n) { return (TGraphErrors*)GraphBoxes[name]->GetGraphObj(n); }

    int FillGraph(string name, int n, double x, double y, double ex, double ey) { 
      TGraphErrors * g = GetGraph(name, n);
      g->AddPoint(x, y);
      int np = g->GetN();
      g->SetPointError(np - 1, ex, ey);
      return np;
    }

    int FillGraphIf(int enable, string name, int n, double x, double y, double ex, double ey) { 
      if (!enable) {return -1;}
      TGraphErrors * g = GetGraph(name, n);
      g->AddPoint(x, y);
      int np = g->GetN();
      g->SetPointError(np - 1, ex, ey);
      return np;
    }

    void ProcessBox(string name) { HistBoxes[name]->ProcessBox(); }
    void ProcessGraphBox(string name) { GraphBoxes[name]->ProcessBox(); }

    void ProcessHistBoxes() {
      for (auto& HistBox: HistBoxes) {
        HistBox.second->ProcessBox(); 
      }      
    };

    void ProcessGraphBoxes() {
      for (auto& GraphBox: GraphBoxes) {
        GraphBox.second->ProcessBox(); 
      }
    };

    void ProcessBoxes() {ProcessHistBoxes(); ProcessGraphBoxes();}
      
    void ProcessHistBoxesGroup(int g) {
      for (auto& HistBox: HistBoxes) {
        if (HistBox.second->GetGroup() != g) {continue;}
        HistBox.second->ProcessBox(); 
      }      
    };

    void ProcessGraphBoxesGroup(int g) {
      for (auto& GraphBox: GraphBoxes) {
        if (GraphBox.second->GetGroup() != g) {continue;}
        GraphBox.second->ProcessBox(); 
      }
    };

    void ProcessBoxesGroup(int g) {ProcessHistBoxesGroup(g); ProcessGraphBoxesGroup(g);}

};

void HistManager::AddHistBox( TString histType, int histN, TString histosName, TString histTitle, TString label1, TString label2, TString label3, TString label4,
  int xBins, double xMin, double xMax, int yBins, double yMin, double yMax,
  TString opti, _ProcessFunction processFunc, _NamerFunction namerFunc
) {

  if (namerFunc == nullptr) {namerFunc = namerUser_def;}
  if (processFunc == nullptr) {processFunc = procUser_def;}

  HistManager::HistBoxes.insert( {histosName.Data(), 
    new HistBox(outDir, useFolders, useIndividualFolders, boxGroup, histType, histN, histosName, histTitle, label1, label2, label3, label4, xBins, xMin, xMax, yBins, yMin, yMax, processFunc, namerFunc, anaVars, opti)
  } );
};

void HistManager::AddHistBox( TString histType, int histN, TString histosName, TString histTitle, TString label1, TString label2, TString label3, TString label4,
  int xBins, double xMin, double xMax, int yBins, double yMin, double yMax,
  TString opti, _ProcessFunction processFunc
) { HistManager::AddHistBox(histType, histN, histosName, histTitle, label1, label2, label3, label4, xBins, xMin, xMax, yBins, yMin, yMax, opti, processFunc, namerUser_def); };

void HistManager::AddHistBox( TString histType, int histN, TString histosName, TString histTitle, TString label1, TString label2, TString label3, TString label4,
  int xBins, double xMin, double xMax, int yBins, double yMin, double yMax,
  TString opti
) { HistManager::AddHistBox(histType, histN, histosName, histTitle, label1, label2, label3, label4, xBins, xMin, xMax, yBins, yMin, yMax, opti, procUser_def, namerUser_def); };

void HistManager::AddHistBox( TString histType, int histN, TString histosName, TString histTitle, TString label1, TString label2,
  int xBins, double xMin, double xMax,
  TString opti, _ProcessFunction processFunc, _NamerFunction namerFunc
) { HistManager::AddHistBox(histType, histN, histosName, histTitle, label1, label2, "", "", xBins, xMin, xMax, 0, 0, 0, opti, processFunc, namerFunc); };

void HistManager::AddHistBox( TString histType, int histN, TString histosName, TString histTitle, TString label1, TString label2,
  int xBins, double xMin, double xMax,
  TString opti, _ProcessFunction processFunc
) { HistManager::AddHistBox(histType, histN, histosName, histTitle, label1, label2, "", "", xBins, xMin, xMax, 0, 0, 0, opti, processFunc, namerUser_def); };

void HistManager::AddHistBox( TString histType, int histN, TString histosName, TString histTitle, TString label1, TString label2,
  int xBins, double xMin, double xMax, 
  TString opti
) { HistManager::AddHistBox(histType, histN, histosName, histTitle, label1, label2, "", "", xBins, xMin, xMax, 0, 0, 0, opti, procUser_def, namerUser_def); };

void HistManager::AddGraphBox(TString type, int num, TString name, TString title, TString l1, TString l2, TString l3, TString l4, 
  TString options, _ProcessGraphFunction proc, _NamerFunction namer, int s1, int s2, int s3, int s4, int s5, int s6
) {

  if (namer == nullptr) {namer = namerUser_def;}
  if (proc == nullptr) {proc = procGraphUser_def;}

  HistManager::GraphBoxes.insert( {name.Data(), 
    new GraphBox(outDir, useFolders, useIndividualFolders, boxGroup, type, num, name, title, l1, l2, l3, l4, proc, namer, anaVars, options, useGraphMulti, s1, s2, s3, s4, s5, s6)
  } );
}

void HistManager::AddGraphBox( TString type, int num, TString name, TString title, TString l1, TString l2, TString l3, TString l4, 
  TString options, _ProcessGraphFunction proc, _NamerFunction namer
) { HistManager::AddGraphBox(type, num, name, title, l1, l2, l3, l4, options, proc, namer, 1, 1, 1, 1, 1, 1); };

void HistManager::AddGraphBox( TString type, int num, TString name, TString title, TString l1, TString l2, TString l3, TString l4, TString options) 
{ HistManager::AddGraphBox(type, num, name, title, l1, l2, l3, l4, options, procGraphUser_def, namerUser_def, 1, 1, 1, 1, 1, 1); };









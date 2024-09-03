#pragma once
using namespace std; 

#include "anaTimeCal.h"
#define ANA_CLASS_NAME anaTimeCal

int ANA_CLASS_NAME::ParseHistVars(int histN, TString hOpt, void *anaVars, int &cry, int &sid, int& row, int& col, int& used) {

  AnaVars *vars = (AnaVars*) anaVars;
  used = 1; sid = -1;

  if (!hOpt.Contains("num(chan)") && !hOpt.Contains("num(cry)")) {return 0;}

  if (hOpt.Contains("num(chan)")) {
    cry = GEO.cryCha(histN), sid = GEO.sideCha(histN);
    row = GEO.rowCry[cry], col = GEO.colCry[cry];
    used = vars->RES.isUsed[cry][sid];
    return 1;
  } else if (hOpt.Contains("num(cry)")) {
    cry = histN; 
    row = GEO.rowCry[cry], col = GEO.colCry[cry];
    used = vars->RES.isUsed[cry][0] || vars->RES.isUsed[cry][1];
    return 1;
  }

  return 0;
}

void ANA_CLASS_NAME::SkipUnused(TH1* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

  if (histObj->GetEntries() < 5) {histSkipFlag = 1; return;}
  if (hOpt.Contains("writeSkip(on)")) { histSkipFlag = 1; return; }
  if (!hOpt.Contains("num(chan)") && !hOpt.Contains("num(cry)")) {return;}
  int row{0}, col{0}, cry{0}, sid{0}, used{0};
  ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
  if (used == 0) {histSkipFlag = 1; return;}
}

void ANA_CLASS_NAME::SkipUnusedGraph(TGraphErrors* histObj, int histN, int& histSkipFlag, void *anaVars, TString hOpt) { 

  if (hOpt.Contains("writeSkip(on)")) { histSkipFlag = 1; return; }
  if (!hOpt.Contains("num(chan)") && !hOpt.Contains("num(cry)")) {return;}
  int row{0}, col{0}, cry{0}, sid{0}, used{0};
  ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used);
  if (used == 0) {histSkipFlag = 1; return;}
  if (histObj->GetN() < 1) { histSkipFlag = 1; return; }
}

void ANA_CLASS_NAME::NamerDefault(int histN, TString& hTag, TString& hTitleTag, void *anaVars, TString hOpt) { 
    
  if (hOpt.Contains("tag(off)")) { hTag = "";  hTitleTag = "";  return; }
  if (hOpt.Contains("tag(arr)")) { hTag = Form("_%d", histN); hTitleTag = Form(" [%d]", histN); }

  int row{0}, col{0}, cry{0}, sid{0}, used{0};
  if (ParseHistVars(histN, hOpt, anaVars, cry, sid, row, col, used)) {
    if (hOpt.Contains("tag(rcs)")) { 
        hTag = Form("_%d%s_r%d_c%d_s%d", cry, sid==0?"L":"R", row, col, sid); 
        hTitleTag = Form(" [%d%s][%d|%d|%s]", cry, sid==0?"L":"R", row, col, sid==0?"L":"R"); 
        return; 
    }
    if (hOpt.Contains("tag(rc)")) { 
        hTag = Form("_%d_r%d_c%d", cry, row, col); 
        hTitleTag = Form(" [%d][%d|%d]", cry, row, col); 
    return; 
    }
  }

  hTag = Form("_%d", histN); hTitleTag = Form(" [%d]", histN);
}
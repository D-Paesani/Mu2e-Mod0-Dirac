#pragma once

#include "pars.h"
#include "TString.h"

using namespace std;


int chan2side(int n) {return n%2; }
int chan2cry(int n) { return floor(n/2); }
int cry2chan(int c, int s) {return 2*c+s;} 
//int mat2cry(int r, int c) {return }
//int chan2row(int r, int c) {return }
//int chan2col(int r, int c) {return }
        
void NamerChannel(int hN, TString& hTag, TString& hTitleTag) {
  hTag = Form("_%d_%d", chan2cry(hN), chan2side(hN)); 
  hTitleTag = Form(" [%d%s]", chan2cry(hN), chan2side(hN)==0 ? "L": "R");
}

void NamerArray(int n, TString& hTag, TString& hTitleTag) { 
  hTag = Form("_%d", n); 
  hTitleTag = Form(" [ch%d]", n); 
};

void NamerMatrix(int n, TString& hTag, TString& hTitleTag) { }

void SkipProc(TH1* histObj, int histN, int& histSkipFlag) { histSkipFlag=1; };



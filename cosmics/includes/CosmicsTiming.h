#pragma once

#include "AnaPars.h"
#include "Geom.h"
#include "TGraphHelper.h"
#include "RootIncludes.h"

using namespace std;

class CosmicsTiming {

public:

    int cellEneMin = 10;
    int cellEneMax = 35;
    int timeCutMin = -100;

    int minCryNum = 3;

    const int topLayer = 5;
    const int btmLayer = 0;

    const double verticalCutMin = 32.00;
    const double verticalCutMax = 36.00;
    
    const double fitPosErr = 10;
    const double fitYXChi2Cut = 10;
    const double cosThetaYXCut = 0.2;

    const double errTimLayer = 0.280; //rifare funzione 
    const double fitYZChi2Cut = 20;
    const int fitYZminPts = 4;

    double MIP_MPV_eRref = pars.MIP_MPV_eRef;

private:

    static const Geom geom;
    static const AnaPars pars;
    
    TF1 *yxFit = new TF1("yxFit", "[0]+[1]*x", -150, 150);

    struct _cell {
        int hit, row, col, cry, read;
        double x, y, ene, tim;
        double path{0};
    };

    struct _sel {

        _cell *cell;

        TGraphErrors *yxGraph, *yzGraph;

        int count{0};

        double eneTot{0};
        double eneLayer[geom.rowNo]{0};
        int cntLayer[geom.rowNo]{0};
        double timErrSqLayer[geom.rowNo]{0};
        int rowTop{-999}, rowBtm{999};
        double topX, btmX;
        double timeCentroid{0};
        double timeCentroidLayer[geom.rowNo]{0};
        
        double thetaYX, tanThetaYX, cosThetaYX, fitYXchi2, tanThetaErrYX;
        
        int layerCnt{0};
        double fitYZpar, fitYZparErr;
        double thetaYZ, tanThetaYZ, cosThetaYZ, fitYZchi2;

        TF1 *fitYX;

        TString cosmicType = "";
    };

public:

    _sel sel;
    _cell *cell = sel.cell;

    CosmicsTiming() {}

    int CheckEnergy(double e) { return (int)(e > cellEneMin && e < cellEneMax); }

    void Init(int maxhit) {

        sel = {};
        cell = new _cell[maxhit];
        sel.yxGraph = new TGraphErrors();
        sel.yzGraph = new TGraphErrors();
        sel.fitYX = yxFit;
    }

    int AddHit(int hit, int cry, double ene, double tim, int read = 0) {

        if (!CheckEnergy(ene)) {return -1;}
        if (tim < timeCutMin) {return -1;} 

        int row = geom.rowCry[cry], col = geom.colCry[cry];
        double x = geom.cryX[row][col], y = geom.cryY[row][col];

        cell[sel.count].hit = hit;
        cell[sel.count].cry = cry;
        cell[sel.count].row = row;
        cell[sel.count].col = col;
        cell[sel.count].read = read;
        cell[sel.count].x = x;
        cell[sel.count].y = y;
        cell[sel.count].ene = ene;
        cell[sel.count].tim = tim;

        sel.eneTot += ene;
        sel.eneLayer[row] += ene;

        if (row > sel.rowTop) { sel.rowTop = row; sel.topX = x; }
        if (row < sel.rowBtm) { sel.rowBtm = row; sel.btmX = x; }

        fillGraph(sel.yxGraph, y, x, fitPosErr, fitPosErr);

        sel.cntLayer[row]++; 
        sel.count++;        

        return 1;
    }

    void GetCosmicType() {

        if (sel.count < minCryNum) { sel.cosmicType = "NO"; return; }

        double diff = abs(sel.topX - sel.btmX);

        if (sel.rowTop == topLayer && sel.rowBtm == btmLayer) {

            if (sel.cntLayer[topLayer] == 1 && sel.cntLayer[btmLayer] == 1 && diff < verticalCutMin) {
                sel.cosmicType = "PV";
                return;
            }

            sel.cosmicType = "PD";
            return;
        }

        sel.cosmicType = "MM"; //aggiungere trigger per faccia/top T1 T2 + faccia/bottom B2 B2 + faccia/faccia FF
    }

    int FilterCosmic(TString filt) {

        if (sel.cosmicType == "NO") {return 0;}
        if (filt == "PA" && sel.cosmicType != "PD" && sel.cosmicType != "PV") {return 0;} 
        if (filt != "OFF" && filt != "PA" && sel.cosmicType != filt) {return 0;}
        return 1;
    }

    int FitTrajectoryYX() {

        yxFit->ReleaseParameter(0);
        yxFit->ReleaseParameter(1);
        yxFit->SetParameter(0, 0);
        yxFit->SetParameter(1, (sel.topX-sel.btmX) /  (geom.cryL * (sel.rowTop - sel.rowBtm) )  ); 
        yxFit->SetParLimits(0, -150, 150);
        //yxFit->SetParLimits(1, -10, 10);
        
        int res = sel.yxGraph->Fit(yxFit, "REMQ");
        if (res < 0) {return -1000;}       

        sel.tanThetaYX = yxFit->GetParameter(1);
        sel.tanThetaErrYX = yxFit->GetParError(1);
        sel.thetaYX = TMath::ATan(sel.tanThetaYX);
        sel.cosThetaYX = TMath::Cos(sel.thetaYX);
        sel.fitYXchi2 = yxFit->GetChisquare() / yxFit->GetNDF();

        if (sel.fitYXchi2 > fitYXChi2Cut) {return -100;}  
        // if (sel.cosThetaYX < cosThetaYXCut) {return - 10;}     
        return 1;
    }

    double ComputePath(int idx){
        
        double half = geom.cryL/2;
        double xmin = cell[idx].x - half, xmax = cell[idx].x + half;
        double ymin = cell[idx].y - half, ymax = cell[idx].y + half;
        double xtop = sel.fitYX->Eval(ymax);
        double xbtm = sel.fitYX->Eval(ymin);
        double yin, yout, path = -1;

        if ( xtop < xmax ) { //controllare i conti
            if (xtop > xmin) { 
                yin = ymax;
            } else {
                yin = sel.fitYX->GetX(xmin);
            }
        } else {
            yin = sel.fitYX->GetX(xmax);
        } 
        if (yin < ymin || yin > ymax) { cell[idx].path = path; return path; }

        if ( xbtm < xmax ) {
            if (xbtm > xmin) { 
                yout = ymin;
            } else {
                yout = sel.fitYX->GetX(xmin);
            }
        } else {
            yout = sel.fitYX->GetX(xmax);
        } 
        if (yout < ymin || yout > ymax || yin < yout) { cell[idx].path = path; return path; }

        path = (yin - yout) / sel.cosThetaYX;
        cell[idx].path = path;
        return path;
    }

    void CoumputePaths(){

        for (int i = 0; i < sel.count; i++) {
           ComputePath(i);
        }
    }

    void ComputeWeightSum() {

        for (int i = 0; i < sel.count; i++) {

            double ene = cell[i].ene, tim = cell[i].tim;
            sel.timeCentroid += ( tim + cell[i].y / (pars.lightSpeed * sel.cosThetaYX) ) * ene / sel.eneTot;
            sel.timeCentroidLayer[cell[i].row] += ene*tim / sel.eneLayer[cell[i].row];
        }
    }

    int ComputeSlopeYZ() {

        TGraphErrors *gra = sel.yzGraph;
        TF1 *yzFit = new TF1("yzFit", "[1] + [0]*(x-1.0)"); 

        if(1) {

            for (int i = 0; i < sel.count; i++) { //rifare per bene il codice
                
                fillGraph(gra, geom.rowNo - (double)cell[i].row + 1.0, cell[i].tim, 1/3.5, errTimLayer*(MIP_MPV_eRref/cell[i].ene) );
            }
        
        } else {

            double timeFirst;
            int layFirst{-1};
            double errPos = 0.0; //implementare errori timing

            for (int i = geom.rowNo - 1; i >= 0; i--) {

                if ( sel.cntLayer[i] >= 1 && sel.eneLayer[i] > sel.cntLayer[i] * cellEneMin) { 

                double tlay = sel.timeCentroidLayer[i];

                // if (layFirst < 0) { timeFirst = tlay; layFirst = i; } 

                fillGraph(gra, geom.rowNo - (double)i + 1.0, tlay, errPos, errTimLayer);

                }
            } 

        }

        sel.layerCnt = gra->GetN();
        int res = gra->Fit(yzFit, "EMQ"); 

        sel.fitYZchi2 = yzFit->GetChisquare() / max(1, yzFit->GetNDF());
        sel.fitYZpar = yzFit->GetParameter(0);
        sel.fitYZparErr = yzFit->GetParError(0);

        if(res < 0) {return -1000;}
        if(sel.layerCnt < fitYZminPts) {return -100;}
        if(sel.fitYZchi2 > fitYZChi2Cut) {return -10;}

        auto zf = [&](Double_t *x, Double_t *par){ 
            double tanxy = sel.tanThetaYX;
            return geom.cryL * ( + x[0] / pars.CsI_vp + ( 1.0 / pars.lightSpeed ) * TMath::Sqrt( 1 + tanxy * tanxy + x[0]*x[0] ) );       
        }; 

        TF1 zfun("zfun", &zf, TMath::Tan(-pars.pi/2), TMath::Tan(pars.pi/2), 1);

        sel.tanThetaYZ = zfun.GetX(sel.fitYZpar);
        sel.cosThetaYZ = TMath::Cos(TMath::ATan(sel.tanThetaYZ));
        sel.thetaYZ = TMath::ATan(sel.tanThetaYZ);
        
        return 1;
    }

    double GetTofCorrection3d(double laydiff = 1) { //controllare i conti

        double corr = ( 1.0 / pars.lightSpeed) * TMath::Sqrt( 1 + sel.tanThetaYX*sel.tanThetaYX + sel.tanThetaYZ*sel.tanThetaYZ);
        corr += sel.tanThetaYZ / pars.CsI_vp;
        return laydiff * corr;
    }

    double GetTofCorrection2d(double laydiff = 1) {

        double corr = 1 / ( pars.lightSpeed * sel.cosThetaYX );
        return laydiff * corr;
    }
















    

};













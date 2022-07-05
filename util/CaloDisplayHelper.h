#pragma once

#include "TH2Poly.h"
#include "TCanvas.h"

class CaloDisplayHelper
{

private:

    static const int cryNo = 51;

    double Xs[cryNo] = { -85, -51, -17, 17, 51, 85, -102, -68, -34, 0, 
    34, 68, 102, -119, -85, -51, -17, 17, 51, 85, 119, -136, -102, 
    -68, -34, 0, 34, 68, 102, 136, -119, -85, -51, -17, 17, 51, 85, 
    119, -102, -68, -34, 0, 34, 68, 102, -85, -51, -17, 17, 51, 85 };

    double Ys[cryNo] = { -102, -102, -102, -102, -102, -102, -68, -68, -68, 
    -68, -68, -68, -68, -34, -34, -34, -34, -34, -34, -34, -34, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 34, 34, 34, 34, 34, 34, 34, 34, 68, 68, 68, 68, 68, 
    68, 68, 102, 102, 102, 102, 102, 102 };

    double cryL = 34.0;

    TH2Poly *disp;

public:

    ~CaloDisplayHelper() {};
    CaloDisplayHelper() {};

    TH2Poly* GetDisplay() {return disp;}

    void GenCaloDisplayYX(TString name, TString title, TString label3, double xMin, double xMax, double yMin, double yMax) {

        disp = new TH2Poly(name.Data(), title.Data(), xMin, xMax, yMin, yMax); 
        disp->SetStats(0);

        for (int i = 0; i < cryNo; i++) {
            double half = cryL/2;
            disp->AddBin( Xs[i] - half, Ys[i] - half, Xs[i] + half, Ys[i] + half );
            disp->GetXaxis()->SetTitle("X [mm]");
            disp->GetYaxis()->SetTitle("Y [mm]");
            disp->GetZaxis()->SetTitle(label3.Data());
        }
    }

    TCanvas* GetDispCanvas(TString name, TString title, TString drawOptions, int logZ = 0) {

        TCanvas *canva = new TCanvas(name.Data());
        canva->SetTitle(title.Data());
        canva->cd();

        disp->Draw(Form("%s goff", drawOptions.Data()));
        canva->SetLogz(logZ);
        
        for (int i = 0; i < cryNo; i++) {
            double half = cryL/2;
            TBox *thisBox = new TBox( Xs[i] - half, Ys[i] - half, Xs[i] + half, Ys[i] + half );
            thisBox->SetLineColor(kBlue);
            thisBox->SetLineWidth(1);
            thisBox->SetFillStyle(0);
            thisBox->Draw("same l goff");
        }

        return canva;
    }
     
    

     
};




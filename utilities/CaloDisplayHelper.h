#pragma once

#include "Geom.h"
#include "TH2Poly.h"
#include "TCanvas.h"

TH2Poly* GenCaloDisplayYX(TString name, TString title, TString label3, double xMin, double xMax, double yMin, double yMax) {

    Geom GEO;

    TH2Poly *disp = new TH2Poly(name.Data(), title.Data(), xMin, xMax, yMin, yMax); 

    for (int i = 0; i < GEO.cryNo; i++) {
        int r = GEO.rowCry[i], c = GEO.colCry[i];
        double half = GEO.cryL/2;
        disp->AddBin( GEO.cryX[r][c] - half, GEO.cryY[r][c] - half, GEO.cryX[r][c] + half, GEO.cryY[r][c] + half );
        disp->GetXaxis()->SetTitle("X [mm]");
        disp->GetYaxis()->SetTitle("Y [mm]");
        disp->GetZaxis()->SetTitle(label3.Data());
    }

    return disp;
}

TCanvas* GenCaloDisplayYXCanvas(TH2Poly *disp, TString name, TString title, TString drawOptions, int logZ = 0) {

    Geom GEO;

    TCanvas *canva = new TCanvas(name.Data());
    canva->SetTitle(title.Data());
    canva->cd();

    disp->Draw(drawOptions.Data());
    canva->SetLogz(logZ);
    
    for (int i = 0; i < GEO.cryNo; i++) {
        int r = GEO.rowCry[i], c = GEO.colCry[i];
        double half = GEO.cryL/2;
        TBox *thisBox = new TBox( GEO.cryX[r][c] - half, GEO.cryY[r][c] - half, GEO.cryX[r][c] + half, GEO.cryY[r][c] + half );
        thisBox->SetLineColor(kBlue);
        thisBox->SetLineWidth(1);
        thisBox->SetFillStyle(0);
        thisBox->Draw("same l");
    }

    return canva;
}
     
     
     
     
  
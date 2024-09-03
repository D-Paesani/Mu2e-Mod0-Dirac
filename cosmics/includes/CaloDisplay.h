#pragma once

#include "Geom.h"
#include "TH2Poly.h"
#include "TCanvas.h"

class CaloDisplay
{

private:

    Geom *geo = new Geom();
    TH2Poly *disp;
    TCanvas *canva;
    double half = geo->cryL/2;

public:

    ~CaloDisplay() { disp->Delete(); };

    TH2Poly* GetDisplay() {return disp;}

    TCanvas* GetCanvas() {
        canva->Update();
        return canva;
    }

    int FillCry(int icry, double w = 1) {
        return Fill(geo->cryX[geo->rowCry[icry]][geo->colCry[icry]], geo->cryY[geo->rowCry[icry]][geo->colCry[icry]], w);
    }

    int FillCrySide(int icry, int s, double w = 1) {
        return FillSide(geo->cryX[geo->rowCry[icry]][geo->colCry[icry]], geo->cryY[geo->rowCry[icry]][geo->colCry[icry]], s, w);
    }

    int Fill(double x, double y, double w = 1) {
        return disp->Fill(x, y, w);
    }

    int FillSide(double x, double y, int s, double w = 1) {

        if (s == -1) {
            Fill(x - 2.0, y, w);
            return Fill(x + 2.0, y, w);
        }

        return Fill(x - 2.0 * (1 - 2 * s), y, w);
    }

    CaloDisplay(TString name, TString title, TString label3, double xMin, double xMax, double yMin, double yMax, TString drawOptions, int logZ = false, int LR = false) {

        if (title == "") {title = name;}
        
        disp = new TH2Poly(name.Data(), title.Data(), xMin, xMax, yMin, yMax); 
        disp->SetStats(0);

        for (int i = 0; i < geo->cryNo; i++) {
            int r = geo->rowCry[i], c = geo->colCry[i];
            if (LR) {
                disp->AddBin( geo->cryX[r][c] - half, geo->cryY[r][c] - half, geo->cryX[r][c] + 0, geo->cryY[r][c] + half );
                disp->AddBin( geo->cryX[r][c] - 0, geo->cryY[r][c] - half, geo->cryX[r][c] + half, geo->cryY[r][c] + half );
            } else {
                disp->AddBin( geo->cryX[r][c] - half, geo->cryY[r][c] - half, geo->cryX[r][c] + half, geo->cryY[r][c] + half );
            }
            disp->GetXaxis()->SetTitle("X [mm]");
            disp->GetYaxis()->SetTitle("Y [mm]");
            disp->GetZaxis()->SetTitle(label3.Data());
        }
        
        canva = new TCanvas(name.Data());
        canva->SetTitle(name.Data());
        canva->cd();

        disp->Draw(drawOptions.Data());
        canva->SetLogz(logZ);
    
        for (int i = 0; i < geo->cryNo; i++) {
            int r = geo->rowCry[i], c = geo->colCry[i];

            TBox *thisBox = new TBox( geo->cryX[r][c] - half, geo->cryY[r][c] - half, geo->cryX[r][c] + half, geo->cryY[r][c] + half );
            thisBox->SetLineColor(kBlack);
            thisBox->SetLineWidth(2);
            thisBox->SetFillStyle(0);
            thisBox->Draw("same l");
            
            if (!LR) { continue; }
            TLine *lin = new TLine( geo->cryX[r][c], geo->cryY[r][c] - 0.7*half, geo->cryX[r][c], geo->cryY[r][c] + 0.7*half );
            lin->SetLineColor(kBlack);
            lin->SetLineWidth(1);
            lin->Draw("same l");
        }

    }



    

     
};




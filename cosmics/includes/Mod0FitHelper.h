//aggiornato 17 giu

#include "RootIncludes.h"

int fitChargeRawLandau(TH1 *hist, double& peak, double &peakErr, double &sigma, double &sigmaErr, TString options) {
    
    gStyle->SetOptFit(1111);
    hist->SetStats(1111); 

    double minTmp = hist->GetXaxis()->GetXmin();
    double maxTmp = hist->GetXaxis()->GetXmax();
    
    hist->GetXaxis()->SetRangeUser(1000, maxTmp);
    // const int tspn = 1;
    // TSpectrum s(tspn);
    // s.Search(hist, 10, "nodraw");
    // double *pks = s.GetPositionX();
    // peak = *std::max_element(pks, pks + tspn);
    peak = hist->GetBinCenter(hist->GetMaximumBin());
    hist->GetXaxis()->SetRangeUser(minTmp, maxTmp);

    sigma = 400;
    TF1 land = TF1("land", "landau", peak - 2*sigma, peak + 3*sigma);
    land.SetParameters(hist->Integral()/2, peak, sigma);

    if ( (hist->Fit(&land, "REMQ")) < 0 ) {return -1;}     
    peak = land.GetParameter(1); 
    sigma = land.GetParameter(2);

    land.SetParameters(land.GetParameter(0), peak, sigma);
    land.SetRange(peak - 1.2*sigma, peak + 3*sigma);
    if ( (hist->Fit(&land, "REMQ")) < 0 ) {return -1;}     

    peak = land.GetParameter(1); 
    peakErr = land.GetParError(1);
    sigma = land.GetParameter(2); 
    sigmaErr = land.GetParError(2);

    if(land.GetChisquare()/land.GetNDF() > 1.5) {return -1;}

    return 1;
}

int fitEneLandau(TH1 *hist, double& peak, double &peakErr, double &sigma, double &sigmaErr, TString options) {
    
    gStyle->SetOptFit(1111);
    hist->SetStats(1111); 

    double minTmp = hist->GetXaxis()->GetXmin();
    double maxTmp = hist->GetXaxis()->GetXmax();
    
    hist->GetXaxis()->SetRangeUser(5, maxTmp);
    const int tspn = 1;
    TSpectrum s(tspn);
    s.Search(hist, 2, "nodraw");
    double *pks = s.GetPositionX();
    peak = *std::max_element(pks, pks + tspn);
    hist->GetXaxis()->SetRangeUser(minTmp, maxTmp);

    sigma = 2;
    TF1 land = TF1("land", "landau", peak - 5, peak + 25);
    land.SetParameters(hist->Integral()/2, peak, sigma);

    if ( (hist->Fit(&land, "REMQ")) < 0 ) {return -1;}     
    peak = land.GetParameter(1); 
    sigma = land.GetParameter(2);
    sigma = max(sigma, 7.0);

    land.SetParameters(land.GetParameter(0), peak, sigma);
    land.SetRange(peak - 0.22*sigma, peak + 1.2*sigma);
    if ( (hist->Fit(&land, "REMQ")) < 0 ) {return -1;}     

    peak = land.GetParameter(1); 
    peakErr = land.GetParError(1);
    sigma = land.GetParameter(2); 
    sigmaErr = land.GetParError(2);

    return 1;
}

int fitTimeGausStandard(TH1 *hist, double& peak, double &peakErr, double &sigma, double &sigmaErr, TString options) {
    
    gStyle->SetOptFit(1);
    hist->SetStats(1111111111);

    peak = hist->GetBinCenter(hist->GetMaximumBin());
    sigma = hist->GetRMS();
    
    TF1 gaus = TF1("g", "gaus");
    gaus.SetParameter(1, peak); 
    gaus.SetParameter(2, sigma);
    gaus.SetRange(peak - 5, peak + 5);
    if ( (hist->Fit(&gaus, "REMQ")) < 0 ) {return -1;}     
    peak = gaus.GetParameter(1); 
    sigma = gaus.GetParameter(2);
    gaus.SetParameter(1, peak); 
    gaus.SetParameter(2, sigma);
    gaus.SetRange(peak - 4*sigma, peak + 4*sigma); 
    if ( (hist->Fit(&gaus, "REMQ")) < 0 ) {return -1;}     
    peak = gaus.GetParameter(1); 
    sigma = gaus.GetParameter(2);
    peakErr = gaus.GetParError(1);
    sigmaErr = gaus.GetParError(2); 
    
    return 1;
}


            
            







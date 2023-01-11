
int fitChargeRawLandau(TH1 *hist, double& peak, double &peakErr, double &sigma, double &sigmaErr, TString options) {
    
    gStyle->SetOptFit(1111111);
    hist->SetStats(1111111);

    double minTmp = hist->GetXaxis()->GetXmin();
    double maxTmp = hist->GetXaxis()->GetXmax();
    
    hist->GetXaxis()->SetRangeUser(1500, maxTmp);
    const int tspn = 1;
    TSpectrum s(tspn);
    s.Search(hist, 10, "nodraw");
    double *pks = s.GetPositionX();
    peak = *std::max_element(pks, pks + tspn);
    hist->GetXaxis()->SetRangeUser(minTmp, maxTmp);

    sigma = 450;
    TF1 land = TF1("land", "landau", peak - 1000, peak + 2000);
    land.SetParameters(hist->Integral()/2, peak, sigma);

    if ( (hist->Fit(&land, "REMQ")) < 0 ) {return -1;}     
    peak = land.GetParameter(1); 
    sigma = land.GetParameter(2);

    land.SetParameters(land.GetParameter(0), peak, sigma);
    land.SetRange(peak - 1.5*sigma, peak + 4.5*sigma);
    if ( (hist->Fit(&land, "REMQ")) < 0 ) {return -1;}     

    peak = land.GetParameter(1); 
    peakErr = land.GetParError(1);
    sigma = land.GetParameter(2); 
    sigmaErr = land.GetParError(2);

    return 1;
}

int fitTimeGausStandard(TH1 *hist, double& peak, double &peakErr, double &sigma, double &sigmaErr, TString options) {
    
    gStyle->SetOptFit(1111111);
    hist->SetStats(1111111);

    peak = hist->GetBinCenter(hist->GetMaximumBin());
    sigma = hist->GetRMS();
    TF1 gaus = TF1("g", "gaus");
    gaus.SetParameter(1, peak); 
    gaus.SetParameter(2, sigma);
    if ( (hist->Fit(&gaus, "EMQ")) < 0 ) {return -1;}     
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


            
            







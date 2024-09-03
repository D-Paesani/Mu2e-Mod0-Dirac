



void branch2histo1d( TH1* hObj, TTree* tree, TString val, TString cut = "" ){
    
    TH1 *hist;
    TString Val = Form( "%s>>histo(%d, %f, %f)", val.Data(), (int)hObj->GetNbinsX(), (double)hObj->GetXaxis()->GetXmin(), (double)hObj->GetXaxis()->GetXmax());
    tree->Draw( Val, cut.Data(), "goff");
    hist = (TH1*)gDirectory->Get("histo"); 
    hObj->Add(hist);
    hist->Delete();

}

void branch2histo2d( TH1* hObj, TTree* tree, TString val, TString cut = "" ){
    
    TH1 *hist;
    int xBins = (int)hObj->GetNbinsX(), yBins = (int)hObj->GetNbinsY();
    double xFrom = (double)hObj->GetXaxis()->GetXmin(), xTo = (double)hObj->GetXaxis()->GetXmax();
    double yFrom = (double)hObj->GetYaxis()->GetXmax(), yTo = (double)hObj->GetYaxis()->GetXmax();
    TString Val = Form( "%s>>histo(%d, %f, %f, %d, %f, %f)", val.Data(), xBins, xFrom, xTo, yBins, yFrom, yTo);
    tree->Draw( Val, cut.Data(), "goff");
    hist = (TH1*)gDirectory->Get("histo"); 
    hObj->Add(hist);
    hist->Delete();

}





void branch2histo1d( TH1* hObj, TTree* tree, TString val, TString cut = "" ){
    
    TH1 *hist;
    TString Val = Form( "%s>>histo(%d, %f, %f)", val.Data(), (int)hObj->GetNbinsX(), (double)hObj->GetXaxis()->GetXmin(), (double)hObj->GetXaxis()->GetXmax());
    tree->Draw( Val, cut.Data(), "goff");
    hist = (TH1*)gDirectory->Get("histo"); 
    hObj->Add(hist);
    hist->Delete();

}
#define analysis_mod0_cxx
#include "analysis_mod0_dirac.h"
//#include <TH2.h>
//#include <TStyle.h>
//#include <TCanvas.h>
//#include <string>
//#include <stdlib.h>
//#include <iostream> // Already in analysis_mod.h
#include "TF1.h"
//#include "logn.h"
#include <TSpline.h>
#include <algorithm>
TSpline3 *sp3;

//=============================================================================
// Fit function for template
//=============================================================================

Double_t spline_fit(Double_t *x, Double_t *par)
{
  double f1;
  f1=par[0]*sp3->Eval(x[0]-par[1])+par[2];
  return f1;  
}

//=============================================================================
// User variables
//=============================================================================

const int   Nboard =  1;
const int   Nchan  = 20;

int         Itmp;
int         ItmpLaser;
float       Qtmp, Ttmp, Btmp, Vtmp;
//float       ped1tmp, ped2tmp;
int         Stmp;
Double_t    wavetmp[maxNsample];
Double_t    twavetmp[maxNsample];
int         BoaTmp, ChaTmp, DaqTmp, HitTmp, RowTmp, ColTmp, SipmTmp, CryTmp;
int         rowID[Nboard][Nchan], colID[Nboard][Nchan], sipmID[Nboard][Nchan];
int         HitNum[Nboard][Nchan];
float       Time, Chi2;
float       fitPar[3]={0.}, fitErr[3]={0.}, fitTmea=0., fitChi2=0.;

//=============================================================================
// Loop function
//=============================================================================

//void analysis_mod0::Loop()
void analysis_mod0::Loop(TString OutputFile, int evflag)
{
//   In a ROOT session, you can do:
//      Root > .L analysis_mod0.C
//      Root > analysis_mod0 t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   int skipfit;
   float Qcut = 0;

   if( evflag == 0 ) { // study for cosmics;
     Qcut    = 2; // prima era 10;
     skipfit = 0;
   } 
   else if( evflag==1 ){
     Qcut = -100.;
     skipfit= 1;
   }
   else {
     cout << "Unknown evflag " << evflag << ": Fatal error" << endl;
     return;
   }

   //**************************************************************************
   // Input files: channel maps + template to fit timing
   //**************************************************************************

   FILE *mapFile;
   mapFile = fopen("mod0_dirac_map.dat", "r");
   for ( int Iloop=0; Iloop<Nboard*Nchan; Iloop++ ){
     fscanf(mapFile, "%d %d %d %d %d", &BoaTmp, &ChaTmp, &RowTmp, &ColTmp, &SipmTmp );
     rowID[BoaTmp][ChaTmp] = RowTmp;
     colID[BoaTmp][ChaTmp] = ColTmp;
     sipmID[BoaTmp][ChaTmp] = SipmTmp;
     //cout << Iloop << " " << " " << BoaTmp << " " << ChaTmp << endl;
   }
   fclose(mapFile);

   TFile* f0 = new TFile("../roottople/splines4_run39.root");
   TGraphErrors* gt = (TGraphErrors*)f0->Get("gtempl");

   //**************************************************************************
   // Output file
   //**************************************************************************

   cout << "Selected output file: " << OutputFile << endl;
   TFile *outFile = new TFile(OutputFile,"recreate");
   TTree *mod0 = new TTree("mod0","mod0");
   BookOutput(mod0); // Book histos and ntuple

   //**************************************************************************
   // Loop on entries
   //**************************************************************************

   int sum=0;
   Long64_t nentries =fChain->GetEntriesFast();
   //cout<<"nentries == "<<nentries<<endl;
   if(evflag==1)nentries = 5000;

   cout <<"nentries after evflag "<< nentries<<endl;
   Long64_t nbytes = 0, nb = 0;
   for( Long64_t jentry=0; jentry<nentries;jentry++ ) { //nentries
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     // if (Cut(ientry) < 0) continue;

     if( jentry%1000==0 ) cout << "Number of processed events: " << jentry << endl;
     nrun    = run;
     nsubrun = subrun; 
     evnum   = nevt;
     _isLaser=0;

     /*
     if( b1_charge7>2000 ){
       _isLaser=1;
       skipfit==1;
     }
     */
     int hitTot = 0; 

     // Zeroing number of hits per channel
     for( int jBoard=0; jBoard<Nboard; jBoard++ ){
       for( int jChan=0; jChan<Nchan; jChan++ ){
	 HitNum[jBoard][jChan] = 0;  
       }
     }
     
     if( nhits>20 ){
       cout << "ERROR: too many hits in the DIRAC input ntuple " << nhits << ".. reducing size to 20" << endl;
       nhits = 20;
     }
     
     for( int jHit=0; jHit<nhits; jHit++ ){
       
       int Status = GetValues(jHit, evflag);
       if( Status!=0 ) cout << "GetValues error: " << Status << " " <<
			 ". Skipping event/DAQid: " << nevt << " " << DaqTmp << endl;

       if( Qtmp>Qcut ){                 ///// Check Qcut value!!!
	 iDAQ[hitTot] = DaqTmp;
	 iHit[hitTot] = HitTmp;
	 iRow[hitTot] = RowTmp;
	 iCol[hitTot] = ColTmp;
	 SiPM[hitTot] = SipmTmp;
	 iCry[hitTot] = CryTmp;
	 Qval[hitTot] = Qtmp;
	 Tval[hitTot] = Ttmp;
	 Vmax[hitTot] = Vtmp;
	 iMax[hitTot] = Itmp;
	 nSamples[hitTot] = Stmp;
	 std::copy( wavetmp, wavetmp+maxNsample, wave[hitTot]);
	 std::copy(twavetmp,twavetmp+maxNsample,tWave[hitTot]);

	 bline[hitTot] = Btmp;

	 lognTime[hitTot] = fitTmea;
	 lognChi2[hitTot] = fitChi2;
	 // Timing from template fit
	 if( skipfit==0){
	   //cout<<"fitting "<<Iboard<<"  "<<Ichan<<endl;
	   getTemplateFit(gt);
	 }
	 templTime[hitTot] = fitTmea;
	 templChi2[hitTot] = fitChi2;
	 std::copy(fitPar,fitPar+3,templFit[hitTot]);
	 std::copy(fitErr,fitErr+3,templErr[hitTot]);
	 
	 hitTot++;
	 if( hitTot>maxNsipm ){
	   cout << "ERROR: too many hits " << hitTot << ". You need to modify the size in the include file" << endl;
	 }
       }
     }

     nHits = hitTot;

     mod0->Fill();
      
   } // end loop on events
   cout<<"laser evts"<<sum<<endl;
   cout << "Closing output file" << endl;

   outFile->cd();
   mod0->Write();
   outFile->Close();

}

//=============================================================================
// Ntuple booking
//=============================================================================
void analysis_mod0::BookOutput(TTree *mod0)
{
  // Create ROOT tree output

  mod0->SetAutoSave(1000);

  mod0->Branch("nrun"      ,&nrun,      "nrun/I");          // ***NEW***
  mod0->Branch("nsubrun"   ,&nsubrun,   "nsubrun/I");       // ***NEW***
  mod0->Branch("evnum"     ,&evnum,     "evnum/I");
  mod0->Branch("nHits"     ,&nHits,     "nHits/I");
  mod0->Branch("isLaser"   ,&_isLaser,  "isLaser/I");       // number of SiPM seeing a laser pulse   
  mod0->Branch("iDAQ"      ,&iDAQ,      "iDAQ[nHits]/I");
  mod0->Branch("iHit"      ,&iHit,      "iHit[nHits]/I");
  mod0->Branch("iRow"      ,&iRow,      "iRow[nHits]/I");
  mod0->Branch("iCol"      ,&iCol,      "iCol[nHits]/I");
  mod0->Branch("SiPM"      ,&SiPM,      "SiPM[nHits]/I");
  mod0->Branch("iCry"      ,&iCry,      "iCry[nHits]/I");
  mod0->Branch("iMax"      ,&iMax,      "iMax[nHits]/I");
  mod0->Branch("Qval"      ,&Qval,      "Qval[nHits]/D");
  mod0->Branch("Tval"      ,&Tval,      "Tval[nHits]/D");
  mod0->Branch("Vmax"      ,&Vmax,      "Vmax[nHits]/D");
  mod0->Branch("nSamples"  ,&nSamples,  "nSamples[nHits]/I");    // ***NEW***
  mod0->Branch("wave"      ,&wave,      "wave[nHits][200]/D");
  mod0->Branch("tWave"     ,&tWave,     "tWave[nHits][200]/D");  // ***NEW***
  mod0->Branch("bline"     ,&bline,     "bline[nHits]/D");
  mod0->Branch("lognTime"  ,&lognTime,  "lognTime[nHits]/D");
  mod0->Branch("lognChi2"  ,&lognChi2,  "lognChi2[nHits]/D");
  mod0->Branch("templTime" ,&templTime, "templTime[nHits]/D");
  mod0->Branch("templChi2" ,&templChi2, "templChi2[nHits]/D");
  mod0->Branch("templFit"  ,&templFit,  "templFit[nHits][3]/D");
  mod0->Branch("templErr"  ,&templErr,  "templErr[nHits][3]/D");

}

//=============================================================================
// Get values for a given board/channel
//=============================================================================
int analysis_mod0::GetValues(int jHit, int evflag)
{
  // jHit ==>>> jHit

  int   nVal;
  float TWmax, Time, bSum, Qsum;
  //int   QbinMin, QbinMax;
  float const camp = 1.;
  float dt = 1/camp;
  //float const Qpeak_min      =  60.; //  -60 ns from Tave
  //float const Qpeak_max      = 190.; // +190 ns from Tave

  //float const QpeakLaser_min = 150.; //  -150 ns from Tave
  //float const QpeakLaser_max = 200.; // +200 ns from Tave

  BoaTmp  = boardID[jHit]-1;
  ChaTmp  = chanID[jHit];
  DaqTmp  = 100*BoaTmp + ChaTmp;
  
  HitNum[BoaTmp][ChaTmp]++;
  HitTmp = HitNum[BoaTmp][ChaTmp];

  RowTmp  = rowID[BoaTmp][ChaTmp];
  ColTmp  = colID[BoaTmp][ChaTmp];
  SipmTmp = sipmID[BoaTmp][ChaTmp];

  CryTmp = Get_iCry(RowTmp, ColTmp);
  int iVal = Get_iSiPM(RowTmp, ColTmp, SipmTmp);
  //CryTmp = iVal;

  Qtmp = -999.;
  Btmp = 2048.;

  int ErrFlag = 0;

  // time[jHit] is the time (in counts) of the first hit
  // peakpos[jHit] is the element of the array (starting from 1) corresponding 
  //               to the maximum amplitude
  Ttmp = (time[jHit]+peakpos[jHit]-1)*5;  // Time (in ns) of the max amplitude
  Itmp = peakpos[jHit];
  Stmp   = nofsamples[jHit];
  if( Stmp>maxNsample ){
    cout << "ERROR: wave out of boundaries " << Stmp << ".. reducing size to " << maxNsample << endl;
    Stmp = maxNsample;
  }

  
  int jStart = firstsample[jHit];
  int jEnd   = jStart + Stmp;
  
  // Get baseline
  
  nVal = 0;                 // Number of samples for baseline evaluation
  bSum = 0.;
  TWmax = Ttmp - 60.;       // baseline TW: 60 ns before the start of the wave

  for(int iSample=jStart; iSample<jEnd; iSample++ ){
    if( chan[iSample]==ChaTmp && boardid[iSample]==BoaTmp+1 ){

      Time = timens[iSample];

      if ( Time<TWmax ){
	nVal++;
	bSum = bSum + ADC[iSample];
      }
    }
    else{
      cout << "Wrong chanId consistency for event/DAQid: " << 
	jStart << " " << jEnd << " " << Stmp << " " <<  
	nevt << " " << DaqTmp << " " << BoaTmp+1 << " " << boardid[iSample] << 
	" " << ChaTmp << " " << chan[iSample] << endl; 
      ErrFlag = 1;
      continue;
    }
  }

  if( nVal!=0 ){
    Btmp = bSum / nVal;
  } 
  else{
    cout << "Run/Nev/DAQid " << run << " " << nevt << " " << DaqTmp << 
      ": no samples for baseline evaluation" << endl;
    ErrFlag = 2;
  }

  //
  //  Fill wave/twave & get charge, with baseline subtracted event-by-event
  //

  if( ErrFlag==0 ){

    Qsum  = 0.;
    int myIdx = 0;

    std::fill(wavetmp, wavetmp+maxNsample, 0.);
    std::fill(twavetmp, twavetmp+maxNsample, 0.);
    for(int iSample=jStart; iSample<jEnd; iSample++ ){
      
      Time = timens[iSample];
      twavetmp[myIdx] = Time;
      wavetmp [myIdx] = ADC[iSample]-Btmp;

      if ( Time>TWmax ){
	Qsum = Qsum + wavetmp[myIdx];
      }
      myIdx++;
    }
    Qtmp = Qsum; //*dt/50.;
    Vtmp = wavetmp[Itmp];
  }

  return ErrFlag;

}

//=============================================================================
// Get time from template fit to waveform
//=============================================================================
void analysis_mod0::getTemplateFit(TGraphErrors* gt)
{
  
  TGraphErrors *gwf;
  TF1          *fitf;

  float const xstart = 60.; // Fit interval:
  float const xend   = 20.; // [-60ns,+20ns] w.r.t. wave peak

  double ex[maxNsample] = {0.05}; //X-error not useful .. just for display graphs
  double ey[maxNsample];
  std::fill_n(ey,maxNsample,3.6);
  int    t_peak = 0 ;
  double v_max  = 0.;
  double CF     = 0.2;

  fitTmea = 0.;
  fitChi2 = 0.;
  std::fill_n(fitPar,3,0.);
  std::fill_n(fitErr,3,0.);

  if( RowTmp<10 ){   // Skip scintillators
    if( Qtmp>70. ){  // Replace with cut in calibrated MeV Q*20/Qmip>10
 
      gwf = new TGraphErrors(160,twavetmp,wavetmp,ex,ey);
      v_max  = wavetmp[Itmp];
      t_peak = twavetmp[Itmp];
      fitf = new TF1("fitf",spline_fit,t_peak-xstart,t_peak-xend,3);
      sp3 = new TSpline3("sp3", gt,0,0.,800.);

      // Three parameter fit ( 0: norm, 1:t0, 2:baseline )
      //Set start parameters and limits
      fitf->SetParameter(0,200.);
      fitf->SetParLimits(0,30,800);
      float tstart = t_peak-30;
      fitf->SetParameter(1,tstart);
      fitf->SetParLimits(1,tstart-xstart,tstart+xend);
      fitf->SetParameter(2,  0.);
      fitf->SetParLimits(2,-10,10);
      
      gwf->Fit("fitf","RBOQ"); 
      //gwf->Fit("fitf","RBOQ");
      fitTmea   = fitf->GetX(v_max*CF);
      fitPar[0] = fitf->GetParameter(0);
      fitPar[1] = fitf->GetParameter(1);
      fitPar[2] = fitf->GetParameter(2);
      fitErr[0] = fitf->GetParError(0);
      fitErr[1] = fitf->GetParError(1);
      fitErr[2] = fitf->GetParError(2);
      //cout << "Fit: " << fitPar[1] << " ";
      if( fitf->GetNDF()!=0 ) fitChi2 = fitf->GetChisquare()/fitf->GetNDF();
      //cout << fitChi2<<endl;

      delete gwf;
      delete fitf;
      delete sp3;
      }
  }
}

//=============================================================================
// Get time from logn fit to waveform
//=============================================================================
void analysis_mod0::getTlogn(float Time, float Chi2)
{

  int   Imin = 50;
  float Vmax = wavetmp[Itmp];
  float Vmin = 0.;
  float minVal = 0.03*Vmax;          // Fit from Vmin = 3% of Vmax
  float Tpeak = twavetmp[Itmp];
  float charge_min = 200.;
  float charge_max = 450.;

  fitTmea = 0.;
  fitChi2 = 0.;

  /*
  // Itmp: Max of waveform    
  for( int iLoop=Itmp; iLoop>Itmp-20; iLoop-- ){
    if( wavetmp[iLoop]<minVal && Imin == 50 ){
      Imin = iLoop;
      //cout<<"min "<<iLoop<<" "<<Itmp<<endl;
      Vmin = wavetmp[iLoop];
    }
  }
  */

  /*
  if( Vmax>50 && time[Itmp]>charge_min && time[Itmp]<charge_max && Vmax < 1400 ){
    Double_t errors[1024] = {0.};
    Double_t empty [1024] = {0.};

    TGraphErrors *g_crystal = new TGraphErrors(100, time, wavetmp, empty, errors);

    // TF1: Last variable is number of fit parameters
    TF1 *f_logn = new TF1( "f_logn", logn, time[Imin], time[Itmp-3], 4 );

    f_logn->SetParNames("#eta", "#sigma", "t_{0}", "Norm");
    //f_logn->SetParLimits(0,-5.,-0.001);//prove con giani
    f_logn->SetParLimits(0,-2.,-0.001);
    f_logn->SetParLimits(1,10.,100);
    //f_logn->SetParLimits(2,200,t_peak+50.);
    //if( Iboard==0 ) f_logn->SetParLimits(2,300,350);
    //if( Iboard==1 ) f_logn->SetParLimits(2,250,300);
    //f_logn->SetParLimits(3,1.,1000000);
    f_logn->SetParLimits(3,100.*Vmax,200.*Vmax);
    f_logn->SetParameters(-1., 50., Tpeak-10., Vmax*140);

    //f_pol->SetParLimits(2, 1800, 1900);
    //g_crystal->Fit("f_pol", "RBOQ","",time[i_min], time[i_max-1]);
    f_logn->FixParameter(2,Tpeak);
  */
    /*
      int result = g_crystal->Fit("f_pol", "RBOQ","",time[i_min], time[i_max-1]);
      result = g_crystal->Fit("f_pol", "RBOQ");
      f_pol->SetParLimits(2,200,t_peak+50.);//prove
      double eta_min = f_pol->GetParameter(0)*1.25;
      double eta_max = f_pol->GetParameter(0)*0.75;
      f_pol->SetParLimits(0,eta_min, eta_max);//prove
      result = g_crystal->Fit("f_pol", "RBOQ");
      result = g_crystal->Fit("f_pol", "RBOQ");
      if (result == 0) {
    
    double const_frac=0.05;//1;
    //tmean[Iboard][Ichan] = f_pol->GetX((v_max + v_min)/2.*const_frac);
    tmean[Iboard][Ichan]  = f_pol->GetX(v_max*const_frac);//-timemu[Iboard][Ichan];
    chi2[Iboard][Ichan]   = f_pol->GetChisquare();
    eta[Iboard][Ichan]    = f_pol->GetParameter(0);
    sigma[Iboard][Ichan]  = f_pol->GetParameter(1);
    t0[Iboard][Ichan]     = f_pol->GetParameter(2);
    N[Iboard][Ichan]      = f_pol->GetParameter(3);
    
    ndof[Iboard][Ichan] = f_pol->GetNDF();
    tedge[Iboard][Ichan]= f_pol->GetX(v_max*const_frac)-4*int(f_pol->GetX(v_max*const_frac)/4);
  } else {
    tmean[Iboard][Ichan] = -99;
  }

  */

}

//=============================================================================
// Get iCry value for a given row/column
//=============================================================================
int analysis_mod0::Get_iCry(int jRow, int jCol)
{
  int   nCol[7] = { 6, 7, 8, 9, 8, 7, 6 };

  int iVal = jCol;
  for( int iRow=0; iRow<jRow; iRow++ ){
    iVal = iVal + nCol[iRow];
  }

  return iVal;
}

//=============================================================================
// Get iSiPM value for a given row/column/sipm
//=============================================================================
int analysis_mod0::Get_iSiPM(int jRow, int jCol, int jSiPM)
{
  int   nCol[7] = { 6, 7, 8, 9, 8, 7, 6 };

  int iVal = jCol;
  for( int iRow=0; iRow<jRow; iRow++ ){
    iVal = iVal + nCol[iRow];
  }

  iVal = 2*iVal + jSiPM;
  return iVal;
}



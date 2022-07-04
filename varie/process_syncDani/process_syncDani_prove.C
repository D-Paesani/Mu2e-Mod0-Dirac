#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>

using namespace std;

#define nSTRINGWORDs 21
#define MAXNSAMPLES 10000
#define MAXNHITS 1000
#define MAXNSTRINGS 500
#define DEBUG 0
#define DEBUGHIT 2

Long64_t iTrigGlobal = 0;

// run_178_369   178    369    0
// Run number not used for the moment (just passed to the tree)


int process_fragment(TString fragment_name, int run, int subrun, int last_event = 0)
{

  cout << "--------------> " << fragment_name << "   " << run << "    " << subrun << "    " << last_event << "    " << endl;

  // Open input file
  ifstream filein;
  TString infile = "../data/data/" + fragment_name + ".data";
  filein.open(infile.Data(), std::ifstream::in);

  cout << "Opening " << infile.Data() << ".." << endl;

  TString nameroot = "../data/processed/" + fragment_name + "_syncDani_prova.root";
  TFile *fileout = new TFile(nameroot.Data(), "recreate");
  fileout->cd();
  TTree *tree = new TTree("tree", "tree");

  int nevt;
  int nhits;
  int boardID[MAXNHITS];
  int chanID[MAXNHITS];
  int reserved1[MAXNHITS];
  int reserved2[MAXNHITS];
  int errflag[MAXNHITS];
  int time[MAXNHITS];
  int peakpos[MAXNHITS];
  int nofsamples[MAXNHITS];
  int firstsample[MAXNHITS];

  int nsamples;
  int boardid[MAXNSAMPLES];
  int chan[MAXNSAMPLES];
  int ADC[MAXNSAMPLES];
  int timens[MAXNSAMPLES];

  tree->Branch("run", &run, "run/I");
  tree->Branch("subrun", &subrun, "subrun/I");
  tree->Branch("nevt", &nevt, "nevt/I");
  tree->Branch("nhits", &nhits, "nhits/I");
  tree->Branch("boardID", &boardID, "boardID[nhits]/I");
  tree->Branch("chanID", &chanID, "chanID[nhits]/I");
  tree->Branch("reserved1", &reserved1, "reserved1[nhits]/I");
  tree->Branch("reserved2", &reserved2, "reserved2[nhits]/I");
  tree->Branch("errflag", &errflag, "errflag[nhits]/I");
  tree->Branch("time", &time, "time[nhits]/I");
  tree->Branch("peakpos", &peakpos, "peakpos[nhits]/I");
  tree->Branch("nofsamples", &nofsamples, "nofsamples[nhits]/I");
  tree->Branch("firstsample", &firstsample, "firstsample[nhits]/I");
  tree->Branch("nsamples", &nsamples, "nsamples/I");
  tree->Branch("boardid", &boardid, "boardid[nsamples]/I");
  tree->Branch("chan", &chan, "chan[nsamples]/I");
  tree->Branch("ADC", &ADC, "ADC[nsamples]/I");
  tree->Branch("timens", &timens, "timens[nsamples]/I");

  tree->Branch("iTrigGlobal", &iTrigGlobal, "iTrigGlobal/L");

  nevt = 0;
  int line = 0;
  while (!filein.eof())
  {
    if (DEBUG)
      cout << "**************************************" << endl;
    if ((nevt % 100 == 0 || DEBUG )&& 0) //
      cout << "EVENT " << nevt << " STARTS AT LINE " << line + 2 << endl;
    //
    int startFound = 0;
    TString headerString;
    while (startFound == 0 && !filein.eof())
    {
      filein >> headerString;
      line++;
      if (headerString[0] == 69)
        startFound = 1; // ascii "E"
    }
    if (!startFound)
    {
      cout << "cont!!_1 EVENT " << nevt << ": NO EVENT START found" << endl;
      continue;
    }
    headerString.Remove(0, 1); // skip the "E"
    if (headerString.Contains("E"))
      headerString.Remove(0, 1);
    int nstrings = (int)atoi(headerString);
    if (DEBUG)
      cout << "NSTRINGS= " << nstrings << endl;

    int data[MAXNSTRINGS][nSTRINGWORDs];
    int endOfHitFound[MAXNHITS];
    int endOfHitString[MAXNHITS];
    int endOfHitWord[MAXNHITS];
    int startOfHitString[MAXNHITS];

    nhits = 0;
    nsamples = 0;
    TString dataString;
    // initialize first hit
    endOfHitFound[0] = 0;
    endOfHitString[0] = -1;
    endOfHitWord[0] = -1;
    startOfHitString[0] = 0;
    // Loop on data strings
    for (int istring = 0; istring < nstrings; istring++)
    {
      if (DEBUG)
        cout << "STRING " << istring << endl;
      filein >> dataString; // read 1 string
      line++;
      // string unpacking
      // loop on 12 bit words (3 characters)
      for (int iword = 0; iword < nSTRINGWORDs; iword++)
      {
        TString sv(dataString(0 + iword * 3, 3));
        data[istring][iword] = (int)strtol(sv.Data(), NULL, 16);
        if (DEBUG && nhits == DEBUGHIT)
          cout << sv << " " << data[istring][iword] << "  ";

        if (data[istring][iword] == 4095 && endOfHitFound[nhits] == 0 && istring >= startOfHitString[nhits])
        {
          if (DEBUG)
            cout << endl
                 << "End of HIT " << nhits << " found: string " << istring << " word " << iword << endl;
          //
          endOfHitWord[nhits] = iword;
          endOfHitString[nhits] = istring;
          if ((endOfHitWord[nhits] + 4 >= nSTRINGWORDs) &&
              endOfHitString[nhits] == nstrings - 1)
          {
            // we need another string but this is the last!
            if (DEBUG)
              cout << "cont!!_2 MISSING STRING TO COMPLETE HIT " << endl;
            continue;
          }
          endOfHitFound[nhits] = 1;

          nhits++;
          // initialize new hit
          endOfHitFound[nhits] = 0;
          endOfHitString[nhits] = -1;
          endOfHitWord[nhits] = -1;
          if (endOfHitWord[nhits - 1] + 4 < nSTRINGWORDs)
            startOfHitString[nhits] = endOfHitString[nhits - 1] + 1;
          else
          {
            startOfHitString[nhits] = endOfHitString[nhits - 1] + 2;
          }
        }
      }
    }
    if (nhits == 0)
    {
      cout << "cont!!_3 No HITS found..." << endl;
      continue;
    }
    if (endOfHitFound[nhits - 1] == 0)
    {
      cout << "cont!!_4 No end of hit.." << endl;
      continue;
    }
    // Check the end of event character
    TString endString;
    filein >> endString;
    line++;
    //    cout << endString << "   " << endString.CompareTo("N") << "   " << endOfHitFound << endl;
    if (!(endString.CompareTo("N") == 0))
    {
      cout << "cont!!_5 No end of event.." << endl;
      continue;
    }
    // Unpack hits
    bool good_boardid;
    good_boardid = true;
    bool good_errflag;
    good_errflag = true;
    if (DEBUG)
      cout << nhits << " HITS FOUND" << endl;
    for (int ihit = 0; ihit < nhits; ihit++)
    {
      // Unpack hit header
      int first_string = startOfHitString[ihit];
      int last_string = endOfHitString[ihit];
      int last_word = endOfHitWord[ihit];
      reserved1[ihit] = data[first_string][0];
      boardID[ihit] = data[first_string][1];
      // TEMPORARY !!
      // chanID[ihit]    = data[first_string][2]%2;
      chanID[ihit] = data[first_string][2];
      if (boardID[ihit] > 2 || chanID[ihit] > 20)
      {
        cout << "EVENT " << nevt << " BAD board ID (" << boardID[ihit] << ") or CHANNEL ID (" << chanID[ihit] << ")" << endl;
        good_boardid = false;
        break;
      }
      reserved2[ihit] = data[first_string][3];
      // Unpack hit summary
      if (last_word + 1 < nSTRINGWORDs)
        errflag[ihit] = data[last_string][last_word + 1];
      else
        errflag[ihit] = data[last_string + 1][(last_word + 1) % nSTRINGWORDs];

      if (errflag[ihit] != 1365)
      {
        cout << "EVENT " << nevt << " BAD ERROR FLAG (" << errflag[ihit] << ")" << endl;
        good_errflag = false;
        break;
      }
      if (last_word + 2 < nSTRINGWORDs)
        time[ihit] = data[last_string][last_word + 2];
      else
        time[ihit] = data[last_string + 1][(last_word + 2) % nSTRINGWORDs];

      if (last_word + 3 < nSTRINGWORDs)
        peakpos[ihit] = data[last_string][last_word + 3];
      else
        peakpos[ihit] = data[last_string + 1][(last_word + 3) % nSTRINGWORDs];

      if (last_word + 4 < nSTRINGWORDs)
        nofsamples[ihit] = data[last_string][last_word + 4];
      else
        nofsamples[ihit] = data[last_string + 1][(last_word + 4) % nSTRINGWORDs];

      firstsample[ihit] = nsamples;
      if (DEBUG)
        cout << "HIT " << ihit << " from STRING " << first_string << " to " << last_string << " board=" << boardID[ihit] << " chan=" << chanID[ihit] << " time=" << time[ihit] << " peak=" << peakpos[ihit] << " nsamples=" << nofsamples[ihit] << " firstsample=" << firstsample[ihit] << endl;
      for (int istring = first_string; istring <= last_string; istring++)
      {
        int firstWord = (istring == first_string) ? 4 : 0;
        int lastWord = (istring == last_string) ? last_word : nSTRINGWORDs;
        for (int iword = firstWord; iword < lastWord; iword++)
        {
          boardid[nsamples] = boardID[ihit];
          chan[nsamples] = chanID[ihit];

          ADC[nsamples] = data[istring][iword];
          timens[nsamples] = (time[ihit] + nsamples - firstsample[ihit]) * 5;
          if (DEBUG && ihit == DEBUGHIT)
            cout << "HIT " << ihit << " sample " << nsamples << " chan " << chan[nsamples] << " time=" << timens[nsamples] << " ns  ADC=" << ADC[nsamples] << endl;
          nsamples++;
        }
      }
    }

    if (nhits > 0 && good_boardid && good_errflag)
      tree->Fill();

    nevt++;
    iTrigGlobal++; // prova -> ma non deve stare qui !!

    // if (nevt>2400) break; // MISSING LAST STRING
    // if (nevt>3517) break; // BAD ERROR FLAG
    // if (nevt>3965) break;
    // if (nevt>16396) break;
    if (last_event > 0 && nevt > last_event)
      break;
  }

  filein.close();
  fileout->Write();
  fileout->Close();

  return 0;
}

void process_syncDani_prove()
{

  int runNo = 165;
  int fragNo = 1 + 960;
  iTrigGlobal = 0;

  // for (int i = 0; i < fragNo; i++)
  // {

  //   process_fragment(Form("run_%d_%d", runNo, i), runNo, i, 0);
  // }

      int i = 15; //314 incriminato --> forse è questo cont!!_3 No HITS found... ???
      process_fragment(Form("run_%d_%d", runNo, i), runNo, i, 0);

          gErrorIgnoreLevel = kFatal;


      cout<<endl<<endl<<"                       ->>>>     "<<iTrigGlobal<<endl;

}

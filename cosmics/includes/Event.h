#pragma once
using namespace std;

#include "Geom.h"
#include "AnaPars.h"

class Event{
private:

    Geom geo;
    AnaPars pars;

    Geom::TypeCryMatrixDouble defTime{0}, defCharg{0};
    Geom::TypeCryMatrixDouble *timOff = &defTime;
    Geom::TypeCryMatrixDouble *chargEqual = &defCharg;

    int *addCount;

    double qRef;
    double qToE;

public: 

    class Cell{
        public:

        int cry{0}, chan{0}, side{0}, row{0}, col{0}, hit{0}; 
        double x{0}, y{0};
        double ene[Geom::sidNo]{0}, eneRaw[Geom::sidNo]{0}, charg[Geom::sidNo]{0}, chargRaw[Geom::sidNo]{0};
        double tim[Geom::sidNo]{0}, timRaw[Geom::sidNo]{0};
        double fitChi2[Geom::sidNo]{0}, fitPars[Geom::sidNo][3]={0};
        double eneMean{0}, eneRawMean{0}, chargMean{0}, chargRawMean{0}, timMean{0}, timRawMean{0};
    };

    Cell *cell;
    int numHit{0}, numCell{0};

    Event() {}

    double GetEne(double q) {
        return qToE * q;
    }

    void Init(int hits) { 

        numHit = hits; 
        numCell = 0;
        cell = new Cell[hits];
        addCount = new int[hits]{0};
    }

    void SetQRef (double v) {
        qRef = v;
    }

    void SetQtoE (double v) {
        qToE = v;
    }

    void SetTimeOffset(Geom::TypeCryMatrixDouble *inMatrix){
        timOff = inMatrix;
    }

    void SetChargeEqual(Geom::TypeCryMatrixDouble *inMatrix){
        chargEqual = inMatrix;
    }

    Cell *GetCry(int cry) {return 0;} //da fare

    int AddCell(int inCry, int inSide){

        int idx = numCell;
        for(int k = 0; k < numCell; k++) {
            if (cell[k].cry == inCry) {numCell--; idx = k; break;}
        }
        
        numCell++;
        if (addCount[idx] > 1) { return -1; }
        addCount[idx]++;

        cell[idx].cry = inCry;
        cell[idx].chan = geo.chaCry(inCry, inSide);
        cell[idx].side = addCount[idx] == 2 ? 2 : inSide; //max(addCount, inSide)
        cell[idx].row = geo.rowCry[inCry];
        cell[idx].col = geo.colCry[inCry];
        cell[idx].x = geo.cryX[cell[idx].row][cell[idx].col];
        cell[idx].y = geo.cryY[cell[idx].row][cell[idx].col];

        return idx;
    }
  
    int AddHit(int incry, int insid, double incharge, double intime, double inchi2 = 0) {

        int idx = AddCell(incry, insid);
        if (idx < 0) { return idx; }

        int div = (float)addCount[idx]; 

        double charg = qRef * incharge / (*chargEqual)[incry][insid]; 
        double ene = GetEne(charg);
        double timCorr = intime - (*timOff)[incry][insid]; 

        cell[idx].chargRaw[insid] = incharge;
        cell[idx].charg[insid] = charg;
        cell[idx].ene[insid] = ene;
        cell[idx].timRaw[insid] = intime;
        cell[idx].tim[insid] = timCorr;

        cell[idx].chargRawMean += incharge;
        cell[idx].chargMean += charg;
        cell[idx].eneMean += ene;
        cell[idx].timRawMean += intime;
        cell[idx].timMean += timCorr;

        cell[idx].chargRawMean *= 1.0/div;
        cell[idx].chargMean *= 1.0/div;
        cell[idx].eneMean *= 1.0/div;
        cell[idx].timRawMean *= 1.0/div;
        cell[idx].timMean *= 1.0/div;      
        
        return idx;
    }

    ~Event(){}

};


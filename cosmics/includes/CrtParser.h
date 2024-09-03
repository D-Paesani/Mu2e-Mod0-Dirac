#include "anaClass/CRTs4.h"
#include "anaClass/CRTs4.C"
#include "RootIncludes.h"
#include "Geom.h"
using namespace std;

class CrtParser
{
private:

    Geom geo;

    TTree *crtTree;
    TFile *crtFile;
    Long64_t entries;

    TString treeName = "CRT";
    TString treeIndex = "iTrig";

public:

    CRTs4 *crtRead;

    int invertY{0}, invertX{0}, invertZ{0}, useMillimeters{1};
    double offsetX{0}, offsetZ{0}, distanceY{0};
    double thetaYXCrt, thetaYZCrt, thetaYZ, thetaYX, tanThetaYZ, tanThetaYX;
    double yTop, yBtm, xTop, xBtm, zTop, zBtm, y0, x0, z0;
    double xTopCrt, xBtmCrt, zTopCrt, zBtmCrt;
    double qTop0, qTop1, qBtm0, qBtm1;
    int barTop, barBtm;
    Long64_t itrig;
    int trigOffset{0};

    CrtParser() {};
    ~CrtParser() {};

    void CopyConfig(CrtParser& crtConf){
        SetGeometry(crtConf.distanceY, crtConf.offsetX, crtConf.offsetZ, crtConf.invertY, crtConf.invertX, crtConf.invertZ);
        SetOrigin(crtConf.x0, crtConf.y0, crtConf.z0);
    }

    void SetGeometry(double y, double x, double z, int invY = 0, int invX = 0, int invZ = 0){
        distanceY = y; offsetX = x, offsetZ = z;
        invertY = invY; //implementare anche x, z
    }

    void SetOrigin(double _x0, double _y0, double _z0){ x0 = _x0; y0 = _y0; z0 = _z0;}

    void Terminate() {
        crtFile->Close();
    }

    void InitTree(TFile *file, TString fileName = ""){

        if (file == 0 ) {
            crtFile = new TFile(fileName.Data());
        } else {
            crtFile = file;  
        }

        crtFile->GetObject(treeName.Data(), crtTree);   
        entries = (Long64_t)crtTree->GetEntriesFast();
        cout<<endl<<"----> CRT: Initialisation: found "<<entries<< " entries, TChain indexing = "<<treeIndex<<endl;
        cout<<"----> CRT: Geometry ["<<(useMillimeters==1?"mm":"cm")<<"] : distY = "<<distanceY<<" , offX = "<<offsetX<<" , offZ = "<<offsetZ<<" , invY = "<<invertY<<" , invX = "<<invertX<<" , invZ = "<<invertZ<<endl;
        cout<<"----> CRT: Coordinates: (x,y,z) = ( "<<x0<<" , "<<y0<<" , "<<z0<<" ) "<<(useMillimeters==1?"mm":"cm")<<endl<<endl;
        
        crtTree->BuildIndex(treeIndex);
        crtRead = new CRTs4(crtTree);
    }

    int SetEntry(Long64_t trig){

        trig = trig + trigOffset;

        if (crtRead->LoadTree(crtTree->GetEntryNumberWithIndex(trig)) < 0) {return -1;}
        crtTree->GetEntryWithIndex(trig);
        // cout<<crtRead->iTrig<<" == "<<trig<<endl;

        int iTop = invertY!=0;
        int iBtm = invertY!=1;

        barTop = crtRead->iScint[iTop];
        barBtm = crtRead->iScint[iBtm];

        qTop0 = crtRead->Q[0];
        qTop1 = crtRead->Q[1];
        qBtm0 = crtRead->Q[2];
        qBtm1 = crtRead->Q[3];

        zTopCrt = ( 1 - 2 * invertZ) * (9 * useMillimeters + 1) * crtRead->Z[iTop];
        zBtmCrt = ( 1 - 2 * invertZ) * (9 * useMillimeters + 1) * crtRead->Z[iBtm] + offsetZ;
        xTopCrt = ( 1 - 2 * invertX) * (9 * useMillimeters + 1) * crtRead->X[iTop];
        xBtmCrt = ( 1 - 2 * invertX) * (9 * useMillimeters + 1) * crtRead->X[iBtm] + offsetX;

        tanThetaYZ = (xTopCrt - xBtmCrt) / distanceY;
        tanThetaYX = (zTopCrt - zBtmCrt) / distanceY;
        thetaYXCrt = TMath::ATan(tanThetaYZ); 
        thetaYZCrt = TMath::ATan(tanThetaYX);
        thetaYZ = thetaYXCrt;
        thetaYX = thetaYZCrt;
        
        zTop = xTopCrt + z0;
        zBtm = xBtmCrt + z0;
        xTop = zTopCrt + x0;
        xBtm = zBtmCrt + x0;
        yTop = y0;
        yBtm = y0 - distanceY;

        itrig = crtRead->iTrig;

        return 1;
    }

    double GetZCryY(int y) { 
        return (yTop - y)*tanThetaYZ - zTop; //da controllare
    }
    


};


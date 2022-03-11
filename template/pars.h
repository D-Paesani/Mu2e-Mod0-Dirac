#pragma once

using SplineTypeDef = TSpline3;

const int cryNo = 51;
const int sdNo = 2;
const int chNo = 102;
const int waveSz = 200;
const int digiTime = 5;


const struct AnaPars {

const int cryNo = 51;
const int sdNo = 2;
const int chNo = 102;

const double wfEx = 0; //perché ???? 
const double wfEy = 0.12; 

const double CF = 0.10; 
const float maxAmp = 1.9;

const int teTiBins = 1400; 
const double teTiFrom = -30, teTiTo = 250;
const int teAmpBins = 1500; // -0.1 to 1.1

const double teQFrom = 0, teQTo = 100000000000; //mettere qui tagli se servono
const double teOff = 20;

const double tiOff = 260;
const int qBins = 200;
const double qFrom = 0;
const double qTo = 20000;

const int tiBins = 2500;
const double tiFrom = -400, tiTo = 400;

const double teFitStart = 32;
const double teFitStop = 20;


// //delete after this

// const float tminPseudo = 10; //tutto ciò sarà da mettere in fzne di tpeak
// const float tmaxPseudo = 30;
// const float tbaseFrom = 0;
// const float tbaseTo = 12;

// const int doCorrectTrigger = 0;
// const int doLogn = 0; 

} PRM;


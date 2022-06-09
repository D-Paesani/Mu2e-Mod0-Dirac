#pragma once


using SplineTypeDef = TSpline5;



const struct AnaPars {


static const int cryNo = 51;
static const int sdNo = 2;
static const int chNo = 102;
static const int waveSz = 200;
static const int digiTime = 5;

const double wfEx = 0; 
const double wfEy = 0.12; // 1/sqrt(12) included

const double CF_def = 0.08; 
const float maxAmp = 1.9;

const double teTiBins_def = 1400; 
const double teTiFrom = -30, teTiTo = 250;
const int teAmpBins = 2000; // -0.1 to 1.1

const double teQFrom = 2500, teQTo = 12000; 
const double teOff = 20;

const double tiOff = 260;
const int qBins = 200;
const double qFrom = 0;
const double qTo = 20000;

const int tiBins = 2500;
const double tiFrom = -400, tiTo = 400;

const double teFitStop_def = 28;




} PRM;


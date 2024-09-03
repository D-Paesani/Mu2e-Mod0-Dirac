#pragma once

#include "TMath.h"

struct AnaPars {

    static constexpr double pi = TMath::Pi();
    static constexpr double lightSpeed = 299.792458;
    static constexpr float CsI_eta = 2.0;
    static constexpr float CsI_vp = 10*10; 
    static constexpr float MIP_MPV_eRef = 20.0;

    static constexpr double ampSaturationCut = 1000000000;

    static inline TString csvSeparator = "   ";
    static const int csvPrecision = 4;






};


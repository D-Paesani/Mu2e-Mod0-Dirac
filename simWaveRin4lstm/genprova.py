import ROOT
import numpy as np

spline = ROOT.TFile.Open("data/wft.root").Get("/splines/fuzzyTempl_0_2_spline")

splineLim = (0.0, 1023.0)
def splineEval(x): return spline.Eval(x)*(x>splineLim[0] and x<splineLim[1])
def splineF(x,p): return splineEval(x[0])
splineShi = ROOT.TF1("splinef", splineF, splineLim[0], splineLim[1], 0).GetMaximumX(100,400)

nsam = 500
tbase = 1.0
ybase = 0.5
elecnoiserms = 0.5
twave = tbase*np.arange(0,nsam)

def get1wave(): #one event per window
    
    wave = np.random.normal(scale=elecnoiserms, size=nsam)

    amp = ROOT.gRandom.Uniform(100.0, 1000.0)
    t0 = tbase*ROOT.gRandom.Uniform(-5.0,5.0)
    bline = ROOT.gRandom.Uniform(-50.0,50.0)
    tw = amp*np.vectorize(splineEval)(twave + t0 + 0.5*nsam*tbase - splineShi) + bline
    wave += tw

    for _ in range(ROOT.gRandom.Poisson(nsam*8/(100.0*tbase))):
        iamp = np.abs(ROOT.gRandom.Gaus(0.0, 20.0)) #should use bertr
        it0 = ROOT.gRandom.Uniform(0, tbase*nsam) #not actually
        wave += iamp*np.vectorize(splineEval)(twave+it0-splineShi)
        
    wave = ybase*np.floor(wave/ybase)
    tw = ybase*np.floor(tw/ybase)
        
    return wave, (t0, amp), tw
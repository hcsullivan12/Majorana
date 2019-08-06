import ROOT
from iminuit import Minuit
import math

#####################################################################
class RecoHelper:
    #------------------------------
    def __init__(self, ntuplepath, pixels, cfg):
        self._pixels = pixels
        self._cfg    = cfg
        self._input = str(ntuplepath)
        self._nsipms = 0
        self._data = None
        self._disk_radius = 0

    #------------------------------
    def reconstruct(self):
        # open ntuple 
        print 'Reading anatree from', self._input
        f = ROOT.TFile.Open(self._input, 'READ')
        t = f.Get('anatree')
        assert(t)

        for ent in t:
            self.doRecontruct(ent)
            print 'Truth:'
            print '\tX =', ent.sourcePosXYZ[0], '  Y = ', ent.sourcePosXYZ[1]
            if ent.event == 3:
                break

    #------------------------------
    def getGuess(self):
        _max_sipm = 1. * self._data.index(max(self._data))
        _N = 100*sum(self._data)
        _angle = (_max_sipm/self._nsipms) * 2. * math.pi
        _x = (self._disk_radius*0.5) * math.cos(_angle)
        _y = (self._disk_radius*0.5) * math.sin(_angle)
        return _N,_x,_y

    #------------------------------
    def doRecontruct(self, ent):
        # initialize data for this event
        self._data = [x for x in ent.mppcToLY]
        self._errors = [1.**0.5 for x in self._data]
        self._nsipms = ent.nMPPCs
        self._disk_radius = ent.diskRadius

        # minimize! with some help
        _N,_x,_y = self.getGuess()
        _sx, _sy = 2.,2.
        _lim = self._disk_radius
        m = Minuit(self.least_squares, errordef=1, fix_N=True,
                   N=_N,      error_N=0.1*_N,      limit_N=(0,1000000),
                   mu_x=_x,   error_mu_x=0.1*_x,   limit_mu_x=(-1*_lim,_lim),
                   mu_y=_y,   error_mu_y=0.1*_y,   limit_mu_y=(-1*_lim,_lim),
                   sig_x=_sx, error_sig_x=0.1*_sx, limit_sig_x=(0,_lim*0.3),
                   sig_y=_sy, error_sig_y=0.1*_sy, limit_sig_y=(0,_lim*0.3))
        print m.migrad()

    #------------------------------
    def getVisibility(self, d, p):
        return self._pixels[p].lookup_table()[d]

    #------------------------------
    def getKernel(self, d, p, N, mu_x, mu_y, sig_x, sig_y):
        _x = self._pixels[p].x()
        _y = self._pixels[p].y()
        _a = N * (2*math.pi*sig_x*sig_y)**-1  
        _ex = math.exp( (-2*sig_x*sig_x)**-1 * (_x - mu_x)**2 )
        _ey = math.exp( (-2*sig_y*sig_y)**-1 * (_y - mu_y)**2 )
        #if -7 < _x and _x < -5.5 and -7 < _y and _y < -5.5:
            #print _a, _ex, _ey, N, mu_x, mu_y, sig_x, sig_y, _a*_ex*_ey
        return _a * _ex * _ey

    #------------------------------
    def least_squares(self, N, mu_x, mu_y, sig_x, sig_y):
        _sum = 0
        for d in range(1, self._nsipms+1):
            for p in range(1, len(self._pixels)+1):
                kern = self.getKernel(d, p, N, mu_x, mu_y, sig_x, sig_y)
                _sum += ((1./self._errors[d-1])**2) * ( self._data[d-1] - self.getVisibility(d,p)*kern )**2
        return _sum 




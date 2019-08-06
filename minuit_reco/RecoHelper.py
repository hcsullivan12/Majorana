import ROOT
from iminuit import Minuit
import math
import matplotlib.pyplot as plt
import numpy as np

#####################################################################
class RecoHelper:
    #------------------------------
    def __init__(self, ntuplepath, pixels, outputpath, cfg):
        self._pixels = pixels
        self._cfg    = cfg
        self._input = str(ntuplepath)
        self._data = None
        self._disk_radius = 0
        self._output_path = outputpath

    #------------------------------
    def reconstruct(self):
        # open ntuple 
        print 'Reading anatree from', self._input
        print ' '
        f = ROOT.TFile.Open(self._input, 'READ')
        t = f.Get('anatree')
        assert(t)

        for ent in t:
            print '\n\nProcessing event #',ent.event
            self.do_reconstruct(ent)
            print 'Truth:'
            print '\tX =', ent.sourcePosXYZ[0], '  Y = ', ent.sourcePosXYZ[1]

    #------------------------------
    def get_guess(self):
        _total_counts = sum(self._data.values())
        _ls_min = float('inf')
        _ls_pixel = None
        # loop over pixels
        for pid,pixel in self._pixels.iteritems():
            _exp_weight = { sid:pixel.lookup_table()[sid] for sid in self._data.keys() }
            _tot_exp_weight = sum(_exp_weight.values())
            
            # calculate least squared metric
            _ls = 0
            for sid in self._data.keys():
                _exp = _exp_weight[sid]/_tot_exp_weight
                _msd = self._data[sid]/_total_counts
                _diff2 = (_exp-_msd)**2
                _ls += _diff2/_exp
            if _ls < _ls_min:
                _ls_min = _ls
                _ls_pixel = pixel

        # another ls method for N
        _num = sum(_ls_pixel.lookup_table()[sid]*self._data[sid] for sid in self._data.keys())
        _den = sum(_ls_pixel.lookup_table()[sid]**2 for sid in self._data.keys())
        _N = _num/_den

        print 'Guess', _N, _ls_pixel.x(), _ls_pixel.y()
        return _N, _ls_pixel.x(), _ls_pixel.y()

    #------------------------------
    def do_reconstruct(self, ent):
        # initialize data for this event
        self._data = {sid:c for sid,c in enumerate(ent.mppcToLY,1)}
        self._errors = {sid:1.**0.5 for sid,c in enumerate(ent.mppcToLY,1)}
        self._disk_radius = ent.diskRadius

        # minimize! with some help
        _N,_x,_y = self.get_guess()
        _sx, _sy = 2.,2.
        _lim = self._disk_radius
        m = Minuit(self.least_squares, errordef=1, fix_N=True, fix_mu_x=True, fix_mu_y=True,
                   N=_N,      error_N=0.1*_N,      limit_N=(0,1000000),
                   mu_x=_x,   error_mu_x=0.1*_x,   limit_mu_x=(-1*_lim,_lim),
                   mu_y=_y,   error_mu_y=0.1*_y,   limit_mu_y=(-1*_lim,_lim),
                   sig_x=_sx, error_sig_x=0.1*_sx, limit_sig_x=(0,3.),
                   sig_y=_sy, error_sig_y=0.1*_sy, limit_sig_y=(0,3.))
        print m.migrad()

        # draw it!
        self.save_image(ent, m.values)

    #------------------------------
    def get_kernel(self, pixel, N, mu_x, mu_y, sig_x, sig_y):
        _x = pixel.x()
        _y = pixel.y()
        _a = N * (2*math.pi*sig_x*sig_y)**-1  
        _ex = math.exp( (-2*sig_x*sig_x)**-1 * (_x - mu_x)**2 )
        _ey = math.exp( (-2*sig_y*sig_y)**-1 * (_y - mu_y)**2 )
        #if -7 < _x and _x < -5.5 and -7 < _y and _y < -5.5:
            #print _a, _ex, _ey, N, mu_x, mu_y, sig_x, sig_y, _a*_ex*_ey
        return _a * _ex * _ey

    #------------------------------
    def least_squares(self, N, mu_x, mu_y, sig_x, sig_y):
        _sum = 0
        for sid,count in self._data.iteritems():
            for pid,pixel in self._pixels.iteritems():
                kern = self.get_kernel(pixel, N, mu_x, mu_y, sig_x, sig_y)
                _sum += ((1./self._errors[sid])**2) * ( self._data[sid] - pixel.lookup_table()[sid]*kern )**2
        return _sum 

    #------------------------------
    def save_image(self, ent, v):
        spacing = float(self._cfg['pixelSpacing'])/10. # convert to cm
        _s = spacing*0.5
        count = 1
        while _s < self._disk_radius:
            _s += spacing
            count += 2

        _gaus = ROOT.TF2('_gaus', 'bigaus', -1*self._disk_radius, self._disk_radius, -1*self._disk_radius, self._disk_radius)
        _gaus.SetParameters(v['N']/(2*math.pi*v['sig_x']*v['sig_y']), v['mu_x'], v['sig_x'], v['mu_y'], v['sig_y'], 0)
        
        _name = 'event'+str(ent.event)+'_image'
        hist = ROOT.TH2D(_name,_name, count,-1*self._disk_radius, self._disk_radius, count, -1*self._disk_radius, self._disk_radius)
        print _gaus.Eval(v['mu_x'], v['mu_y'])
        for pid,pixel in self._pixels.iteritems():
            xbin = hist.GetXaxis().FindBin(pixel.x())
            ybin = hist.GetXaxis().FindBin(pixel.y())
            z = _gaus.Eval(pixel.x(), pixel.y())
            hist.SetBinContent(xbin, ybin, z)
        f = ROOT.TFile.Open(self._output_path, 'UPDATE')
        hist.Write()
        f.Close()




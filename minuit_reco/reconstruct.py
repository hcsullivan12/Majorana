import argparse
import os
import ROOT
import RecoHelper

#####################################################################
class Pixel:
    #------------------------------
    def __init__(self, pid, x, y):
        self._x  = x
        self._y  = y
        self._id = pid
        self._lookup_table = {}

    #------------------------------
    def x(self):
        return self._x
    
    #------------------------------
    def y(self):
        return self._y
    
    #------------------------------
    def lookup_table(self):
        return self._lookup_table
    
    #------------------------------
    def add(self, sid, p):
        self._lookup_table[sid] = p

#####################################################################
class PixelTable:
    #------------------------------
    def __init__(self, prod_dir, pixel_spacing, nsipms):
        self._prod_dir = prod_dir
        self._pixel_spacing = pixel_spacing
        self._nsipms = nsipms
        self._pixels = {}
        self._pixel_path = ''
        self._op_ref_path = ''
        # load the pixels and lookup table
        self.form_path()
        self.load_pixels()
        self.load_lookup_table()

    #------------------------------
    def form_path(self):
        # get path to pixelization and look up table
        pixelpath = str(self._pixel_spacing+'mm/pixelization.txt')
        oprefpath = str(self._pixel_spacing+'mm/'+self._nsipms+'sipms/splinedOpRefTable.txt')
        self._pixel_path  = os.path.join(str(self._prod_dir), pixelpath)
        self._op_ref_path = os.path.join(str(self._prod_dir), oprefpath)

    #------------------------------
    def load_pixels(self):
        with open(self._pixel_path) as f:
            # skip header: id x y
            line = f.readline()
            while True:
                line = f.readline()
                line_vec = line.split()
                if len(line_vec) < 1: break
                pixel = Pixel(int(line_vec[0]), float(line_vec[1]), float(line_vec[2]))
                self._pixels[int(line_vec[0])] = pixel

    #------------------------------
    def getPixels(self):
        return self._pixels

    #------------------------------
    def load_lookup_table(self):
        with open(self._op_ref_path) as f:
          # skip first line: id sipm prob
          line = f.readline()
          while True:
            line    = f.readline()
            line_vec = line.split()
            if len(line_vec) < 1: break
            self._pixels[int(line_vec[0])].add(int(line_vec[1]), float(line_vec[2]))

#####################################################################
def parse_config(path):
    try:                from ConfigParser import SafeConfigParser
    except ImportError: from configparser import SafeConfigParser
    
    cfg = SafeConfigParser()
    # want case sensitive
    cfg.optionxform = str
    if not os.path.exists(path):
        raise ValueError('No such file: %s' % path)
    cfg.read(path)
    return cfg


#####################################################################
def convert(cfg):
    dat = {}
    for secname in cfg.sections():
        for k,v in cfg.items(secname):
            dat[k] = v
    return dat

#####################################################################
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Reconstruction algorithm for SiPM Wheel")
    parser.add_argument("-c", "--config", help="The configuration file",    required=True)
    parser.add_argument("-o", "--output", help="The name of output ntuple", required=True)
    parser.add_argument("-i", "--input", help="The path to ntuple", required=True)
    args = parser.parse_args()

    # parse config
    cfg = parse_config(args.config)
    assert cfg.sections()
    cfg = convert(cfg)

    # initialize output file
    f = ROOT.TFile.Open(args.output, 'RECREATE')
    f.Close()

    # load pixel table
    pixel_table = PixelTable(cfg['prodDir'], cfg['pixelSpacing'], cfg['nsipms'])

    # start reconstruction
    reco_helper = RecoHelper.RecoHelper(args.input, pixel_table.getPixels(), cfg)
    reco_helper.reconstruct()
    



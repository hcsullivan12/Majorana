import ROOT

rf = ROOT.TFile.Open('output.root', 'READ')
sf = ROOT.TFile.Open('../output/simulateOutput_64.root', 'READ')

sh = sf.Get('primHist;2')
rh = rf.Get('event1_image')

ROOT.gStyle.SetPalette(ROOT.kDarkBodyRadiator)
c1 = ROOT.TCanvas('c1', 'c1', 800, 800)
sh.Draw('colz')
c2 = ROOT.TCanvas('c2', 'c2', 800, 800)
rh.SetMinimum(0)
rh.SetMaximum(sh.GetMaximum())
rh.Draw('colz')

wait = input(' ')
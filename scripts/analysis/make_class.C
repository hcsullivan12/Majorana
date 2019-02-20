void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/02_19_thetaDepWithDiskCorrection.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("thetaDepWithDiskCorrectionAna");
}

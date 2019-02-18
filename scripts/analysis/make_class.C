void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/25_thetaDepNoDisk.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("thetaDepNoDiskAna");
}

void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/thetaDep60DegNoDisk.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("angleNoDiskAna");
}

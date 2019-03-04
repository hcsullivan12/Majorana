void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/output/simulateOutput.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("reco16SAna");
}

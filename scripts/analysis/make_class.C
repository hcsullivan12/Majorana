void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/1000_center.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("Center1000Ana");
}

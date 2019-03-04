void makeClass() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/2618Pixels_v1/03_02_2618Pixels.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("makeOpRefTable2618");
}

void makeClass() {
    TFile *f = new TFile("/home/hunter/projects/03_02_2618Pixels.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("makeOpRefTable2618_1");
}

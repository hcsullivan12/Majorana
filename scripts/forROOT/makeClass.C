void makeClass() {
    TFile *f = new TFile("02_23_663Voxels.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("temp");
}

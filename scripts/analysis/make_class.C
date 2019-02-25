void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/02_23_663Voxels.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("makeOpRefTable");
}

void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/voxelization1deg30.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("angler_RAna");
}

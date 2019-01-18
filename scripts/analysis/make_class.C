void make_class() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/voxelization1mmXaxis.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("xAxisVoxelsAna");
}

void makeClass() {
    TFile *f = new TFile("/home/hunter/projects/Majorana/production/665Pixels_v2/02_25_663Voxels.root");
    TTree *t = (TTree*)f->Get("anatree");
    t->MakeClass("makeOpRefTable2");
}

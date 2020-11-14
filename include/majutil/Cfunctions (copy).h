//
// Created by ilker on 3/27/20.
//

#ifndef ANA_CFUNCTIONS_H
#define ANA_CFUNCTIONS_H
#include <cmath>
#include <stdio.h>
#include <TTree.h>
#include <TFile.h>
#include "TH2.h"
#include "TF2.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TPaveLabel.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include "TTreeHelp.h"

using namespace std;

namespace majutil{

class Cfunctions {
    public:
        string Title;
        TFile * f;
        string FileName;
        string TrueFilePath;
        string FilePath;
        Double_t    R;
        Double_t dif = 10;
        Double_t TrueX;
        Double_t TrueY;
        Double_t ExpX;
        Double_t ExpY;
        TH1I *Measured;
        TH2F *chi2;
        TH2F *mli;
        int EventID;
        bool CDraw=true;
        bool SaveImg=true;
        bool SaveFile=false;


    struct RecoTTreeHelp{

        vector<double>AnaExpY;
        vector<double>AnaExpX;
        vector<double>AnaErrX;
        vector<double>AnaErrY;

        vector<double>ActExpY;
        vector<double>ActExpX;
        vector<double>ActErrX;
        vector<double>ActErrY;

        vector<double>FiltExpY;
        vector<double>FiltExpX;
        vector<double>FiltErrX;
        vector<double>FiltErrY;

        vector<double>NPairExpY;
        vector<double>NPairExpX;
        vector<double>NPairErrX;
        vector<double>NPairErrY;
    };



    void CreateTree(RecoTTreeHelp *htr)
    {
        TTree* t = (TTree*)f->Get("reco");
        if (!t)
        {
            t = new TTree("reco", "reco");
            t->Branch("Histos",&htr);
        }
        else
            t->SetBranchAddress("Histos",&htr);


        t->Fill();
        t->Write(t->GetName(), TObject::kWriteDelete);
        t->Reset();
    }


    void DrawCircle(){
            TGraph *TruePos = new TGraph(1);
            TGraph *ExpPos = new TGraph(1);
            TGraph *gr;
            TMultiGraph *mg= new TMultiGraph();


            if(!f) return;

            Int_t n = 360;
            Double_t x, y;
            string sdev;

            Double_t pR = R + dif;
            Double_t nR = -R - dif;
            vector<string> Error;
            string ErrX;
            string ErrY;

            Error=ErroEst();
            ErrX="ErrX= " + (Error[0]) ;
            ErrY="ErrY= " + (Error[1])  ;

            TCanvas *c1 = new TCanvas(Title.c_str(),Title.c_str(), 800, 800);
            c1->SetGrid();

            //Draw The circle
            gr = new TGraph(n);
            gr->SetMarkerColor(kBlack);
            gr->SetMarkerStyle(21);
            gr->SetLineWidth(1);
            gr->SetMarkerSize(1);


            TruePos->SetMarkerColor(kGreen);
            TruePos->SetMarkerStyle(20);
            TruePos->SetMarkerSize(2);

            ExpPos->SetMarkerColor(kRed);
            ExpPos->SetMarkerStyle(20);
            ExpPos->SetMarkerSize(2);


            for (Int_t i = 0; i < n; i++) {
                x = R * cos(i);
                y = R * sin(i);
                gr->SetPoint(i, x, y);
            }

            TruePos->SetPoint(0,TrueX,TrueY);
            ExpPos->SetPoint(0,ExpX, ExpY);



            string TrueLabel;
            string ExpLabel;
            TrueLabel = "TrueX= " + to_string(TrueX) + " , " + "TrueY= " + to_string(TrueY);
            ExpLabel = "ExpX= " + to_string(ExpX) + " , " + "ExpY= " + to_string(ExpY);

            TLegend *leg;

            leg = new TLegend(0.1,0.75,0.9,0.9);
            leg->SetHeader("Results");
            leg->AddEntry(TruePos, TrueLabel.c_str(), "lep");
            leg->AddEntry(ExpPos, ExpLabel.c_str(), "lep");
            leg->AddEntry("ErrX", ErrX.c_str(), "l");
            leg->AddEntry("ErrY", ErrY.c_str(), "l");
            leg->SetEntrySeparation(0.2);
            leg->SetTextSize(0.03);

            mg->Add(gr);
            mg->Add(TruePos);
            mg->Add(ExpPos);

            string MTitle=Title+"_TrueVsExp";
            c1->DrawFrame(nR, nR, pR, pR)->SetTitle(MTitle.c_str());
            mg->Draw("p");
            leg->Draw();

            // Just to Draw Some Plots not all
            if((abs(stof(Error[0]))<150 && abs(stof(Error[1]))<150) || CDraw)
            {
                this->CombinedTCanvas(c1);

            }
    }
    vector<string> ErroEst()
    {
        Double_t ErrX;
        Double_t ErrY;
        ReadTrueFile();
        vector<string> Result;
        if(TrueX==0){ TrueX+=1;ExpX+=1; }
        if(TrueY==0){ TrueY+=1;ExpY+=1; }


        ErrX=abs((ExpX-TrueX)/TrueX)*100;
        ErrY=abs((ExpY-TrueY)/TrueY)*100;

        Result.push_back(to_string(round(ErrX * 100) / 100.0));
        Result.push_back(to_string(round(ErrY * 100) / 100.0));
        return Result;

    }

    void ReadTrueFile()
    {
        string line;
        ifstream myfile (TrueFilePath);
        vector<string> splitLine;
        int count(0);
        if (myfile.is_open())
        {
            getline (myfile,line);
            while ( getline (myfile,line) )
            {
                boost::split(splitLine,line,boost::is_any_of(" "));
                if(count==EventID){
                    TrueX=stof(splitLine[0]);
                    TrueY=stof(splitLine[1]);
                    myfile.close();
                    break;
                }
                count++;

            }
            if (count==0)
            {
                cout<< "could not find the true position!";
                return;
            }
            myfile.close();

        } else {cout << "Unable to open file True Position File"; return;};
    }

    void CombinedTCanvas(TCanvas *c2)
    {
            string Name=Title+"Combined";
        auto *mc1=new TCanvas(Name.c_str(),Name.c_str(),150,10,800,800);

        mc1->SetTitle(Name.c_str());
        mc1->Divide(2,2,0.01,0.01);
        mc1->SetTopMargin(100);
        mc1->cd(1);
        gPad->SetTickx(2);
        gPad->SetTicky(2);
        chi2->Draw("colz");
        mc1->cd(2);
        gPad->SetTickx(2);
        gPad->SetTicky(2);
        mli->Draw("colz");
        mc1->cd(3);
        gPad->SetTickx(2);
        gPad->SetTicky(2);
        Measured->GetYaxis()->SetTitleOffset(1.55);
        Measured->SetLineColor(2);
        Measured->SetLineWidth(4);
        Measured->Draw();
        mc1->cd(4);
        gPad->SetTickx(2);
        gPad->SetTicky(2);
        c2->DrawClonePad();
        gPad->Update();
        mc1->cd();

        auto *newpad=new TPad("newpad","a transparent pad",1,1,0,0);
        newpad->SetFillStyle(4000);
        newpad->Draw();
        newpad->cd();
        string n=FileName;
        /*auto *title = new TPaveLabel(0.1,0.96,0.9,1,n.c_str());
        title->SetFillColor(16);
        title->SetTextFont(42);
        title->Draw();
         */
        TPaveLabel *title = new TPaveLabel(.4,.95,.60,.99,n.c_str(),"brndc");
        title->Draw();
        gPad->Update();



        /*if(SaveImg && Title.find("Fill")!=string::npos){
            string ImageName=FilePath+FileName+Name+".jpg";
            mc1->Print(ImageName.c_str());
        }*/
        string ImageName=FilePath+FileName+Name+".jpg";
        mc1->Print(ImageName.c_str());
        delete gROOT->FindObject("hframe");
        //if(SaveFile)
            //mc1->Write();

    }
    // DarkCounts
    void GetDarkFiles(string FilePath,vector<string>&FileNames)
    {
        string line;
        cout<<"ThePath  : " << FilePath <<endl;
        ifstream myfile (FilePath);
        unsigned int count(0);
        cout<< "Getting the file Names"<<endl;
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                FileNames.push_back(line);
                count++;
            }
            if (count==0)
            {
                cout<< "could not find any files!";
                return;
            } else cout << "Total of " << count << " are obtained" << endl;
            myfile.close();

        } else {cout << "Unable to open file "; return;};
    }

    //---- Function is used for averaging the backgrounds ---
    void DarkCountAveraging(string Path,string NewFileName,string TxtFileList){
        struct TTreeHelp ahtr;
        struct TTreeHelp aAvr;
        struct TTreeHelp *AverAnaData;
        struct TTreeHelp *htr;

        htr=&ahtr;
        AverAnaData=&aAvr;

        string NewAnaPath=Path + NewFileName;
        string Files=Path + TxtFileList; // File name and path that contains the dark counts
        string DarkPath;
        AverAnaData->eventID=0;
        Int_t NSIPMs=64; // Number of SIPMs

        vector<string> FileNames;

        GetDarkFiles(Files,FileNames); // Gets the file names
        for (auto Name : FileNames){

            DarkPath= Path + Name;
            cout << "Openning the file " << Name<<endl;
            auto fDark = TFile::Open(DarkPath.c_str(),"READ");

            if (!fDark) return;
            TTree * dt;
            fDark->GetObject("ana",dt);

            if(!dt) return;
            dt->SetBranchAddress("TTreeHelp",&htr);
            Long64_t nentries = dt->GetEntries();
            if (dt->GetEntries()==0) // Checks if there is any event
            {
                cout<<"Entries are 0"<<endl;
                return;
            }


            for (Long64_t jentry=0; jentry<nentries; jentry++) {
                dt->GetEvent(jentry);
                cout<<"Working on Event " << jentry<<endl;
                AverAnaData->eventID+=jentry;

                for (unsigned int i = 0; i < NSIPMs; i++) {
                    if(AverAnaData->DiffTime.size()<NSIPMs)
                    {
                        AverAnaData->ActualCounts.insert(i+AverAnaData->ActualCounts.begin(), (htr->ActualCounts.at(i)));
                        AverAnaData->FilteredCounts.insert(i+AverAnaData->FilteredCounts.begin(),(htr->FilteredCounts.at(i)));
                        AverAnaData->AnaCounts.insert(i+AverAnaData->AnaCounts.begin(), (htr->AnaCounts.at(i)));
                        AverAnaData->NPairCounts.insert(i+AverAnaData->NPairCounts.begin(),(htr->NPairCounts.at(i)));
                        AverAnaData->DiffTime.insert(i+AverAnaData->DiffTime.begin(),(htr->DiffTime.at(i)));
                    }else{
                        AverAnaData->ActualCounts.at(i)+=htr->ActualCounts.at(i);
                        AverAnaData->FilteredCounts.at(i)+=htr->FilteredCounts.at(i);
                        AverAnaData->AnaCounts.at(i)+=htr->AnaCounts.at(i);
                        AverAnaData->NPairCounts.at(i)+=htr->NPairCounts.at(i);
                        AverAnaData->DiffTime.at(i)+=htr->DiffTime.at(i);
                    }


                } // Loop over each SIPM


            } // Loop over each events
            fDark->Close();
            cout<<"Done -->>  " + Name << endl;
        } // Loop over each File

        unsigned int NofFiles= FileNames.size();
        for (unsigned i = 0; i < NSIPMs; i++) {

            AverAnaData->ActualCounts.at(i)=AverAnaData->ActualCounts.at(i)/NofFiles;
            AverAnaData->FilteredCounts.at(i)=AverAnaData->FilteredCounts.at(i)/NofFiles;
            AverAnaData->AnaCounts.at(i)=AverAnaData->AnaCounts.at(i)/NofFiles;
            AverAnaData->NPairCounts.at(i)=AverAnaData->NPairCounts.at(i)/NofFiles;
            AverAnaData->DiffTime.at(i)=AverAnaData->DiffTime.at(i)/NofFiles;
        }
        auto f  = TFile::Open(NewAnaPath.c_str(),"RECREATE"); // Open the new file


        TTree* Avrt = (TTree*)f->Get("ana");
        if (!Avrt)
        {
            Avrt = new TTree("ana", "ana");
            Avrt->Branch("TTreeHelp",&AverAnaData);
        }
        else
            Avrt->SetBranchAddress("TTreeHelp", &AverAnaData);


        Avrt->Fill();
        Avrt->Write(Avrt->GetName(), TObject::kWriteDelete);
        Avrt->Reset();
        f->Close();
        cout<<"Writing to file is Done"<<endl;


    }


};
}

#endif //ANA_CFUNCTIONS_H

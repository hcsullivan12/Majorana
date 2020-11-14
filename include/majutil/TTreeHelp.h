struct TTreeHelp{
    Int_t eventID;
    //Int_t nsipms;
    std::vector<unsigned>ActualCounts;
    std::vector<unsigned>AnaCounts;
    std::vector<unsigned>FilteredCounts;
    std::vector<unsigned>DiffTime;
    std::vector<unsigned> NPairCounts;
    /*vector<unsigned>DeadSIPM;
    vector<int> ts0sub;
    vector<UInt_t>Board1ts0;
    vector<UInt_t>Board2ts0;
     */
};
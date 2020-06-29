void SplitTree(string filename, Long64_t maxentries=10000 ) {
// Run like
// $ root -q 'SplitTree.C("user.rnewhous.21710573._000001.tree.root", 1000)'

   //Get old file, old tree and set top branch address
   TFile *oldfile = new TFile(TString(filename));

   TTree *oldtree = (TTree*)oldfile->Get("outTree");
   Long64_t nentries = oldtree->GetEntries();
   Int_t block = 0;


    cout << "Splitting file: " << filename << endl;
    cout << "Maximum number of events per file: " << maxentries << endl;
    cout << "Total events: " << nentries << endl;

    // loop over all entries
    int i = 0;
    while (i<nentries) {

        cout << "Processing block " << block << " starting with event " << i << endl;
        TFile *newfile = new TFile(TString(filename+"_split"+to_string(block)),"recreate");
        TTree *newtree = oldtree->CloneTree(0);

        // make a new tree of size maxentries
        for (Long64_t j=0;j<maxentries; j++) {
            if (i >= nentries)
                break;
            // get the event from the old tree
            oldtree->GetEntry(i);
            // and put it in the new tree
            newtree->Fill();
            // increment event
            i++;
        }
        
        // increment the block to start a new file
        block++;
        // save the new tree
        newtree->AutoSave();
        // when you're done, delete the new file to not leak memory
        delete newfile;
    }
    // when you're done, delete the old file to not leak memory
   delete oldfile;
}

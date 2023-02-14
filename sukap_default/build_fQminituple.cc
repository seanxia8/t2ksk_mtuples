#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

#include "fQMiniTupleInterface.h"


int main( int argc , char * argv[] )
{
  if( argc < 3 )
  {
     std::cout << "Usage: " << argv[0] << " [t2k mc reg. expression] [channel] [fluxfile] [banff weight] [niwg weight] [n50-dt cut flag] |outfile.name| " << std::endl;
     return -1;
  }

  std::string regex   = argv[1];
  int  OscChannel     = atoi( argv[2] );
  std::string fluxf = argv[3];
  //std::string rhcfluxf = argv[4];
  std::string wghex   = argv[4];
  std::string niwgw   = argv[5];
  int  use_n50cut = atoi(argv[6]);

  TChain * h1 = new TChain("h1");
  int nFiles = h1->Add( regex.c_str() );

  //NIWG Weight
  TChain * niwgtree = new TChain("weightstree");
  int nFilesn = niwgtree->Add(niwgw.c_str() );
  if (nFilesn==nFiles) {
    //h1->AddFriend(niwgtree, "niwg");
  } else {
    niwgtree = 0;
  }

  //BANFF Weight
  TChain * weightstree = new TChain("weightstree");
  int nFilesw = weightstree->Add(wghex.c_str() );
  if (nFilesw==nFiles) {
    //h1->AddFriend(weightstree , "banff");
    //if (h1->GetListOfBranches()->FindObject("fWeight"))
    //std::cout<<"BANFF "<<wghex<<std::endl;
    //if (weightstree->GetListOfBranches()->FindObject("fWeight"))
    //std::cout<<"BANFF "<<std::endl;
  } else {
    weightstree = 0;
  }

  std::string outname = "tmp.root";

  if( argc > 7 ) 
     outname = argv[7] ;

 
  std::cout << "Processing " << nFiles << " files. " << std::endl;

  fQMiniTupleInterface * mtp = new fQMiniTupleInterface( h1 , weightstree, niwgtree, fluxf, outname, use_n50cut );
  mtp->SetOscChannel( OscChannel );

  for( unsigned i = 0 ; i < h1->GetEntries() ; i++ )
  {
     h1->GetEntry(i);
     if (weightstree) weightstree->GetEntry(i);
     if (niwgtree) niwgtree->GetEntry(i);
     // Determine event type for this entry 
     mtp->PerformSelection();

     // Record will be stored only if event is defined as either 
     // single-ring e-like, sinle-ring mu-like, or two-ring pi0 
     mtp->Fill(); 

  }
  mtp->Finish();

  std::cout << "Finished processing " << regex << " , " << nFiles << " files. " << std::endl;

  return 0;
}


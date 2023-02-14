#include "fQMiniTupleInterface.h"
#include <cmath>
#include <sstream>

//fQMiniTupleInterface::fQMiniTupleInterface( TTree * h , TTree * hban, TTree * hniwg, const std::string fluxfhc , const std::string fluxrhc, const std::string name )
fQMiniTupleInterface::fQMiniTupleInterface( TTree * h , TTree * hban, TTree * hniwg, const std::string flux , const std::string name, const bool use_n50_cut )
{

  parenttree = h ;
  //  wgtbantree = 0;
  //  wgtniwgtree = 0;
  wgtbantree = hban;
  wgtniwgtree = hniwg;
  hFluxRatio = 0;
  fluxfile = new TFile( flux.c_str() );
  //hfhcFluxRatio = 0;
  //hrhcFluxRatio = 0;
  //fhcfluxfile = new TFile( fluxfhc.c_str() );
  //rhcfluxfile = new TFile( fluxrhc.c_str() );

  SF_RFG_weight = new TArrayF(3);

  EnableCopyBranches( parenttree ); 
  LoadBranchAddresses( );

  //  if (wgtbantree) parenttree->AddFriend(wgtbantree);
  //  if (wgtniwgtree) parenttree->AddFriend(wgtniwgtree);
  
  outfile = new TFile( name.c_str() , "recreate" );
  childtree = (TTree*) parenttree->CloneTree(0);
  childtree->SetName("minituple");

  DefineExtraBranches( childtree );

  propagator = new BargerPropagator();

  OscChannel = -1 ;

  UseN50Cut = use_n50_cut;
}

fQMiniTupleInterface::~fQMiniTupleInterface( )
{
  
  delete propagator;

}

void fQMiniTupleInterface::SetOscChannel( int x )
{
   OscChannel = -1 ;
   if( x < EndOfChannels && x >= 0 )
      OscChannel = (int) x ; 
   else 
   {
     std::cout << "fQMiniTupleInterface::SetOscChannel Osc Channel " << x << " is ill defined." << std::endl;
     exit(-1);
   }

   std::cout << "OscChannel is " << OscChannel << std::endl;
   
   SetFluxTuning( );
}


void fQMiniTupleInterface::SetFluxTuning()
{
   
  std::string types[] = { "numu", "numub", "nue", "nueb", "numu" , "numub" };
 
  stringstream ss;
  if ( OscChannel > 0 ) {
    ss << "enu_sk_tuned13a_" << types[OscChannel-1] << "_ratio" ;
    hFluxRatio = (TH1D*) fluxfile->Get( ss.str().c_str() );
    if (!hFluxRatio) {
      std::cout << "fQMiniTupleInterface::SetFluxTuning flux tuning histogram not found for channel " << OscChannel <<std::endl
		<< " Please check the input files and channel specification. " << std::endl;	
    } else {
      std::cout << "fQMiniTupleInterface::SetFluxTuning reweighting to " << ss.str() <<
	" flux" << std::endl;
    }
  }
}

void fQMiniTupleInterface::LoadBranchAddresses()
{
 
  parenttree->SetBranchAddress( "ipnu"        ,   ipnu          );
  parenttree->SetBranchAddress( "pnu"         ,   pnu           );
  parenttree->SetBranchAddress( "mode"        ,  &mode          );

  parenttree->SetBranchAddress( "npar"        ,  &npar          );
  parenttree->SetBranchAddress( "ipv"         ,  &ipv           );
  parenttree->SetBranchAddress( "pmomv"       ,  &pmomv         );

  parenttree->SetBranchAddress( "evclass"     ,  &evclass       );
  parenttree->SetBranchAddress( "nhitac"      ,  &nhitac        );
  parenttree->SetBranchAddress( "fq1rnll"     ,   fq1rnll       );
  parenttree->SetBranchAddress( "fqpi0nll"    ,   fqpi0nll      );
  parenttree->SetBranchAddress( "fqpi0mass"   ,   fqpi0mass     );
  parenttree->SetBranchAddress( "fq1rmom"     ,   fq1rmom       );
  parenttree->SetBranchAddress( "fq1rdir"     ,   fq1rdir       );
  parenttree->SetBranchAddress( "fq1rpos"     ,   fq1rpos       );
  parenttree->SetBranchAddress( "fq1rt0"      ,   fq1rt0        );
  parenttree->SetBranchAddress( "fq1reloss"   ,   fq1reloss     );  
  parenttree->SetBranchAddress( "fqnse"       ,  &fqnse         );
  parenttree->SetBranchAddress( "fqn50"       ,   fqn50         );
  parenttree->SetBranchAddress( "fqpi0pos"    ,   fqpi0pos      );
  parenttree->SetBranchAddress( "fqpi0dirtot" ,   fqpi0dirtot   );  

  parenttree->SetBranchAddress( "fqnmrfit"    ,  &fqnmrfit      );
  parenttree->SetBranchAddress( "fqmrifit"    ,   fqmrifit      );
  parenttree->SetBranchAddress( "fqmrnll"     ,   fqmrnll       );
  parenttree->SetBranchAddress( "fqmrnring"   ,   fqmrnring     );
  parenttree->SetBranchAddress( "fqmrpid"     ,   fqmrpid       );
  parenttree->SetBranchAddress( "fqmrmom"     ,   fqmrmom       );
  parenttree->SetBranchAddress( "fqmreloss"   ,   fqmreloss     );
  parenttree->SetBranchAddress( "fqmrdir"     ,   fqmrdir       );
  parenttree->SetBranchAddress( "fqmrpos"     ,   fqmrpos       );
  parenttree->SetBranchAddress( "fqmrt0"      ,   fqmrt0        );  

  //if (parenttree->GetListOfBranches()->FindObject("good_spill_flag"))
  parenttree->SetBranchAddress( "good_spill_flag" ,  &good_spill_flag );

  //BANFF Weight
  if (wgtbantree)
    if (wgtbantree->GetListOfBranches()->FindObject("fWeight"))
      wgtbantree->SetBranchAddress("fWeight",&fWeight);
   
  //NIWG Weight
  if (wgtniwgtree)
    if (wgtniwgtree->GetListOfBranches()->FindObject("weights"))
      wgtniwgtree->SetBranchAddress("weights", &SF_RFG_weight);
  
}

void fQMiniTupleInterface::InitVars()
{

   iclass   = -1  ;
   cosbm    = -2. ;
   erec     = -1. ;
   wgtosc   =  1. ;
   wgtflx   =  1. ;
   pizmass  =  0. ; 
   pizmom   =  0. ;
   pizcosb  = -2. ;

   horn_current = 0. ;

   //NIWG Weights
   wgtfrfg  = 1.;
   wgtrpar  = 1.;
   wgtcccoh = 1.;
   wgtban = 1.;
   wgtncgamma = 1.;
   wgtnubarmec = 1.;
}


void fQMiniTupleInterface::PerformSelection()
{

   InitVars();
   ComputeCommonVars();

   
   if ( ! FC                () ) return ;
   
   if ( UseN50Cut )     ComputefQNdcye(0);
   
   if ( SingleRingELike     () ) return ;
   if ( SingleRingMuLike    () ) return ;
   if ( SingleRingELikeDcy1 () ) return ;
   if ( MultiRingMuLikeDcy1 () ) return ;
   if ( MultiRingMuLikeDcy2 () ) return ;   

}

bool fQMiniTupleInterface::FC() 
{
 
   bool kPass = false ;
   if ( nhitac < 16 && evclass == 1 && fqevis>30 ) {
     kPass = true ;
   }
   return kPass;
}

// FC cut should be applied prior to calling this function
bool fQMiniTupleInterface::SingleRingELike()
{
   bool kPass = false ;
   if( fqnring == 1 && elike ) { // single-ring e-like
     //if( fqnmue==0 && fqevis > 100.0 ) {// no decay electrons
     if( fqnmue==0 && fqevis > 75.0 ) {// loose cut for momentum shift in OA
       if( fq1rnll[0][1] - fqpi0nll[0]  < 175. - 0.875 * fqpi0mass[0] ) { // pi0 rejection
  	 fqwall   = ComputeWall( fq1rpos[0][1][0], fq1rpos[0][1][1], fq1rpos[0][1][2] );
	 fqtowall = ComputeTowall( fq1rpos[0][1][0], fq1rpos[0][1][1], fq1rpos[0][1][2], fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2] );
	 if ( fqwall > 80 && fqtowall > 170 ) {
	   kPass = true ;
	 }
       }
     }
   }

   if ( kPass )
   { 
      iclass    = fQSingleRingELike ;  // single-ring e-like = 1 
      cosbm     = ComputeCosBeam( fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2] );
      erec      = ComputeErec( (double) fq1rmom[0][1] , cosbm , 1 ) ;  // assuming electron  
      wgtosc    = ComputeOscWeight( );
      wgtflx    = ComputeFluxWeight( );
      wgtban    = ComputeBFFWeight( );
      wgtfrfg   = ComputeNIWGWeight_frfg( );
      wgtrpar   = ComputeNIWGWeight_RPAr( );
      if (mode==-2) {
	wgtnubarmec = GetExtraWeights();
      } else if (abs(mode)==16) {
	wgtcccoh  = GetExtraWeights( );
      } else if (abs(mode)==38 || abs(mode)==39) {
	wgtncgamma = GetExtraWeights();
      }

      if (erec >= 1400.0) // loose cut for OA, erec < 1400 MeV
	{
	  kPass = false;
	  iclass = -1;
	}            
   }

   return kPass;
}


// FC cut should be applied prior to calling this function
bool fQMiniTupleInterface::SingleRingMuLike()
{
   bool kPass = false ;
  
   if( fqnring == 1 && (!elike) && (fqnmue == 1 || fqnmue == 0) ) {// single-ring mu-like 
     //if( fq1rmom[0][2] > 200. ) {// momentum threshold
     if (fq1rmom[0][2] > 170.0) {//loose cut for OA
       if ( fq1rnll[0][2] - fq1rnll[0][3] < 0.15 * fq1rmom[0][2] ) {
	 fqwall   = ComputeWall( fq1rpos[0][2][0], fq1rpos[0][2][1], fq1rpos[0][2][2] );
	 fqtowall = ComputeTowall( fq1rpos[0][2][0], fq1rpos[0][2][1], fq1rpos[0][2][2], fq1rdir[0][2][0], fq1rdir[0][2][1], fq1rdir[0][2][2] );
	 if ( fqwall>50 && fqtowall>250 ) {
	   kPass = true ;
	 }
       }
     }
   }

   if ( kPass )
   { 
      iclass  = fQSingleRingMuLike ;  // Single-ring mulike
      cosm    = ComputeCosBeam( fq1rdir[0][2][0], fq1rdir[0][2][1], fq1rdir[0][2][2] ); 
      erec    = ComputeErec( (double) fq1rmom[0][2] , cosm , 0 ) ;  // assuming muon 
      wgtosc  = ComputeOscWeight( );
      wgtflx  = ComputeFluxWeight( );
      wgtban    = ComputeBFFWeight( );
      wgtfrfg   = ComputeNIWGWeight_frfg( );
      wgtrpar   = ComputeNIWGWeight_RPAr( );
      if (mode==-2) {
	wgtnubarmec = GetExtraWeights();
      } else if (abs(mode)==16) {
	wgtcccoh  = GetExtraWeights( );
      } else if (abs(mode)==38 || abs(mode)==39) {
	wgtncgamma = GetExtraWeights();
      }
   }

   return kPass;
}


// FC cut should be applied prior to calling this function
bool fQMiniTupleInterface::TwoRingPi0Like()
{
   bool kPass = false ;
  
   if( fqnring == 2 && fqmrpid[0][0]==1 && fqmrpid[0][1]==1 ) // two e-like rings
     if( fqnmue == 0 ) // no decay electrons
       kPass = true ;

   if ( kPass )
   { 
      iclass     = fQTwoRingPi0Like ;  // Two Ring pi0like  = 2
      pizmass    = ComputePi0Mass();
      wgtosc     = ComputeOscWeight( );
      wgtflx     = ComputeFluxWeight( );
      pizmom     = ComputePi0Mom ( );
      pizcosb    = ComputePi0CosBeam ( );
      fqwall   = ComputeWall( fqpi0pos[0][0], fqpi0pos[0][1], fqpi0pos[0][2] );
      fqtowall = ComputeTowall( fqpi0pos[0][0], fqpi0pos[0][1], fqpi0pos[0][2], fqpi0dirtot[0][0], fqpi0dirtot[0][1], fqpi0dirtot[0][2] );
   }

   return kPass;
}

// FC cut should be applied prior to calling this function
bool fQMiniTupleInterface::SingleRingELikeDcy1()
{
   bool kPass = false ;
   if( fqnring == 1 && elike ) {  // single-ring e-like 
     //if( fqnmue == 1 && fqevis > 100.0 ) {// one decay electrons
     if( fqnmue == 1 && fqevis > 75.0 ) {// loose cut for momentum shift syst in OA
       if( fq1rnll[0][1] - fqpi0nll[0]  < 175. - 0.875 * fqpi0mass[0] ) {// pi0 rejection
	 fqwall   = ComputeWall( fq1rpos[0][1][0], fq1rpos[0][1][1], fq1rpos[0][1][2] );
	 fqtowall = ComputeTowall( fq1rpos[0][1][0], fq1rpos[0][1][1], fq1rpos[0][1][2], fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2] );
	 if (fqwall>50 && fqtowall>270) {
           kPass = true ;
	 }
       }
     }
   }    

   if ( kPass )
   {
     iclass    = fQSingleRingELike1Decay ;
 
     cosbm     = ComputeCosBeam( fq1rdir[0][1][0], fq1rdir[0][1][1], fq1rdir[0][1][2] );
     erec      = ComputeErecCCDel( (double) fq1rmom[0][1] , cosbm , 1 ) ;  // assuming electron  
     wgtosc    = ComputeOscWeight( );
     wgtflx    = ComputeFluxWeight( );
     wgtban    = ComputeBFFWeight( );
     wgtfrfg   = ComputeNIWGWeight_frfg( );
     wgtrpar   = ComputeNIWGWeight_RPAr( );
     if (mode==-2) {
	wgtnubarmec = GetExtraWeights();
      } else if (abs(mode)==16) {
	wgtcccoh  = GetExtraWeights( );
      } else if (abs(mode)==38 || abs(mode)==39) {
	wgtncgamma = GetExtraWeights();
      }
     if (erec >= 1400.)
       {
	 kPass = false;
	 iclass = -1;
       }
   }

   return kPass;
}

bool fQMiniTupleInterface::MultiRingMuLikeDcy1()
{
  bool kPass = false;
  if( fqnring > 1 && fqnmue == 1 ) {  // multi-ring one decay electrons
      fqwall   = ComputeWallMRmuLike();
      if (fqwall>200) { // most energetic ring wall > 200 cm
	if (nllr_pipi_e() > -400. + 1.2*min_mom_pipi() && nllr_2Rother_pipi() < 340.){ //e-pi PID
	  if (sum_eloss_pipi() > 300.){// NC background
	    kPass = true ;	  
	  }
	}
      }
  }        
  
  if ( kPass )
   {
     iclass    = fQMultiRingMuLike1Decay ;
 
     cosbm     = ComputeCosBeamMRmuLike();
     erec      = ComputeErecCCDelMRmuLike() ;  // assuming electron  
     wgtosc    = ComputeOscWeight( );
     wgtflx    = ComputeFluxWeight( );
     wgtban    = ComputeBFFWeight( );
     wgtfrfg   = ComputeNIWGWeight_frfg( );
     wgtrpar   = ComputeNIWGWeight_RPAr( );
     if (mode==-2) {
	wgtnubarmec = GetExtraWeights();
      } else if (abs(mode)==16) {
	wgtcccoh  = GetExtraWeights( );
      } else if (abs(mode)==38 || abs(mode)==39) {
	wgtncgamma = GetExtraWeights();
      }
   }
  
  return kPass;  
}

bool fQMiniTupleInterface::MultiRingMuLikeDcy2()
{
  bool kPass = false;
  if( fqnmue == 2 ) {  // multi-ring two decay electrons
      fqwall   = ComputeWallMRmuLike();
      if (fqwall>200) { // most energetic ring wall > 200 cm
	if (nllr_pipi_e() > -400. + 1.0*min_mom_pipi() && nllr_2Rother_pipi() < 310.){ //e-pi PID
	  kPass = true ;	  
	}
      }
  }        
  
  if ( kPass )
   {
     iclass    = fQMultiRingMuLike2Decay ;
 
     cosbm     = ComputeCosBeamMRmuLike();
     erec      = ComputeErecCCDelMRmuLike() ;  // assuming electron  
     wgtosc    = ComputeOscWeight( );
     wgtflx    = ComputeFluxWeight( );
     wgtban    = ComputeBFFWeight( );
     wgtfrfg   = ComputeNIWGWeight_frfg( );
     wgtrpar   = ComputeNIWGWeight_RPAr( );
     if (mode==-2) {
	wgtnubarmec = GetExtraWeights();
      } else if (abs(mode)==16) {
	wgtcccoh  = GetExtraWeights( );
      } else if (abs(mode)==38 || abs(mode)==39) {
	wgtncgamma = GetExtraWeights();
      }
   }
  
  return kPass;  
  
}

void fQMiniTupleInterface::EnableCopyBranches( TTree * h  )
{

  h->SetBranchStatus( "*" , 0 );
  for( unsigned i = 0 ; i < nExportBranches; i++ )
     h->SetBranchStatus( ExportBranches[i].c_str() , 1 );

}

void fQMiniTupleInterface::DefineExtraBranches( TTree * h )
{
   
  h->Branch( "iclass"       , &iclass       , "iclass/I"         );

  h->Branch( "cosbm"        , &cosbm        , "cosbm/D"          );
  h->Branch( "erec"         , &erec         , "erec/D"           );
  h->Branch( "pizmass"      , &pizmass      , "pizmass/D"        );
  h->Branch( "pizmom"       , &pizmom       , "pizmom/D"         );
  h->Branch( "pizcosb"      , &pizcosb      , "pizcosb/D"        );
  h->Branch( "cosm"         , &cosm         , "cosm/D"           );

  h->Branch( "wgtosc"       , &wgtosc       , "wgtosc/D"         );
  h->Branch( "wgtflx"       , &wgtflx       , "wgtflx/D"         );
  h->Branch( "OscChannel"   , &OscChannel   , "OscChannel/I"     );

  h->Branch( "wgtban"       , &wgtban       , "wgtban/D"         );
  h->Branch( "wgtsfrfg"     , &wgtfrfg      , "wgtsfrfg/D"       );
  h->Branch( "wgtRPA_r"     , &wgtrpar      , "wgtRPA_r/D"       );
  h->Branch( "wght_CCcoh"   , &wgtcccoh     , "wght_CCcoh/D"     );
  h->Branch( "wght_NubMEC"  , &wgtnubarmec  , "wght_NubMEC/D"    );
  h->Branch( "wght_NCgm"    , &wgtncgamma   , "wght_NCgm/D"      );
  h->Branch( "fqwall"       , &fqwall       , "fqwall/F"         );
  h->Branch( "fqtowall"     , &fqtowall     , "fqtowall/F"       );
  h->Branch( "fqnmue"       , &fqnmue       , "fqnmue/I"         );
  h->Branch( "fqnring"      , &fqnring      , "fqnring/I"        );
  h->Branch( "fqevis"       , &fqevis       , "fqevis/F"         );
  h->Branch( "fqmome"       , &fqmome       , "fqmome/F"         );
  h->Branch( "fqmomm"       , &fqmomm       , "fqmomm/F"         );
  
  h->Branch( "horn_current" , &horn_current , "horn_current/D"   );
  h->Branch( "fqremoved"    , fqremoved     , "fqremoved[10]/I"  );
  h->Branch( "fqtscnd"      , fqtscnd       , "fqtscnd[10]/F"    );

}

double fQMiniTupleInterface::ComputeCosBeam( float dirx, float diry, float dirz  )
{

   double cosb = (double) dirx*beamdir[0] +
                          diry*beamdir[1] +
                          dirz*beamdir[2] ;
   return cosb;
}

double fQMiniTupleInterface::ComputeCosBeamMRmuLike()
{
  int ipp = -1;
  
  for (int ifit=0; ifit<fqnmrfit; ++ifit) {
      if (std::abs(fqmrifit[ifit]) == 20000033) {
        ipp = ifit;
        break;
      }
  }
  if (ipp < 0) {
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun multi-ring ifit result.");
  }
  
  // Take highest energy particle as the muon
  int muIndex = fqmrmom[ipp][0]<fqmrmom[ipp][1] ? 1 : 0;
  
    return ComputeCosBeam(fqmrdir[ipp][muIndex][0],
                          fqmrdir[ipp][muIndex][1],
                          fqmrdir[ipp][muIndex][2]);
    
}

double fQMiniTupleInterface::ComputeErec( double LeptonP , double cosBeam, int flag )
{
   static const double Vnuc  = 27.0        ; // MeV 
   static const double mn    = 939.565346  ; // MeV
   static const double mp    = 938.272013  ; // MeV


   static const double me    = 0.510998   ; // MeV
   static const double mm    = 105.65836  ; // MeV

   double mass ;
   if( flag == 1 ) mass = me ;
   else            mass = mm ;

   double E  = 0.;
   double LeptonE = sqrt( mass*mass + LeptonP*LeptonP );


   E  = ( mn - Vnuc)*LeptonE - mass*mass/2. ;
   E +=   mn*Vnuc  - Vnuc*Vnuc/2.;
   E += ( mp*mp - mn*mn)/2.;

   E /= ( mn - Vnuc - LeptonE + LeptonP*cosBeam );

   // returns value in MeV 
   return E;

}

double fQMiniTupleInterface::ComputePi0Mass()
{

   double cosTheta = (double) 
          fqmrdir[0][0][0] * fqmrdir[0][1][0] 
        + fqmrdir[0][0][1] * fqmrdir[0][1][1] 
        + fqmrdir[0][0][2] * fqmrdir[0][1][2]; 

   double invmass = sqrt( 2.*fqmrmom[0][0]*fqmrmom[0][1]*(1. - cosTheta) );

   return invmass;

}

double fQMiniTupleInterface::ComputePi0Mom()
{

   double cosTheta = (double)
          fqmrdir[0][0][0] * fqmrdir[0][1][0] 
        + fqmrdir[0][0][1] * fqmrdir[0][1][1] 
        + fqmrdir[0][0][2] * fqmrdir[0][1][2]; 

    return sqrt(      fqmrmom[0][0]*fqmrmom[0][0]
                 +    fqmrmom[0][1]*fqmrmom[0][1]
                 + 2.*fqmrmom[0][0]*fqmrmom[0][1] * cosTheta );
}


double fQMiniTupleInterface::ComputePi0CosBeam()
{

   double Ppi0 = ComputePi0Mom();

   double pi0x = fqmrmom[0][0]*fqmrdir[0][0][0] + fqmrmom[0][1]*fqmrdir[0][1][0];
   double pi0y = fqmrmom[0][0]*fqmrdir[0][0][1] + fqmrmom[0][1]*fqmrdir[0][1][1];
   double pi0z = fqmrmom[0][0]*fqmrdir[0][0][2] + fqmrmom[0][1]*fqmrdir[0][1][2];

   pi0x /= Ppi0 ;
   pi0y /= Ppi0 ;
   pi0z /= Ppi0 ;

   return  pi0x * beamdir[0] +  pi0y * beamdir[1] +  pi0z * beamdir[2]; 

}


void fQMiniTupleInterface::Fill()
{

   // failed event selection 
   if( iclass < 0 ) return ;

   // otherwise store the record
   childtree->Fill();
}

double fQMiniTupleInterface::ComputeOscWeight()
{

  // no neutral current signal 
  if ( abs(mode) > 30 && OscChannel == fNueSignal    ) return 0.0; 
  if ( abs(mode) > 30 && OscChannel == fNueBarSignal ) return 0.0; 

  // neutral current events and data get no weight
  if ( abs(mode) > 30           ) return 1.0 ;
  if ( OscChannel == fData      ) return 1.0 ;

  propagator->SetMNS( s12, s13, s23, m12, m23, cpd, (double) pnu[0], kSinQVars , ipnu[0] ) ;
  propagator->propagateLinear( ipnu[0] , baseline , eden);

  if ( OscChannel == fNumu         ) return propagator->GetProb( 2, 2 ) ;  // mu -> mu 
  if ( OscChannel == fNumuBar      ) return propagator->GetProb( -2, -2 ) ;  // mu -> mu 
  if ( OscChannel == fNue          ) return propagator->GetProb( 1, 1 ) ;  // e  -> e 
  if ( OscChannel == fNueBar       ) return propagator->GetProb( -1, -1 ) ;  // e  -> e 
  if ( OscChannel == fNueSignal    ) return propagator->GetProb( 2, 1 ) ;  // mu -> e 
  if ( OscChannel == fNueBarSignal ) return propagator->GetProb( -2, -1 ) ;  // mubar -> ebar 

  if ( OscChannel < 0 )
     std::cout << "Please set oscillation channel via fQMiniTupleInterface::SetOscChannel " << std::endl; 
  
}

double fQMiniTupleInterface::ComputeFluxWeight( const bool fhc )
{
  if ( ! hFluxRatio ) return 1.0 ;
  int bin = hFluxRatio->GetXaxis()->FindBin( pnu[0] );
  return hFluxRatio->GetBinContent( bin );
}

void fQMiniTupleInterface::Finish()
{
   outfile->cd();
   childtree->Write(0 , TObject::kWriteDelete );
   outfile->Close();
}

double fQMiniTupleInterface::ComputeBFFWeight()
{
    if ( OscChannel == fData      ) return 1.0 ;
    //std::cout<<"fWeight: "<<fWeight<<std::endl;
    return fWeight;
}

double fQMiniTupleInterface::ComputeNIWGWeight_frfg()
{
    if ( OscChannel == fData      ) return 1.0 ;
    return SF_RFG_weight->At(0);
}

double fQMiniTupleInterface::ComputeNIWGWeight_RPAr()
{
    if ( OscChannel == fData      ) return 1.0 ;
    //return SF_RFG_weight->At(2); // Currently only for neutrino mode in official directory
    return 0.;
}

double fQMiniTupleInterface::GetExtraWeights()
{
  if ( OscChannel == fData      ) return 1.0 ;

  int mode_ = TMath::Abs( mode );
  double Epi = 0.; //MeV

  double weight = 1.;
  if(mode_ == 38 || mode_ == 39){ weight *= 2; } // NCgamma prefit weight

  if(mode_ == 16){ // CCCoh prefit weight

    int j=0;
    int find = 0;
    while(j<npar && find==0){
      if(ipv[j] == 8 || ipv[j] == 9){
        Epi = (pmomv[j]*pmomv[j])/(2.*139.57);
        find++;
      }
      j++;
    }
    if(Epi<250.){ weight *= 0.135; }
    else if(Epi<500.){ weight *= 0.4; }
    else if(Epi<750.){ weight *= 0.294; }
    else if(Epi<1000.){ weight *= 1.206; }
  }


  if(mode == -2){ weight *= 0.578037; } //postfit error for MEC nubar not found in T2KREWEIGHT yet

  return weight;
}

double fQMiniTupleInterface::ComputeErecCCDel( double LeptonP , double cosBeam, int flag )
{
   static const double mn    = 939.565346  ; // MeV
   static const double mp    = 938.272013  ; // MeV
   static const double md    = 1232.0  ; // MeV


   static const double me    = 0.510998   ; // MeV
   static const double mm    = 105.65836  ; // MeV

   double mass ;
   if( flag == 1 ) mass = me ;
   else            mass = mm ;

   double E  = 0.;
   double LeptonE = sqrt( mass*mass + LeptonP*LeptonP );


   E  = mp *LeptonE - mass*mass/2. ;
   E += ( md*md - mp*mp)/2.;
   E /= ( mp - LeptonE + LeptonP*cosBeam );

   // returns value in MeV 
   return E;
}

double fQMiniTupleInterface::ComputeErecCCDelMRmuLike()
{
  int ipp = -1;
  
  for (int ifit=0; ifit<fqnmrfit; ++ifit) {
    if (std::abs(fqmrifit[ifit]) == 20000033) {
      ipp = ifit;
      break;
    }
  }
  if (ipp < 0) {
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun multi-ring ifit result.");
  }
  
  // Take highest energy particle as the muon
  int muIndex = fqmrmom[ipp][0]<fqmrmom[ipp][1] ? 1 : 0;
  
  return ComputeErecCCDel(double(fqmrmom[ipp][muIndex]), ComputeCosBeamMRmuLike(), 2);//assuming muon mass
};

void fQMiniTupleInterface::ComputeCommonVars()
{
   if( good_spill_flag ==  1 )  horn_current =  250.0; 
   if( good_spill_flag ==  2 )  horn_current =  200.0; 
   if( good_spill_flag == -1 )  horn_current = -250.0; 
   
   fqnring = fqmrnring[0];
   fqnmue = fqnse-1;
   //fqevis = fq1rmom[0][1];
   fqevis = GetMREvis(0);//changed to be compatible with MR samples - J.Xia, Jan 2023
   fqmome = fq1rmom[0][1];
   fqmomm = fq1rmom[0][2];
   elike = (fq1rnll[0][2]-fq1rnll[0][1]>0.2*fq1rmom[0][1]);

   for (int i = 0; i < fqnse; i++){
     fqremoved[i] = 0;
     fqtscnd[i] = 0.;
   }
} 


float fQMiniTupleInterface::ComputeWall(float x, float y, float z)
{
  float Rmax = 1690.;
  float Zmax = 1810.;
  float rr   = sqrt(x*x + y*y);
  float absz = TMath::Abs(z);
  //check if vertex is outside tank
  float signflg = 1.;
  if (absz>Zmax) signflg = -1.;
  if (rr>Rmax)   signflg = -1.;
  //find min distance to wall
  float distz = TMath::Abs(Zmax-absz);
  float distr = TMath::Abs(Rmax-rr);
  float dwall = signflg*fmin(distz,distr);
  return dwall;
}

float fQMiniTupleInterface::ComputeTowall(float x, float y, float z, float dx, float dy, float dz)
{
  Double_t const R(1690);
  Double_t l_b(100000.0), H;
  Double_t l_t(100000.0);
  Double_t A, B, C, RAD;
  if(dx!=0 || dy!=0){
    A = (dx*dx+dy*dy);
    B = 2*(x*dx+y*dy);
    C = (x*x+y*y-R*R);
    RAD = (B*B) - (4*A*C);
    l_b = ((-1*B) + sqrt(RAD))/(2*A);
  }
  if (dz==0){return l_b;}
  else if(dz>0){H=1810;}
  else if(dz<0){H=-1810;}
  l_t=(H - z)/dz;
  return  (l_t > l_b ? l_b:l_t);
}

float fQMiniTupleInterface::ComputeWallMRmuLike(){
    //    std::cout << "ComputeWall Starts" <<std::endl;
    float x, y, z;

    float maxMom = 0.;
    
    for (int i_ring = 0; i_ring < fqmrnring[0]; i_ring++){
      if (fqmreloss[0][i_ring] >= maxMom){
	maxMom = fqmreloss[0][i_ring];
	x = fqmrpos[0][i_ring][0];
	y = fqmrpos[0][i_ring][1];
	z = fqmrpos[0][i_ring][2];	
      }
    }
    float ret = ComputeWall(x, y, z);
    return ret;
}


bool fQMiniTupleInterface::ComputefQNdcye(int MRFitIndex)
{
  int nseaftercut = 0; // counter
  float lt_low = 1.5; // anything < 1.5 us will be accepted
  float lt_high = 20.0; // anything >= 20 us will be rejected
  float slope_n50 = 0.25;
  float intercept_lt = -7.5; // triangular part
  float maxMom = 0.;
  int maxring = 0;

  if (fqmrnring[MRFitIndex] < 1){
    return false;    
  }
  
  if (fqmrnring[MRFitIndex] == 1){
    for (int ie = 1; ie<fqnse; ie++)
      {	
	float lifetime_e = (fq1rt0[ie][1] - fq1rt0[0][2]) * 0.001;
	fqtscnd[ie] = lifetime_e;
	if(lifetime_e < lt_low)
	  {
	    nseaftercut++;
	  }
	else if((lifetime_e < lt_high) && (lifetime_e < fqn50[ie] * slope_n50 + intercept_lt))
	  {
	    nseaftercut++;
	  }
	else
	  {
	    fqremoved[ie] = 1;
	  }
      }
  }
  else {
    for (int i_ring = 0; i_ring < fqmrnring[MRFitIndex]; i_ring++){
      if (fqmreloss[MRFitIndex][i_ring] >= maxMom){
	maxMom = fqmreloss[MRFitIndex][i_ring];
	maxring = i_ring;
      }
    }
    for (int ie = 1; ie<fqnse; ie++)
      {	
	float lifetime_e = (fq1rt0[ie][1] - fqmrt0[MRFitIndex][maxring]) * 0.001;
	fqtscnd[ie] = lifetime_e;
	if(lifetime_e < lt_low)
	  {
	    nseaftercut++;
	  }
	else if((lifetime_e < lt_high) && (lifetime_e < fqn50[ie] * slope_n50 + intercept_lt))
	  {
	    nseaftercut++;
	  }
	else
	  {
	    fqremoved[ie] = 1;
	  }
      }          
    
  }
  fqnmue = nseaftercut;
  
  return true;
}

float fQMiniTupleInterface::GetElossMax(float mass, float mom)
{
  double pCherenkovThr = (double)mass/0.882925;
  return std::max( 0., std::sqrt( double(mom*mom + mass*mass) ) - std::sqrt( std::pow( pCherenkovThr, 2.) + double(mass*mass) ) );
}

float fQMiniTupleInterface::GetRingEvis(int MRFitIndex, int iring)
{
  if (iring<0 || iring>=fqmrnring[MRFitIndex]) return 0.;  
  int iPID = fqmrpid[MRFitIndex][iring];
  if (iPID==1) {//electron ring
    return fqmrmom[MRFitIndex][iring];
  } else if (iPID==2) {//muon ring
    return GetElossMax(105.65836, fqmrmom[MRFitIndex][iring]);
  } else {//upstream track
    return fqmreloss[MRFitIndex][iring];
  }
}

float fQMiniTupleInterface::GetMREvis(int MRFitIndex){
  float evistot = 0;
  if (fqmrnring[MRFitIndex]==1) {
      if (fq1rnll[0][2]-fq1rnll[0][1]>0.2*fq1rmom[0][1]) {
        evistot = fq1rmom[0][1];      
      } else if (fq1rnll[0][2]-fq1rnll[0][3]<0.15*fq1rmom[0][2]) {
        evistot = GetElossMax(105.65836, fq1rmom[0][2]);
      } else {
        evistot = fq1reloss[0][3];
      }
  } else {
    for (int iring=0; iring<fqmrnring[MRFitIndex]; ++iring) {
      evistot += GetRingEvis(0, iring);
    }
  }
  return evistot;
}

float fQMiniTupleInterface::nllr_pipi_e()
{
    int ipp = -1;
    int ie = -1;
    for (int ifit=0; ifit<fqnmrfit; ++ifit) {
      if (std::abs(fqmrifit[ifit]) == 20000033) {
        ipp = ifit;
      } else if (std::abs(fqmrifit[ifit]) == 10000001) {
        ie = ifit;
      }
      if ((ipp >= 0) and (ie >= 0)) break;
    }
    
    if ((ipp < 0) or (ie < 0)) {
      throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun multi-ring ifit result");
    }
    return fqmrnll[ie] - fqmrnll[ipp];
}

float fQMiniTupleInterface::min_mom_pipi()
{
  int ipp = -1;
  
  for (int ifit=0; ifit<fqnmrfit; ++ifit) {
    if (std::abs(fqmrifit[ifit]) == 20000033) {
      ipp = ifit;
      break;
    }
  }
  if (ipp < 0) {
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun multi-ring ifit result.");
  }
  return fqmrmom[ipp][(fqmrmom[ipp][0]<fqmrmom[ipp][1] ? 0 : 1)];
}

float fQMiniTupleInterface::nllr_2Rother_pipi()
{
  int ipp = -1;
  int i2Rbest = -1;
  float nllBest = 1000000.;
  
  for (int ifit=0; ifit<fqnmrfit; ++ifit) {
    if (std::abs(fqmrifit[ifit]) == 20000033) {
      ipp = ifit;
    } else if (std::abs(std::abs(fqmrifit[ifit]) - (2*10000000)) < 4 * std::abs(std::pow(10, 1))) { // We want the best 2-ring fit which is not pipi
      if (fqmrnll[ifit] < nllBest){
	nllBest = fqmrnll[ifit];
	i2Rbest = ifit;
      }
    }
  }
  
  if (ipp < 0) {
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun pipi multi-ring ifit result.");
  }
  if (i2Rbest < 0){
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find best non-pipi FiTQun multi-ring ifit result.");
  }
  
  return fqmrnll[ipp] - fqmrnll[i2Rbest];
}

float fQMiniTupleInterface::sum_eloss_pipi()
{
  int ipp = -1;
  
  for (int ifit=0; ifit<fqnmrfit; ++ifit) {
    if (std::abs(fqmrifit[ifit]) == 20000033) {
      ipp = ifit;
      break;
    }
  }
  if (ipp < 0) {
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun multi-ring ifit result");
  }
  return fqmreloss[ipp][0] + fqmreloss[ipp][1];
}

float fQMiniTupleInterface::rc_par()
{
  int i_2R_best = -1;
  float nll_2R_best = 1000000;
  
  for (int ifit=0; ifit<fqnmrfit; ++ifit) {
    
    // Two ring
    if (std::abs(std::abs(fqmrifit[ifit]) - (2*10000000)) < 4 * std::abs(std::pow(10, 1))){
      if (fqmrnll[ifit] < nll_2R_best){
	i_2R_best = ifit;
	nll_2R_best = fqmrnll[ifit];
      }
    }
  }
  
  //    if (i_1R_best < 0 or i_2R_best < 0) {
  if (i_2R_best < 0) {
    throw std::runtime_error("t2ksk_eventSelections: ERROR! Couldn't find FiTQun multi-ring ifit or nll result. I need to run on files with all multi-ring results written out.");
  }
  
  float mommin2R = std::min(fqmrmom[i_2R_best][0], fqmrmom[i_2R_best][1]);
  float cthreshold = std::max(150. -0.6*mommin2R, 0.);
  float deltalnL = std::min(fq1rnll[0][1], fq1rnll[0][2]) - fqmrnll[i_2R_best];
  float ringctpar = 50*std::sqrt(std::abs(deltalnL - cthreshold));
  if (ringctpar > 1000.) ringctpar = 999.;
  if (deltalnL - cthreshold < 0.)  ringctpar *= -1.;
  
  return ringctpar;
}


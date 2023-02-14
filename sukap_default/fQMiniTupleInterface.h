#ifndef __fQMiniTupleInterface__
#define __fQMiniTupleInterface__

#include <iostream>
#include <string>

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TArray.h"

#include "globalVars.h"
#include "BargerPropagator.h"


class fQMiniTupleInterface
{

 public:
  
  //fQMiniTupleInterface( TTree * h , TTree * hban, TTree * hniwg, const std::string fhcfluxf , const std::string rhcfluxf, const std::string name = "tmp.root" );
  fQMiniTupleInterface( TTree * h , TTree * hban, TTree * hniwg, const std::string fluxf , const std::string name = "tmp.root", const bool use_n50_cut = 0 );
  ~fQMiniTupleInterface();

   virtual void PerformSelection();
   virtual void Fill();
   virtual void Finish();

   double ComputeErec      ( double , double , int );
   double ComputeErecCCDel ( double , double , int );
   double ComputeErecCCDelMRmuLike                ();

   double ComputeCosBeam   ( float, float, float );
   double ComputeCosBeamMRmuLike( );
   double ComputePi0Mass   ( );
   double ComputeOscWeight ( );
   double ComputeFluxWeight( const bool fhc = true );
   double ComputePi0Mom    ( );
   double ComputePi0CosBeam( );
   float  GetElossMax      ( float, float );
   float  GetRingEvis      ( int, int );
   float  GetMREvis        ( int );
   float  nllr_pipi_e      ( );
   float  min_mom_pipi     ( );
   float  nllr_2Rother_pipi( );
   float  sum_eloss_pipi   ( );
   float  rc_par           ( );

   void   SetOscChannel ( int );
   void   SetFluxTuning (     );

   bool ComputefQNdcye( int );
   
 protected: // functions
   virtual void InitVars();

   // Branches from the parent tree that we want to copy 
   // into the Child
   virtual void EnableCopyBranches  ( TTree * ); 
 
   // Branches which don't exist in the parent tree but 
   // which we will compute and add to the Child
   // (Erec, fluxweight, eventclass, etc...)
   virtual void DefineExtraBranches ( TTree * );

  
  // requires parenttree to be a valid pointer
   virtual void LoadBranchAddresses ( );
   
   // Fully Contained fiducial volume selection
   virtual bool  FC            ();

   // standard T2K Event Selections (called from PerformSelection)
   virtual bool  SingleRingELike ();
   virtual bool  SingleRingMuLike();
   virtual bool  TwoRingPi0Like  ();
   virtual bool  SingleRingELikeDcy1 ();
   virtual bool  MultiRingMuLikeDcy1 ();
   virtual bool  MultiRingMuLikeDcy2 ();  

   virtual void  ComputeCommonVars();
   
   float ComputeWall(float x, float y, float z);
   float ComputeTowall(float x, float y, float z, float dx, float dy, float dz);
   float ComputeWallMRmuLike(); 

 protected: //variables
   TTree  * parenttree;
   TTree  * wgtbantree;
   TTree  * wgtniwgtree;

   // Computing Oscillation Probabilites
   BargerPropagator * propagator ;

   int             OscChannel;

   bool            UseN50Cut;

   // input variables from parent tree 
   Int_t           ipnu [10];   
   Float_t         pnu  [10];   
   //   Float_t         amome[10];   
   //   Float_t         amomm[10];   
   //   Float_t         dir  [10][3];

   Float_t         dirx, diry, dirz;
   Float_t         posx, posy, posz;
   
   Int_t           npar;
   Float_t         pmomv[50];
   unsigned int    ipv[50];
   Int_t           mode;

   UInt_t          nhitac;
   UInt_t          ip[10];  
   Int_t           evclass;

   //   Float_t         msdir[10][3][6];   

   Float_t         fq1rpos[10][7][3];
   Float_t         fq1rdir[10][7][3];
   Int_t           fqnse;

   Int_t           fqn50[10]; 
   Float_t         fqpi0nll[2];
   Float_t         fqpi0mass[2];
   Float_t         fq1rnll[10][7];   //[fqnse]
   Float_t         fq1rmom[10][7];
   Float_t         fq1rt0[10][7];
   Float_t         fq1reloss[10][7];
   Float_t         fqpi0pos[2][3];
   Float_t         fqpi0dirtot[2][3];
   
   Float_t         fqwall;
   Float_t         fqtowall;
   Int_t           fqnmue;
   Int_t           fqnring;
   Float_t         fqevis;
   Float_t         fqmome;
   Float_t         fqmomm;
   Int_t           fqremoved[10];//flag for rejected fq dcye candidate by the n50-dt cut
   Float_t         fqtscnd[10];//the re-calculated timing in us for secondary particles

   Int_t           fqmrifit[200];
   Int_t           fqnmrfit;
   Float_t         fqmrnll[200];
   Int_t           fqmrpid[200][6];
   Float_t         fqmrdir[200][6][3];
   Float_t         fqmrpos[200][6][3];
   Float_t         fqmrmom[200][6];
   Float_t         fqmreloss[200][6];
   Int_t           fqmrnring[200];
   Float_t         fqmrt0[200][6];
  
   Int_t           good_spill_flag;

   //BANFF Weights
   double ComputeBFFWeight ( );

   //NIWG Weights
   double ComputeNIWGWeight_frfg( );
   double ComputeNIWGWeight_RPAr( );

   //ADDITIONAL WEIGHTS
   double GetExtraWeights();
   
   Float_t         fWeight; //BANFF Weight

 protected:  // output variables

   //TFile * fhcfluxfile ;
   //TFile * rhcfluxfile ;
   //TH1D  * hfhcFluxRatio;
   //TH1D  * hrhcFluxRatio;   

   TFile * fluxfile;
   TH1D * hFluxRatio;
   
   TFile * outfile ;
   TTree * childtree;

   bool     elike;
   bool     notPi0;
   bool     notPip;
   int      iclass;

   double   erec    ;
   double   pizmass ;
   double   pizmom  ;
   double   pizcosb ;
   double   cosbm   ;
   double   cosm    ;

   double   wgtosc;
   double   wgtflx;
   double   wgtflxrhc;
   double   wgtban;

   double   wgtfrfg;
   double   wgtrpar;
   double   wgtcccoh; // cc coherent prefit weight
   double   wgtncgamma; // NCgamma prefit weight
   double   wgtnubarmec; // MEC postfit weight

   double   horn_current ;

   TArrayF *SF_RFG_weight;


};

#endif

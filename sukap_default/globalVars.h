#ifndef globalVars_h
#define globalVars_h

#include <string>
  
enum EventCategories{ fSingleRingELike = 1 , fTwoRingPi0Like = 2,  fSingleRingMuLike = 3,  fSingleRingELike1Decay = 4};
enum fQEventCategories{	fQSingleRingELike = 11 , fQTwoRingPi0Like = 12,  fQSingleRingMuLike = 13, fQSingleRingELike1Decay = 14, fQMultiRingMuLike1Decay = 31, fQMultiRingMuLike2Decay = 32 };
enum NumuCC1piEventCategories{ fSingleRingMuLike2Decay = 15, fTwoRingMuLike1Decay = 16, fTwoRingMuLike2Decay = 17, fThreeRingMuLike1Decay = 18, fThreeRingMuLike2Decay = 19};

enum Channels{ fData , fNumu , fNumuBar, fNue , fNueBar, fNueSignal , fNueBarSignal , EndOfChannels };


  // Parameters for a default oscillation weight
/*
static const double s12  = 0.8704; 
  static const double s13  = 0.1 ;
  static const double s23  = 1.0 ;
  static const double m12  = 7.6E-5;
  static const double m23  = 2.4E-3; 
  static const double cpd  = 0.0 ;
  static const double eden = 2.6 ;
*/
static const double s12  = 0.304;
static const double s13  = 0.0217 ;
static const double s23  = 0.528 ;
static const double m12  = 7.53E-5;
static const double m23  = 2.509E-3;
static const double cpd  = -1.601 ;
static const double eden = 2.6 ;

  // if above mixing angle parameters are of the form 
  //  sin^2( 2x ) kSinQVars should be false 
  //  sin^2(  x ) kSinQVars should be true 
  static const bool   kSinQVars = true ;
 
  static const double baseline = 295. ;

  static const double beamdir[3] = { 0.669764, -0.742179, 0.024223 };

  // Fields to fill in the output ntuples 
  static const int nExportBranches = 69;//71 ;  // 63
  static const std::string ExportBranches [] = { 
    "dir",
    "amome",
    "pi0mass",
    "npar",
    "wallv",
    "ipv",
    "posv",
    "pos",
    "dirv",
    "pmomv",
    "npar2",
    "ipv2",
    "numnu",
    "mode",
    "ipnu",
    "pnu",
    "dirnu",
    "nscndprt",
    "itrkscnd",
    "vtxscnd",
    "pscnd",
    "iprtscnd",
    "tscnd",
    "iprntprt",
    "lmecscnd",
    "iprnttrk",
    "iorgprt",
    "pprnt",
    "iflgscnd",
    "Npvc",
    "Ipvc",
    "Ichvc",
    "Iorgvc",
    "Iflvc",
    "Abspvc",
    "Pvc",
    "Crsx",
    "Crsy",
    "Crsz",
    "Crsphi",
    "Nvert",
    "Posvert",
    "Iflgvert",
    "Nvcvert",
    "Dirvert",
    "Abspvert",
    "Abstpvert",
    "Ipvert",
    "Iverti",
    "Ivertf",
    "Fsiprob",
    "t2kposc",
    "Numbndn",
    "Numbndp",
    "Numfrep",
    "Numatom",
    "Ibound" ,
    "amom"   ,
    "nev"    ,
    "nrun"   ,
    "pi0_dir",
    "pi0_e"  ,
    "pi0like",
    "nring" ,
    "wall",
    "evis",
    "ip",
    "amomm",
    "nmue"
   };

#endif

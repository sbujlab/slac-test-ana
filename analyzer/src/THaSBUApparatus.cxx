//*-- Author :    Ole Hansen   2-Oct-01

//////////////////////////////////////////////////////////////////////////
//
// THaSBUApparatus
//
// The standard Hall A High Resolution Spectrometers (HRS).
//
// The usual name of this object is either "R" or "L", for Left 
// and Right HRS, respectively.
//
// Defines the functions FindVertices() and TrackCalc(), which are common
// to both the LeftHRS and the RightHRS.
//
// By default, the THaSBUApparatus class does not define any detectors. (This is new
// in analyzer 1.6 and later.) In this way, the user has full control over
// the detector configuration in the analysis script.
// Detectors should be added with AddDetector() as usual.
//
// However: To maintain backward compatibility with old scripts, the THaSBUApparatus
// will auto-create the previous set of standard detectors, "vdc", "s1" and
// "s2", if no "vdc" detector is defined at Init() time.
// This can be turned off by calling AutoStandardDetectors(kFALSE).
//
// For timing calculations, one can specify a reference detector via SetRefDet
// (usually a scintillator) as the detector at the 'reference distance',
// corresponding to the pathlength correction matrix.
//
//////////////////////////////////////////////////////////////////////////

#include "THaSBUApparatus.h"
#include "THaTrackingDetector.h"
#include "THaTrack.h"
#include "THaScintillator.h"  // includes THaNonTrackingDetector
#include "THaVDC.h"
#include "THaTrackProj.h"
#include "THaTriggerTime.h"
#include "TMath.h"
#include "TList.h"
#include <cassert>

#ifdef WITH_DEBUG
#include <iostream>
#endif


using namespace std;

//_____________________________________________________________________________
THaSBUApparatus::THaSBUApparatus( const char* name, const char* description ) :
  THaSpectrometer( name, description ), fRefDet(0)
{
  // Constructor

  SetTrSorting(kFALSE);
  AutoStandardDetectors(kTRUE); // for backward compatibility
}

//_____________________________________________________________________________
THaSBUApparatus::~THaSBUApparatus()
{
  // Destructor
}

//_____________________________________________________________________________
Bool_t THaSBUApparatus::SetTrSorting( Bool_t set )
{
  Bool_t oldset = TestBit(kSortTracks);
  SetBit( kSortTracks, set );
  return oldset;
}

//_____________________________________________________________________________
Bool_t THaSBUApparatus::GetTrSorting() const
{
  return TestBit(kSortTracks);
}
 
//_____________________________________________________________________________
Bool_t THaSBUApparatus::AutoStandardDetectors( Bool_t set )
{
  Bool_t oldset = TestBit(kAutoStdDets);
  SetBit( kAutoStdDets, set );
  return oldset;
}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THaSBUApparatus::Init( const TDatime& run_time )
{
  // Special HRS Init method for approximate backward compatibility with old
  // scripts. If no "vdc" detector has been defined by the user, assume we
  // are being run from an old script and create the old set of "standard
  // detectors" ("vdc", "s1", "s2") at the beginning of the detector list.
  // Note that the old script may have defined non-standard detectors, e.g.
  // Cherenkov, Shower, FPP etc.
  // This behavior can be turned off by calling AutoStandardDetectors(kFALSE).
  
  return THaSpectrometer::Init(run_time);
}

//_____________________________________________________________________________
Int_t THaSBUApparatus::SetRefDet( const char* name )
{
  // Set reference detector for TrackTimes calculation to the detector
  // with the given name (typically a scintillator)

  return 0;
}

//_____________________________________________________________________________
Int_t THaSBUApparatus::SetRefDet( const THaNonTrackingDetector* obj )
{
 

  return 0;
}

//_____________________________________________________________________________
Int_t THaSBUApparatus::FindVertices( TClonesArray& tracks )
{
 

  return 0;
}

//_____________________________________________________________________________
Int_t THaSBUApparatus::TrackCalc()
{
  // Additioal track calculations. At present, we only calculate beta here.

  return TrackTimes( fTracks );
}

//_____________________________________________________________________________
Int_t THaSBUApparatus::TrackTimes( TClonesArray* Tracks )
{

  return 0;
}

//_____________________________________________________________________________
ClassImp(THaSBUApparatus)

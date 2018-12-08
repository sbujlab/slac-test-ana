#ifndef Podd_THaSBUScint_h_
#define Podd_THaSBUScint_h_

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THaSBUScint                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaNonTrackingDetector.h"

class TClonesArray;

class THaSBUScint : public THaNonTrackingDetector {

public:
  THaSBUScint( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  THaSBUScint();
  virtual ~THaSBUScint();

  virtual void       Clear( Option_t* opt="" );
  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );

  
protected:

  Int_t fhadc0;
  Int_t fhadc1;
  Int_t fhadc2;
  Int_t fhadc3;
  Int_t fhadc4;
  Int_t fhadc5;
  Int_t fhadc6;
  Int_t fhadc7;
  Int_t fhadc8;
  Int_t fhadc9;
  Int_t fhadc10;
  Int_t fhadc11;
  Int_t fhadc12;
  Int_t fhadc13;
  Int_t fhadc14;
  Int_t fhadc15;

  Int_t fladc0;
  Int_t fladc1;
  Int_t fladc2;
  Int_t fladc3;
  Int_t fladc4;
  Int_t fladc5;
  Int_t fladc6;
  Int_t fladc7;
  Int_t fladc8;
  Int_t fladc9;
  Int_t fladc10;
  Int_t fladc11;
  Int_t fladc12;
  Int_t fladc13;
  Int_t fladc14;
  Int_t fladc15;

//  Int_t fadc[32];

  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );

  ClassDef(THaSBUScint,1)   // Generic scintillator class
};

////////////////////////////////////////////////////////////////////////////////
#endif

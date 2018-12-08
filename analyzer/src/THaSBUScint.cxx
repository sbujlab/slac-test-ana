///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THaSBUScint                                                           //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaSBUScint.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THaTrackProj.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <sstream>

using namespace std;

//_____________________________________________________________________________
THaSBUScint::THaSBUScint( const char* name, const char* description,
				  THaApparatus* apparatus )
  : THaNonTrackingDetector(name,description,apparatus),
    fhadc0(0), fhadc1(0), fhadc2(0), fhadc3(0), fhadc4(0),
    fhadc5(0), fhadc6(0), fhadc7(0), fhadc8(0), fhadc9(10),
    fhadc10(0), fhadc11(0), fhadc12(0), fhadc13(0), fhadc14(0),
    fhadc15(0), fladc0(0), fladc1(0), fladc2(0), fladc3(0),
    fladc4(0), fladc5(0), fladc6(0), fladc7(0), fladc8(0),
    fladc9(0), fladc10(0), fladc11(0), fladc12(0), fladc13(0),
    fladc14(0), fladc15(0)
{
  // Constructor
}

//_____________________________________________________________________________
THaSBUScint::THaSBUScint()
  : THaNonTrackingDetector(),
    fhadc0(0), fhadc1(0), fhadc2(0), fhadc3(0), fhadc4(0),
    fhadc5(0), fhadc6(0), fhadc7(0), fhadc8(0), fhadc9(10),
    fhadc10(0), fhadc11(0), fhadc12(0), fhadc13(0), fhadc14(0),
    fhadc15(0), fladc0(0), fladc1(0), fladc2(0), fladc3(0),
    fladc4(0), fladc5(0), fladc6(0), fladc7(0), fladc8(0),
    fladc9(0), fladc10(0), fladc11(0), fladc12(0), fladc13(0),
    fladc14(0), fladc15(0)
{
  // Default constructor (for ROOT I/O)

}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THaSBUScint::Init( const TDatime& date )
{
  // Extra initialization for scintillators: set up DataDest map

  if( THaNonTrackingDetector::Init( date ) )
     return fStatus;

  return fStatus = kOK;
}

//_____________________________________________________________________________
Int_t THaSBUScint::ReadDatabase( const TDatime& date )
{
 
   // Read this detector's parameters from the database

  const char* const here = "ReadDatabase";
  //cout<<"THaSBUScint ReadDataBase(000)"<<endl;
  FILE* file = OpenFile( date );
  if( !file ) return kFileError;
  
  fDetMap->Clear(); 
  // Read fOrigin and fSize (required!)
  Int_t err = ReadGeometry( file, date, true );
  if( err ) {
    fclose(file);
    return err;
  }
  //cout<<"THaSBUScint ReadDataBase (111) err ReadGeometry "<<err<<endl;
  //vector<Int_t> detmap;
  UShort_t crate;
  UShort_t slot;
  UShort_t lo;
  UShort_t hi;
  Int_t nelem;
   // Read configuration parameters
  DBRequest config_request[] = {
    { "crate",       &crate,   kUShort },
    { "slot",       &slot,   kUShort },
    { "lo",       &lo,   kUShort },
    { "hi",       &hi,   kUShort },
    { "npaddles",     &nelem,    kInt  },
    { 0 }
  };
  err = LoadDB( file, date, config_request, fPrefix );
  cout<<"Test crate "<<crate<<" Slot "<<slot<<" lo "<<lo<<" hi "<<hi<<" nelem "<<nelem<<endl; 
  if ( fDetMap->AddModule(crate, slot, lo, hi, 0, 965) < 0 )
   {
      cout<<"Some problem to add module "<<endl;
   }
  // Sanity checks
  if( !err && nelem <= 0 ) {
    Error( Here(here), "Invalid number of paddles: %d", nelem );
    err = kInitError;
  }

  cout<<"THaSBUScint ReadDataBase (222) err  "<<err<<"  fNelem  "<<nelem<<endl;
  // Reinitialization only possible for same basic configuration
  if( !err ) {
    if( fIsInit && nelem != fNelem ) {
      Error( Here(here), "Cannot re-initalize with different number of paddles. "
	     "(was: %d, now: %d). Detector not re-initialized.", fNelem, nelem );
      err = kInitError;
    } else
      fNelem = nelem;
  }

  cout<<"THaSBUScint ReadDataBase (333) err  "<<err<<"  fNelem  "<<nelem<<endl;
  /*
  UInt_t flags = THaDetMap::kFillLogicalChannel | THaDetMap::kFillModel;
  if( !err && FillDetMap(detmap, flags, here) <= 0 ) {
    err = kInitError;  // Error already printed by FillDetMap
  }
  */
  cout<<"test chandan err "<< err<<" nelem "<< fDetMap->GetTotNumChan()<<endl;
 /* 
  if( !err && (nelem = fDetMap->GetTotNumChan()) != 4*fNelem ) {
    Error( Here(here), "Number of detector map channels (%d) "
	   "inconsistent with 4*number of paddles (%d)", nelem, 4*fNelem );
    err = kInitError;
  }
  */
  fclose(file);
  THaDetMap::Module* d = fDetMap->GetModule(fDetMap->GetSize());
  cout<<" THaDetMap Module pointer "<<fDetMap<<endl;
  //cout<<" Module pointer "<< d->crate <<endl;
  cout<<" Inside ReadDataBase "<<endl; 
  return kOK;
}

//_____________________________________________________________________________
Int_t THaSBUScint::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder
  
  //if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );  

  //cout<<"THaSBUScint : DefineVariables "<< endl;
 /* 
  VarDef vars[]={
  {"adc",  "ADC values", kInt, 32, fadc },
  { 0 }
};
*/
 RVarDef vars[] = {
  {"hadc0","HR ADC 0","fhadc0"},
  {"hadc1","HR ADC 1","fhadc1"},
  {"hadc2","HR ADC 2","fhadc2"},
  {"hadc3","HR ADC 3","fhadc3"},
  {"hadc4","HR ADC 4","fhadc4"},
  {"hadc5","HR ADC 5","fhadc5"},
  {"hadc6","HR ADC 6","fhadc6"},
  {"hadc7","HR ADC 7","fhadc7"},
  {"hadc8","HR ADC 8","fhadc8"},
  {"hadc9","HR ADC 9","fhadc9"},
  {"hadc10","HR ADC 10","fhadc10"},
  {"hadc11","HR ADC 11","fhadc11"},
  {"hadc12","HR ADC 12","fhadc12"},
  {"hadc13","HR ADC 13","fhadc13"},
  {"hadc14","HR ADC 14","fhadc14"},
  {"hadc15","HR ADC 15","fhadc15"},
  {"ladc0","LR ADC 0","fladc0"},
  {"ladc1","LR ADC 1","fladc1"},
  {"ladc2","LR ADC 2","fladc2"},
  {"ladc3","LR ADC 3","fladc3"},
  {"ladc4","LR ADC 4","fladc4"},
  {"ladc5","LR ADC 5","fladc5"},
  {"ladc6","LR ADC 6","fladc6"},
  {"ladc7","LR ADC 7","fladc7"},
  {"ladc8","LR ADC 8","fladc8"},
  {"ladc9","LR ADC 9","fladc9"},
  {"ladc10","LR ADC 10","fladc10"},
  {"ladc11","LR ADC 11","fladc11"},
  {"ladc12","LR ADC 12","fladc12"},
  {"ladc13","LR ADC 13","fladc13"},
  {"ladc14","LR ADC 14","fladc14"},
  {"ladc15","LR ADC 15","fladc15"},
  { 0}
};

 //DefineVariables( vars);
 return DefineVarsFromList( vars, mode );
//  return kOK;
}

//_____________________________________________________________________________
THaSBUScint::~THaSBUScint()
{
  // Destructor. Remove variables from global list.

  if( fIsSetup )
    RemoveVariables();
  if( fIsInit )
    DeleteArrays();
}

//_____________________________________________________________________________
void THaSBUScint::DeleteArrays()
{
  // Delete member arrays. Used by destructor.

}

//_____________________________________________________________________________
void THaSBUScint::Clear( Option_t* opt )
{
  // Reset per-event data.

  THaNonTrackingDetector::Clear(opt);
  assert(fIsInit);
}

//_____________________________________________________________________________
Int_t THaSBUScint::Decode( const THaEvData& evdata )
{
  bool has_warning = false;
  const char* const here = "Decode";

//  cout<<"THaSBUScint Decoder Module  "<<fName<<endl;//"  crate "<<fCrate
 // <<" slot "<<fSlot<<endl;
  //cout <<"-- Det (name: "<<GetName()<<") Decode THaEvData of Run#"<<evdata.GetRunNum()<<" --"<<endl;
  //cout<<"Get size fDetMap  "<<fDetMap->GetSize()<<" Module fDetMap  "<<fDetMap->GetModule(fDetMap->GetSize()) <<fDetMap<<endl;
  THaDetMap::Module* d = fDetMap->GetModule(fDetMap->GetSize());
  //cout<<" Module pointer "<< d <<endl;  
  //cout<<" crate "<<d->crate<<"  slot "<<d->slot<<endl;
  //cout<<"EvType="<<evdata.GetEvType()<<"; EvLength="<<evdata.GetEvLength()<<"; EvNum="<<evdata.GetEvNum()<<endl;


//  for( Int_t i = 0; i < fDetMap->GetSize(); i++ ) {
  {  THaDetMap::Module* d = fDetMap->GetModule( 0 );
    
  //  cout<<"Decoder THaSBUScint 1111:"<<endl;
    
    // Loop over all channels that have a hit.
    for( Int_t j = 0; j < evdata.GetNumChan( 4, 3 ); j++) {

      Int_t chan = evdata.GetNextChan( 4, 3, j );
      if( chan < 0 || chan > 32  ) continue;     // Not one of my channels
     
      //cout<<" Decode:  crate : "<<4<<"  slot : "<<3<<endl;      

      Int_t nhit = evdata.GetNumHits(4, 3, chan);
      if( nhit > 1 || nhit == 0 ) {
        ostringstream msg;
        msg << nhit << " hits on "
            << " channel " << 4 << "/" << 3 << "/" << chan;
        ++fMessages[msg.str()];
        has_warning = true;
        if( nhit == 0 ) {
          msg << ". Should never happen. Decoder bug. Call expert.";
          Warning( Here(here), "Event %d: %s", evdata.GetEvNum(),
                   msg.str().c_str() );
          continue;
        }
#ifdef WITH_DEBUG
        if( fDebug>0 ) {
          Warning( Here(here), "Event %d: %s", evdata.GetEvNum(),
                   msg.str().c_str() );
        }
#endif
      }
// Get the data. If multiple hits on a TDC channel, take
//        either first or last hit, depending on TDC mode
      assert( nhit>0 );
      Int_t data = evdata.GetData( 4, 3, chan, 0 );//mot sure about this zero cg
      //cout<<"Data chandan  j "<<j<<"  " <<data<<endl;
      if(j==0)fhadc0=data;
      if(j==1)fhadc8=data;
      if(j==2)fladc0=data;
      if(j==3)fladc8=data;

      if(j==4)fhadc1=data;
      if(j==5)fhadc9=data;
      if(j==6)fladc1=data;
      if(j==7)fladc9=data;

      if(j==8)fhadc2=data;
      if(j==9)fhadc10=data;
      if(j==10)fladc2=data;
      if(j==11)fladc10=data;

      if(j==12)fhadc3=data;
      if(j==13)fhadc11=data;
      if(j==14)fladc3=data;
      if(j==15)fladc11=data;

      if(j==16)fhadc4=data;
      if(j==17)fhadc12=data;
      if(j==18)fladc4=data;
      if(j==19)fladc12=data;

      if(j==20)fhadc5=data;
      if(j==21)fhadc13=data;
      if(j==22)fladc5=data;
      if(j==23)fladc13=data;

      if(j==24)fhadc6=data;
      if(j==25)fhadc14=data;
      if(j==26)fladc6=data;
      if(j==27)fladc14=data;

      if(j==28)fhadc7=data;
      if(j==29)fhadc15=data;
      if(j==30)fladc7=data;
      if(j==31)fladc15=data;

  }
  //cout<<"fhadc2  "<<fhadc2<<endl;
  //return 0;
}

  return 0;
}

//_____________________________________________________________________________
Int_t THaSBUScint::CoarseProcess( TClonesArray& tracks )
{

  return 0;
}

//_____________________________________________________________________________
Int_t THaSBUScint::FineProcess( TClonesArray& tracks )
{

  return 0;
}

//_____________________________________________________________________________
ClassImp(THaSBUScint)

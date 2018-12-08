/////////////////////////////////////////////////////////////////////
//
//   Caen965Module
//   author Stephen Wood
//   author Vincent Sulkosky
//   
//   Decoder module to retrieve Caen 965 QDCs.  Based on CAEN 792 decoding in
//   SkeletonModule.C in podd 1.6.   (Written by S. Wood, modified by V. Sulkosky)
//
/////////////////////////////////////////////////////////////////////


#include "Caen965Module.h"
#include "THaSlotData.h"
#include "TString.h"
#include <iostream>

using namespace std;

#define DEBUG 1
#define WITH_DEBUG 0

namespace Decoder {

Module::TypeIter_t Caen965Module::fgThisType =
  DoRegister( ModuleType( "Decoder::Caen965Module" , 965 ));

//Caen965Module::Caen965Module(Int_t crate, Int_t slot) : VmeModule(crate, slot) {
Caen965Module::Caen965Module(Int_t crate, Int_t slot) : VmeModule(crate, 3) {
  fDebugFile=0;
  Init();
}

Caen965Module::~Caen965Module() {
  if(fNumHits) delete [] fNumHits;
}

void Caen965Module::Init() {
  Module::Init();
//  cout << endl << "Initializing v" << MyModName() << "!" << endl << endl;
  fNumChan=NQDCCHAN;
  for (Int_t i=0; i<fNumChan; i++) fData.push_back(0);
#if defined DEBUG && defined WITH_DEBUG
  // This will make a HUGE output
  delete fDebugFile; fDebugFile = 0;
  fDebugFile = new ofstream;
#if __cplusplus >= 201103L
  fDebugFile->open(string("v")+MyModName()+"debug.txt");
#else
  fDebugFile->open((string("v")+MyModName()+"debug.txt").c_str());
#endif
#endif
  //fDebugFile=0;
  Clear();
  IsInit = kTRUE;
  TString modtypeup(MyModType());
  modtypeup.ToUpper();
  fName = Form("Caen %s %s Module",modtypeup.Data(),MyModName());
}

Int_t Caen965Module::LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, const UInt_t *pstop) {
// This is a simple, default method for loading a slot
  const UInt_t *p = evbuffer;
  fWordsSeen = 0;
//  cout << "version like V965"<<endl;
  ++p;
  Int_t nword=*p-2;
  ++p;
  for (Int_t i=0;i<nword;i++) {
       ++p;
       if (p>pstop)
	 break;
       UInt_t chan=((*p)&0x00ff0000)>>16;
       UInt_t raw=((*p)&0x00000fff);
      // cout<<"Before word "<<i<<"  "<<chan<<"  "<<raw<<endl;
       Int_t status = sldat->loadData(MyModType(),chan,raw,raw);
       fWordsSeen++;
       if (chan < fData.size()) fData[chan]=raw;
       //cout << "word   "<<i<<"   "<<chan<<"   "<<raw<<endl;
       if( status != SD_OK ) return -1;
  }
  return fWordsSeen;
}

Int_t Caen965Module::LoadSlot(THaSlotData *sldat, const UInt_t *evbuffer, Int_t pos, Int_t len) {
  
  // Fill data structures of this class
  // Read until out of data or until decode says that the slot is finished
  // len = ndata in event, pos = word number for block header in event
  Clear();
  Int_t index = 0;
  Int_t counter = 0;
  Int_t nword = 0;

  const UInt_t *p = evbuffer;
  fWordsSeen = 0; 

#ifdef WITH_DEBUG
  //cout << "Number of words from 965: \t" << len << endl << endl;
#endif
  // cout << "hi !  I am a 965 "<<len<<endl;
  
  while (fWordsSeen < len) {   
    index = pos + fWordsSeen;
    // first word is the header
    if (fWordsSeen == 0) {
      nword=((p[index])&0x00003f00)>>8; // number of converted channels bits 8-13
      fWordsSeen++;
    } else if (fWordsSeen <= nword) { // excludes the End of Block (EOB) word
      UInt_t chan=((p[index])&0x00ff0000)>>16; // number of channel which data are coming from bits 16-20
      UInt_t raw=((p[index])&0x00000fff);      // raw datum bits 0-11
    //  cout<<"Before word "<<index-pos<<"  "<<chan<<"  "<<raw<<endl;
      Int_t status = sldat->loadData(MyModType(),chan,raw,raw);
      fWordsSeen++;
      counter++;
      if (chan < fData.size()) fData[chan]=raw;
#ifdef WITH_DEBUG
      //cout << "word\t"<<index<<"\t"<<nword<<"\t"<<raw<<endl;
#endif
      if( status != SD_OK ) return -1;
    } else fWordsSeen++; // increment the counter for the EOB word

#ifdef WITH_DEBUG
    //cout << "word   "<<i<<"   "<<nword<<"   "<<raw<<endl;
    if (fDebugFile != 0)
      *fDebugFile << "\n" << "Caen965Module::LoadSlot >> evbuffer[" << index 
		  << "] = " << hex << evbuffer[index] << dec << " >> crate = "
		  << fCrate << " >> slot = " << fSlot << " >> pos = " 
		  << pos << " >> fWordsSeen = " << fWordsSeen 
		  << " >> len = " << len << " >> index = " << index << "\n" << endl;
#endif
  }
  if (counter != nword) cout << Form("Warning in v%s Number of converted channels, %d, is not equal to number of decoded words, %d!",MyModName(),
				     nword,counter) << endl << endl;
  return fWordsSeen;
}

  /* Does anything use this method */
UInt_t Caen965Module::GetData(Int_t chan) const {
  if (chan < 0 || chan > fNumChan) return 0;
  return fData[chan];
}

void Caen965Module::Clear(const Option_t* opt) {
  VmeModule::Clear(opt);
  fNumHits = 0;
  for (Int_t i=0; i<fNumChan; i++) fData[i]=0;
}

}

ClassImp(Decoder::Caen965Module)

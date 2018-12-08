#ifndef Podd_Caen965Module_h_
#define Podd_Caen965Module_h_

/////////////////////////////////////////////////////////////////////
//
//   Caen965Module
//   Single Hit QDC 
//   Copied from Caen775Module.h
//
/////////////////////////////////////////////////////////////////////

#include "VmeModule.h"

namespace Decoder {

class Caen965Module : public VmeModule {

public:

   Caen965Module() : VmeModule() {}
   Caen965Module(Int_t crate, Int_t slot);
   virtual ~Caen965Module();

   using Module::GetData;
   using Module::LoadSlot;

   virtual UInt_t GetData(Int_t chan) const;
   virtual void Init();
   virtual void Clear(const Option_t *opt="");
   virtual Int_t Decode(const UInt_t*) { return 0; }
   virtual Int_t LoadSlot(THaSlotData *sldat,  const UInt_t *evbuffer, const UInt_t *pstop );
   virtual Int_t LoadSlot(THaSlotData *sldat, const UInt_t* evbuffer, Int_t pos, Int_t len);
   virtual const char* MyModType() {return "qdc";}
   virtual const char* MyModName() {return "965";}
 
private:

   static const size_t NQDCCHAN = 32; //actually 16 channels, but it has higher and lower resolutions

   Int_t *fNumHits;

   static TypeIter_t fgThisType;
   ClassDef(Caen965Module,0)  //  Caen965 of a module; make your replacements

};

}

#endif

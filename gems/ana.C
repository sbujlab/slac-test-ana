
#include "TSystem.h"
#include "TList.h"
#include "THaRun.h"
#include "THaEvent.h"
#include "THaAnalyzer.h"
#include "THaApparatus.h"
//#pragma cling load("libsbs.so");
//#include "SBSGEMStand.h"
//#include "SBSBigBite.h"

void ana(){

    gSystem->Load("libsbs.so");
    
    SBSBigBite   *sbs = new SBSBigBite("sbs", "Generic apparatus");
    SBSGEMStand *gems = new SBSGEMStand("gems", "Collection of GEMs in stand");
    THaSBUScint *det = new THaSBUScint("sbuscint", "sbu scint");

    sbs->AddDetector(gems);
    sbs->AddDetector(det);
    

  //
  //  Steering script for Hall A analyzer demo
  //
  
  // Set up the equipment to be analyzed.
  
  // add the two spectrometers with the "standard" configuration
  // (VDC planes, S1, and S2)
  // Collect information about a easily modified random set of channels
  // (see DB_DIR/*/db_D.dat)
  /*
  THaApparatus* DECDAT = new THaDecData("D","Misc. Decoder Data");
  gHaApps->Add( DECDAT );
  */
  

  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Apparatus's and PhysicsModules,
  // and executes the output routines.
  THaAnalyzer* analyzer = new THaAnalyzer;
  
  gHaApps->Add(sbs);

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;
  
  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
//  THaRun* run = new THaRun( "prod12_4100V_TrigRate25_4.dat" );
  //THaRun* run = new THaRun( "/home/chad/data/test_589.dat" );
  int argc=0;
  std::cout<<"enter file name: "<<std::endl;
  cin>>argc;
  //THaRun* run = new THaRun(Form("/home/coda/data/test_%d.dat",argc));
  THaRun* run = new THaRun(Form("/home/coda/data/run_%d.dat",argc));
  run->SetLastEvent(-1);

  run->SetDataRequired(0);
  run->SetDate(TDatime());

  analyzer->SetVerbosity(0);
  
  // Define the analysis parameters
  analyzer->SetEvent( event );
  //analyzer->SetOutFile(Form("~/rootfiles/test_%d.root",argc));
  analyzer->SetOutFile(Form("~/rootfiles/run_%d.root",argc));
  // File to record cuts accounting information
  analyzer->SetSummaryFile("summary_example.log"); // optional
  
  //analyzer->SetCompressionLevel(0); // turn off compression
  analyzer->Process(run);     // start the actual analysis
}

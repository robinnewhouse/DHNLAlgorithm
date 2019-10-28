#include "DHNLAlgorithm/DHNLFilter.h"

#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include "AthContainers/ConstDataVector.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include <DHNLAlgorithm/DHNLFunctions.h>
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODTruth/TruthVertex.h>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "TFile.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

static float GeV = 1000.;


// this is needed to distribute the algorithm to the workers
ClassImp(DHNLFilter)

DHNLFilter :: DHNLFilter ()
{
// Here you put any code for the base initialization of variables,
// e.g. initialize all pointers to 0.  Note that you should only put
// the most basic initialization here, since this method will be
// called on both the submission and the worker node.  Most of your
// initialization code will go into histInitialize() and
// initialize().

    ANA_MSG_INFO("DHNLFilter() : Calling constructor");

//    m_allJetContainerName      = "";
//    m_inMuContainerName        = "";
//    m_inElContainerName        = "";
//    m_msgLevel                 = MSG::INFO;
//    m_TrackMinPt               = 0;
//    m_TrackZ0Max               = 0;
//    m_TrackD0Max               = 0;
//    m_jetPtCut                 = 0;
//    m_AlphaMaxCut              = 0;
//    m_CHFCut                   = 0;
//    m_electronPtCut            = 0;
//    m_muonPtCut                = 0;
}



EL::StatusCode  DHNLFilter :: configure ()
{
    ANA_MSG_INFO("configure() : Configuring DHNLFilter Interface.");

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLFilter :: setupJob (EL::Job& job)
{
    // Here you put code that sets up the job on the submission object
    // so that it is ready to work with your algorithm, e.g. you can
    // request the D3PDReader service or add output files.  Any code you
    // put here could instead also go into the submission script.  The
    // sole advantage of putting it here is that it gets automatically
    // activated/deactivated when you add/remove the algorithm from your
    // job, which may or may not be of value to you.
    job.useXAOD();
    xAOD::Init( "DHNLFilter" ).ignore(); // call before opening first file

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLFilter :: histInitialize ()
{
    // Here you do everything that needs to be done at the very                                                                                                                        
    // beginning on each worker node, e.g. create histograms and output                                                                                                                
    // trees.  This method gets called before any input files are                                                                                                                      
    // connected.                                                                                                                                                                      
    ANA_MSG_INFO("histInitialize() : Calling histInitialize \n");

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode DHNLFilter :: fileExecute ()
{
    // Here you do everything that needs to be done exactly once for every                                                                                                             
    // single file, e.g. collect a list of all lumi-blocks processed                                                                                                                   
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter :: changeInput (bool /*firstFile*/)
{
    // Here you do everything you need to do when we change input files,                                                                                                               
    // e.g. resetting branch addresses on trees.  If you are using                                                                                                                     
    // D3PDReader or a similar service this method is not needed.                                                                                                                      
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter :: initialize ()
{
    // Here you do everything that you need to do after the first input
    // file has been connected and before the first event is processed,
    // e.g. create additional histograms based on which variables are
    // available in the input files.  You can also create all of your
    // histograms and trees in here, but be aware that this method
    // doesn't get called if no events are processed.  So any objects
    // you create here won't be available in the output if you have no
    // input events.

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();


    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter :: execute ()
{
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.
    //cout<<"Hto4b execute"<<endl;

    ANA_MSG_DEBUG("execute(): Applying selection");
    return EL::StatusCode::SUCCESS;

}





EL::StatusCode DHNLFilter :: postExecute ()
{
    // Here you do everything that needs to be done after the main event                                                                                                               
    // processing.  This is typically very rare, particularly in user                                                                                                                  
    // code.  It is mainly used in implementing the NTupleSvc.                                                                                                                         
    return EL::StatusCode::SUCCESS;
}




EL::StatusCode DHNLFilter :: finalize ()
{
    // This method is the mirror image of initialize(), meaning it gets
    // called after the last event has been processed on the worker node
    // and allows you to finish up any objects you created in
    // initialize() before they are written to disk.  This is actually
    // fairly rare, since this happens separately for each worker node.
    // Most of the time you want to do your post-processing on the
    // submission node after all your histogram outputs have been
    // merged.  This is different from histFinalize() in that it only
    // gets called on worker nodes that processed input events.

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter :: histFinalize ()
{
    // This method is the mirror image of histInitialize(), meaning it                                                                                                                 
    // gets called after the last event has been processed on the worker                                                                                                               
    // node and allows you to finish up any objects you created in                                                                                                                     
    // histInitialize() before they are written to disk.  This is                                                                                                                      
    // actually fairly rare, since this happens separately for each                                                                                                                    
    // worker node.  Most of the time you want to do your                                                                                                                              
    // post-processing on the submission node after all your histogram                                                                                                                 
    // outputs have been merged.  This is different from finalize() in                                                                                                                 
    // that it gets called on all worker nodes regardless of whether                                                                                                                   
    // they processed input events.  

    return EL::StatusCode::SUCCESS;
}

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

DHNLFilter::DHNLFilter() : Algorithm("DHNLFilter") {
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


EL::StatusCode DHNLFilter::configure() {
    ANA_MSG_INFO("configure() : Configuring DHNLFilter Interface.");

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLFilter::setupJob(EL::Job &job) {
    // Here you put code that sets up the job on the submission object
    // so that it is ready to work with your algorithm, e.g. you can
    // request the D3PDReader service or add output files.  Any code you
    // put here could instead also go into the submission script.  The
    // sole advantage of putting it here is that it gets automatically
    // activated/deactivated when you add/remove the algorithm from your
    // job, which may or may not be of value to you.
    job.useXAOD();
    xAOD::Init("DHNLFilter").ignore(); // call before opening first file

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLFilter::histInitialize() {
    // Here you do everything that needs to be done at the very                                                                                                                        
    // beginning on each worker node, e.g. create histograms and output                                                                                                                
    // trees.  This method gets called before any input files are                                                                                                                      
    // connected.                                                                                                                                                                      
    ANA_MSG_INFO("histInitialize() : Calling histInitialize \n");

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLFilter::fileExecute() {
    // Here you do everything that needs to be done exactly once for every                                                                                                             
    // single file, e.g. collect a list of all lumi-blocks processed                                                                                                                   
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter::changeInput(bool /*firstFile*/) {
    // Here you do everything you need to do when we change input files,                                                                                                               
    // e.g. resetting branch addresses on trees.  If you are using                                                                                                                     
    // D3PDReader or a similar service this method is not needed.                                                                                                                      
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter::initialize() {
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
    m_eventCounter = -1;


    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLFilter::execute() {
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.
    //cout<<"Hto4b execute"<<endl;

    ANA_MSG_DEBUG("execute(): Applying selection");

    ++m_eventCounter;


    ANA_MSG_DEBUG("execute() : Get Containers");
    const xAOD::EventInfo *eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));

//    const xAOD::VertexContainer* vertices = 0;
//    ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));

    const xAOD::MuonContainer *allMuons = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(allMuons, m_inMuContainerName, m_event, m_store));

    eventInfo->auxdecor<int>("passesFilter") = muonMuonFilter(allMuons);

    ANA_MSG_DEBUG("execute() : stored in auxdecor 'passesFilter'");
    return EL::StatusCode::SUCCESS;

}

bool DHNLFilter::muonMuonFilter(const xAOD::MuonContainer *allMuons) {

    int passesFilter = 0;

    ANA_MSG_DEBUG("Applying prompt muon filter cuts");
    for (const xAOD::Muon *muon1 : *allMuons) {
        if (not(muon1->pt() / GeV > 28)) continue; // pt cut
        if (not(muon1->eta() < 2.5)) continue; // eta cut
        if (not(muon1->muonType() == xAOD::Muon::Combined)) continue;
        if (not(muon1->isolation(xAOD::Iso::ptcone30) / muon1->pt() < 0.05)) continue;
        ANA_MSG_DEBUG("Prompt muon passed filter cuts");

        ANA_MSG_DEBUG("Applying displaced muon filter cuts");
        for (const xAOD::Muon *muon2 : *allMuons) {
            if (muon1 == muon2) continue; // don't compare the same muon
            if (not(muon2->pt() / GeV > 5)) continue; // pt cut
            if (not(muon2->eta() < 2.5)) continue; // eta cut
            if (not(muon2->muonType() == xAOD::Muon::Combined || muon2->muonType() == xAOD::Muon::MuonStandAlone || muon2->muonType() == xAOD::Muon::SegmentTagged)) continue;
            if (not(muon2->isolation(xAOD::Iso::ptcone30) / muon1->pt() < 1.0)) continue;


            // > 0.1 mm for “good” Combined muon
            // “good”: MS-ID matching chi2/DOF < 5.
            // Is this interpreted corectly? // Robin
            if (muon2->muonType() == xAOD::Muon::Combined) {
                // Get chi2/dof for d0 decision
                float d0 = muon2->primaryTrackParticle()->d0();
                float chi2 = -1.0f;
                int dof = -1;
                if (!muon2->parameter(chi2, xAOD::Muon::msInnerMatchChi2)) continue;
                if (!muon2->parameter(dof, xAOD::Muon::msInnerMatchDOF)) continue;
                if (dof == 0) dof = 1;

                float quality = chi2 / static_cast<float>(dof);
                if (quality < 5.0)
                    if (not(fabs(d0) > 0.1)) continue;
            }
            // I am interpreting this as if muon is StandAlone or SegmentTagged, don't submit it to the same requirements
            // This is how it appears to be done in the old framework

            ANA_MSG_DEBUG("Displaced muon passed filter cuts");

            passesFilter = 1;
            break;
        }
        break;
    }
    if (passesFilter)
        ANA_MSG_DEBUG("Event passes muon-muon filter");
    else
        ANA_MSG_DEBUG("Event fails muon-muon filter");
    return passesFilter;
}

EL::StatusCode DHNLFilter::postExecute() {
    // Here you do everything that needs to be done after the main event                                                                                                               
    // processing.  This is typically very rare, particularly in user                                                                                                                  
    // code.  It is mainly used in implementing the NTupleSvc.                                                                                                                         
    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLFilter::finalize() {
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

EL::StatusCode DHNLFilter::histFinalize() {
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

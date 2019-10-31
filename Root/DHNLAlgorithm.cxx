#include <DHNLAlgorithm/DHNLAlgorithm.h>
#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include "AthContainers/ConstDataVector.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
//#include <DHNLAlgorithm/DHNLFunctions.h>
#include "DVAnalysisBase/DVHelperFunctions.h"
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODTruth/TruthVertex.h>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "TFile.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// this is needed to distribute the algorithm to the workers
ClassImp(DHNLAlgorithm)

DHNLAlgorithm::DHNLAlgorithm() :
        m_cutflowHist(nullptr),
        m_cutflowHistW(nullptr) {
// Here you put any code for the base initialization of variables,
// e.g. initialize all pointers to 0.  This is also where you
// declare all properties for your algorithm.  Note that things like
// resetting statistics variables or booking histograms should
// rather go into the initialize() function.


    ANA_MSG_INFO("Hto4bLLPAlgorithm() : Calling constructor");

    m_inJetContainerName       = "";
    m_inputAlgo                = "";
    m_allJetContainerName      = "";
    m_allJetInputAlgo          = "";
    m_inMETContainerName       = "";
    m_inMETTrkContainerName    = "";
    m_msgLevel                 = MSG::INFO;
    m_useCutFlow               = true;
    m_MCPileupCheckContainer   = "AntiKt4TruthJets";
    m_leadingJetPtCut          = 225;
    m_subleadingJetPtCut       = 225;
    m_jetMultiplicity          = 3;
    m_truthLevelOnly           = false;
    m_metCut                   = 0;

}


EL::StatusCode DHNLAlgorithm::initialize() {
    // Here you do everything that needs to be done at the very
    // beginning on each worker node, e.g. create histograms and output
    // trees.  This method gets called before any input files are
    // connected.
    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLAlgorithm::execute() {
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.

    ANA_MSG_INFO ("in execute");


    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    // retrieve the eventInfo object from the event store
    const xAOD::EventInfo *eventInfo = nullptr;
    ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

    // print out run and event number from retrieved object
    ANA_MSG_INFO ("in execute, runNumber = " << eventInfo->runNumber() << ", eventNumber = " << eventInfo->eventNumber());

    //////////////////// Store primary vertex information //////////////////////

    SG::AuxElement::ConstAccessor<float> NPVAccessor("NPV");
    const xAOD::VertexContainer* vertices = nullptr;
    if(!m_truthLevelOnly) {
        ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));
    }
    if(!m_truthLevelOnly && !NPVAccessor.isAvailable( *eventInfo )) { // NPV might already be available
        // number of PVs with 2 or more tracks
        //eventInfo->auxdecor< int >( "NPV" ) = HelperFunctions::countPrimaryVertices(vertices, 2);
        // TMP for JetUncertainties uses the same variable
        eventInfo->auxdecor< float >( "NPV" ) = HelperFunctions::countPrimaryVertices(vertices, 2);
    }
    const xAOD::Vertex* primaryVertex = HelperFunctions::getPrimaryVertex( vertices, msg() );
    eventInfo->auxdecor< float >( "PV_x" ) = primaryVertex->x();
    eventInfo->auxdecor< float >( "PV_y" ) = primaryVertex->y();
    eventInfo->auxdecor< float >( "PV_z" ) = primaryVertex->z();


    //////////////////// Store truth information ////////////////////////////

    if (m_isMC) {
        const xAOD::TruthVertexContainer *truthVertexContainer = nullptr;
        ANA_CHECK (HelperFunctions::retrieve(truthVertexContainer, "TruthVertices", m_event, m_store));

        const xAOD::VertexContainer *secVertexContainer = nullptr;
        ANA_CHECK (HelperFunctions::retrieve(secVertexContainer, "FilteredSecondaryVertices", m_event, m_store));

        if (!secVertexContainer->empty()) {
            for (auto secVertex : *secVertexContainer) {

                DVHelper::TruthVertexLink_t truthVertexLink = secVertex->auxdecor<DVHelper::TruthVertexLink_t>("maxlinkedTruthVertexLink") ;
                const xAOD::TruthVertex * truthVertex = *truthVertexLink; // use * operator to retrieve vertex from element link

                if (truthVertex == nullptr){continue;}
                ANA_MSG_DEBUG ("vertex ID: " << truthVertex->id());
                ANA_MSG_DEBUG ("vertex barcode: " << truthVertex->barcode());
//
//                for (size_t i = 0; i < truthVertex->nIncomingParticles(); i++) {
//                    const xAOD::TruthParticle *incoming = truthVertex->incomingParticle(i);
//                    if (incoming == nullptr){continue;}
//                    ANA_MSG_DEBUG ("incoming particle pdgID: " << incoming->pdgId());
//                }
//
//
//                for (size_t j = 0; j < truthVertex->nOutgoingParticles(); j++) {
//                    const xAOD::TruthParticle *outgoing = truthVertex->outgoingParticle(j);
//                    if (outgoing == nullptr){continue;}
//                    ANA_MSG_DEBUG ("outgoing particle pdgID: " << outgoing->pdgId());
//
//                }
            }

        }
    }


    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLAlgorithm::finalize() {
    // This method is the mirror image of initialize(), meaning it gets
    // called after the last event has been processed on the worker node
    // and allows you to finish up any objects you created in
    // initialize() before they are written to disk.  This is actually
    // fairly rare, since this happens separately for each worker node.
    // Most of the time you want to do your post-processing on the
    // submission node after all your histogram outputs have been
    // merged.
    return EL::StatusCode::SUCCESS;
}

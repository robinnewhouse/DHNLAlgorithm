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


    ANA_MSG_INFO("DHNLAlgorithm() : Calling constructor");

    m_inJetContainerName = "";
    m_inputAlgo = "";
    m_allJetContainerName = "";
    m_allJetInputAlgo = "";
    m_inMETContainerName = "";
    m_inMETTrkContainerName = "";
    m_msgLevel = MSG::INFO;
    m_useCutFlow = true;
    m_MCPileupCheckContainer = "AntiKt4TruthJets";
    m_leadingJetPtCut = 225;
    m_subleadingJetPtCut = 225;
    m_jetMultiplicity = 3;
    m_truthLevelOnly = false;
    m_metCut = 0;

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

    //////////////////// Store lepton information //////////////////////

    const xAOD::MuonContainer *inMuons = nullptr;
    ANA_CHECK(HelperFunctions::retrieve(inMuons, m_inMuContainerName, m_event, m_store, msg()));

    const xAOD::ElectronContainer *inElectrons = nullptr;
    ANA_CHECK(HelperFunctions::retrieve(inElectrons, m_inElContainerName, m_event, m_store, msg()));

    for (const xAOD::Muon *muon : *inMuons) {
        muon->auxdecor<int>("index") = muon->index();
        muon->auxdecor<int>("type") = muon->muonType();
    }

    for (const xAOD::Electron *electron : *inElectrons) {
        electron->auxdecor<int>("index") = electron->index();
    }

    //////////////////// Store primary vertex information //////////////////////

    SG::AuxElement::ConstAccessor<float> NPVAccessor("NPV");
    const xAOD::VertexContainer *vertices = nullptr;
    if (!m_truthLevelOnly) {
        ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));
    }
    if (!m_truthLevelOnly && !NPVAccessor.isAvailable(*eventInfo)) { // NPV might already be available
        // number of PVs with 2 or more tracks
        //eventInfo->auxdecor< int >( "NPV" ) = HelperFunctions::countPrimaryVertices(vertices, 2);
        // TMP for JetUncertainties uses the same variable
        eventInfo->auxdecor<float>("NPV") = HelperFunctions::countPrimaryVertices(vertices, 2);
    }
    const xAOD::Vertex *primaryVertex = HelperFunctions::getPrimaryVertex(vertices, msg());
    eventInfo->auxdecor<float>("PV_x") = primaryVertex->x();
    eventInfo->auxdecor<float>("PV_y") = primaryVertex->y();
    eventInfo->auxdecor<float>("PV_z") = primaryVertex->z();


    //////////////////// Store displaced vertex information ////////////////////////////


    std::vector<std::vector<int>> secVtxTrackParticleIndex;
    std::vector<std::vector<int>> secVtxMuonIndex;
    std::vector<std::vector<int>> secVtxElectronIndex;

    const xAOD::VertexContainer *secVertexContainer = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(secVertexContainer, "FilteredSecondaryVertices", m_event, m_store));

    // Loop over each displaced vertex
    if (!secVertexContainer->empty()) {
        for (xAOD::Vertex *secVertex : *secVertexContainer) {

            std::vector<int> trackParticleIndex;
            std::vector<int> muonIndex;
            std::vector<int> electronIndex;

            // For each track particle making the vertex, store the index of the particle
            // Find the associated muon (if there is one) and store that index
            // Find the associated electron (if there is one) and store that index
            // TODO: move these to the secondary vertex collection
            for (size_t k = 0; k < secVertex->nTrackParticles(); k++) {
                const xAOD::TrackParticle *trackParticle = secVertex->trackParticle(k);
                ANA_MSG_DEBUG ("trackParticle index: " << trackParticle->index());
                trackParticleIndex.push_back(trackParticle->index());

                const xAOD::Muon *matchedMuon = matchTrackToMuon(trackParticle, inMuons);
                if (matchedMuon != nullptr) {
                    ANA_MSG_DEBUG ("matchedMuon->index(): " << matchedMuon->index());
                    muonIndex.push_back(matchedMuon->index());
                } else muonIndex.push_back(-1);

                const xAOD::Electron *matchedElectron = matchTrackToElectron(trackParticle, inElectrons);
                if (matchedElectron != nullptr) {
                    ANA_MSG_DEBUG ("matchedElectron->index(): " << matchedElectron->index());
                    electronIndex.push_back(matchedElectron->index());
                } else electronIndex.push_back(-1);
            }


            // If sample is simulated, store truth information
            // including vertex identification and incoming/outgoing particle identification
            // TODO: move these to the secondary vertex collection
            if (m_isMC) {
                DVHelper::TruthVertexLink_t truthVertexLink = secVertex->auxdecor<DVHelper::TruthVertexLink_t>("maxlinkedTruthVertexLink");
                const xAOD::TruthVertex *truthVertex = *truthVertexLink; // use * operator to retrieve vertex from element link

                if (truthVertex == nullptr) { continue; }
                ANA_MSG_DEBUG ("vertex ID: " << truthVertex->id());
                ANA_MSG_DEBUG ("vertex barcode: " << truthVertex->barcode());

                for (size_t i = 0; i < truthVertex->nIncomingParticles(); i++) {
                    const xAOD::TruthParticle *incoming = truthVertex->incomingParticle(i);
                    if (incoming == nullptr) { continue; }
                    ANA_MSG_DEBUG ("incoming particle pdgID: " << incoming->pdgId());
                    ANA_MSG_DEBUG ("incoming particle barcode: " << incoming->barcode());
                }

                for (size_t j = 0; j < truthVertex->nOutgoingParticles(); j++) {
                    const xAOD::TruthParticle *outgoing = truthVertex->outgoingParticle(j);
                    if (outgoing == nullptr) { continue; }
                    ANA_MSG_DEBUG ("outgoing particle pdgID: " << outgoing->pdgId());
                    ANA_MSG_DEBUG ("outgoing particle barcode: " << outgoing->barcode());
                }
            }

            secVtxTrackParticleIndex.push_back(trackParticleIndex);
            secVtxMuonIndex.push_back(muonIndex);
            secVtxElectronIndex.push_back(electronIndex);
            // testing for later storing as aux on vertex
            secVertex->auxdecor<std::vector<int>>("trackParticleIndex") = trackParticleIndex;
            secVertex->auxdecor<std::vector<int>>("muonIndex") = muonIndex;
            secVertex->auxdecor<std::vector<int>>("electronIndex") = electronIndex;

        }
    }

    eventInfo->auxdecor<std::vector<std::vector<int>>>("secVtxTrackParticleIndex") = secVtxTrackParticleIndex;
    eventInfo->auxdecor<std::vector<std::vector<int>>>("secVtxMuonIndex") = secVtxMuonIndex;
    eventInfo->auxdecor<std::vector<std::vector<int>>>("secVtxElectronIndex") = secVtxElectronIndex;


    return EL::StatusCode::SUCCESS;
}

const xAOD::Muon *DHNLAlgorithm::matchTrackToMuon(const xAOD::TrackParticle *track, const xAOD::MuonContainer *inMuons) {
    for (const xAOD::Muon *muon : *inMuons) {
        // Quesion: Does this work beause we're only interested in ID tracks?
        const xAOD::TrackParticle *id_tr = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
//        const xAOD::TrackParticle * id_tr = muon->primaryTrackParticle();
        if (id_tr == nullptr)continue;
        if (id_tr == track) {
            return muon;
        }
    }
    ANA_MSG_DEBUG ("Could not match muon");
    return nullptr;
}

const xAOD::Electron *DHNLAlgorithm::matchTrackToElectron(const xAOD::TrackParticle *track, const xAOD::ElectronContainer *inElectrons) {
    for (const xAOD::Electron *electron : *inElectrons) {
        for (size_t i = 0; i < electron->nTrackParticles(); ++i) {
            const xAOD::TrackParticle *id_tr = electron->trackParticle(i);
            if (id_tr == nullptr)continue;
            if (id_tr == track) {
                return electron;
            }
        }
    }
    ANA_MSG_DEBUG ("Could not match electron");
    return nullptr;
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


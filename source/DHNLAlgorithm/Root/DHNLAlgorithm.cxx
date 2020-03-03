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

static float GeV = 1000.;

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

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    // retrieve the eventInfo object from the event store
    const xAOD::EventInfo *eventInfo = nullptr;
    ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

    // print out run and event number from retrieved object
    ANA_MSG_DEBUG ("in execute, runNumber = " << eventInfo->runNumber() << ", eventNumber = " << eventInfo->eventNumber());

    //////////////////// Store lepton information //////////////////////

    const xAOD::MuonContainer *inMuons = nullptr;
    ANA_CHECK(HelperFunctions::retrieve(inMuons, m_inMuContainerName, m_event, m_store, msg()));

    const xAOD::ElectronContainer *inElectrons = nullptr;
    ANA_CHECK(HelperFunctions::retrieve(inElectrons, m_inElContainerName, m_event, m_store, msg()));

    // Copy over the aux data containing filter pass information
    // We think this should be done automatically in the shallow copy od MuonCalibrator.cxx, but it appears not to be.
    // Be careful with these hardcoded collection names.
    const xAOD::MuonContainer *inMuonsUncalibrated = nullptr;
    ANA_CHECK(HelperFunctions::retrieve(inMuonsUncalibrated, "Muons", m_event, m_store, msg()));

    const xAOD::ElectronContainer *inElectronsUncalibrated = nullptr;
    ANA_CHECK(HelperFunctions::retrieve(inElectronsUncalibrated, "Electrons", m_event, m_store, msg()));

    const xAOD::TrackParticleContainer *tracks = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(tracks, "InDetTrackParticles", m_event, m_store));

    for (const xAOD::Muon *muon : *inMuons) {
        muon->auxdecor<int>("index") = muon->index();
        muon->auxdecor<int>("type") = muon->muonType();
        muon->auxdecor<float>("px") = muon->p4().Px() / GeV;
        muon->auxdecor<float>("py") = muon->p4().Py() / GeV;
        muon->auxdecor<float>("pz") = muon->p4().Pz() / GeV;
//        muon->auxdecor<float>("ptC30") = muon->isolation(xAOD::Iso::ptcone30);
        if (not(m_inMuContainerName == "Muons")) {
            muon->auxdecor<bool>("passesPromptCuts") = inMuonsUncalibrated->at(muon->index())->auxdecor<bool>("passesPromptCuts");
            muon->auxdecor<bool>("passesDisplacedCuts") = inMuonsUncalibrated->at(muon->index())->auxdecor<bool>("passesDisplacedCuts");
        }

        float chi2;
        if (not muon->parameter(chi2, xAOD::Muon::msInnerMatchChi2))
            chi2 = -1;
        muon->auxdecor<float>("chi2") = chi2;

        int msInnerMatchDOF;
        if (not muon->parameter(msInnerMatchDOF, xAOD::Muon::msInnerMatchDOF))
            msInnerMatchDOF = -1;
        muon->auxdecor<int>("msDOF") = msInnerMatchDOF;
 
        // DT commented out for SUSY15 
        // muon->auxdecor<bool>("isLRT") = muon->primaryTrackParticle()->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0);

//        ANA_CHECK(calculateIsolation(tracks, muon));

    }

    for (const xAOD::Electron *electron : *inElectrons) {
        electron->auxdecor<int>("index") = electron->index();
        electron->auxdecor<float>("px") = electron->p4().Px() / GeV;
        electron->auxdecor<float>("py") = electron->p4().Py() / GeV;
        electron->auxdecor<float>("pz") = electron->p4().Pz() / GeV;
        if (not(m_inElContainerName == "Electrons")) {
            electron->auxdecor<bool>("passesPromptCuts") = inElectronsUncalibrated->at(electron->index())->auxdecor<bool>("passesPromptCuts");
            electron->auxdecor<bool>("passesDisplacedCuts") = inElectronsUncalibrated->at(electron->index())->auxdecor<bool>("passesDisplacedCuts");
        }
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
    if (primaryVertex) {
        eventInfo->auxdecor<float>("PV_x") = primaryVertex->x();
        eventInfo->auxdecor<float>("PV_y") = primaryVertex->y();
        eventInfo->auxdecor<float>("PV_z") = primaryVertex->z();
    }

    return EL::StatusCode::SUCCESS;
}

StatusCode DHNLAlgorithm::calculateIsolation(const xAOD::TrackParticleContainer *tracks, const xAOD::Muon *efmuon) const {

    if (m_debug)
        msg() << MSG::DEBUG << "Running isolation over EF Muon!" << endmsg;

//    // clear vectors
//    results.clear();
//    dzvals->clear();
//    drvals->clear();
//    selfremoval->clear();
    bool m_removeSelf = true;
    bool selfpt_isLRT = false;

    // this will point to the muon (combined or extrapolated)
    const xAOD::TrackParticle* muon=0;

    // variable to store pt of the muon (this is different for different muon types so we pass it separately)
    double selfpt=0.0;

    //for combined muons use the associated ID track for the self removal
    const xAOD::TrackParticle* muon_idtrk = 0;
//    const Trk::Perigee* muidtrk_perigee = 0;
    if( efmuon->combinedTrackParticleLink().isValid()  ) {
        if(m_debug) {
            msg() << MSG::DEBUG << "EF muon has combined muon" << endmsg;
        }
        muon = *(efmuon->combinedTrackParticleLink());
        if( efmuon->inDetTrackParticleLink().isValid() ) {
            muon_idtrk = *(efmuon->inDetTrackParticleLink());
//            muidtrk_perigee = &(muon_idtrk->perigeeParameters());
            selfpt = muon_idtrk->pt();
            selfpt_isLRT = muon_idtrk->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0);
            if(m_debug) {
                msg() << MSG::DEBUG << "Found ID track attached to combined muon, " << muon_idtrk << ",pt = " << selfpt << endmsg;
            }
        }
        else {
            msg() << MSG::WARNING << "Found EF combined muon without a link to ID track, will use combined pt for self removal" << endmsg;
            selfpt = muon->pt();
            selfpt_isLRT = muon->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0);
        }
    }//combinedmuon
    else {
        // for extrapolated muons use the extrapolated muon for self removal
        if( !efmuon->muonSpectrometerTrackParticleLink().isValid() ) {
            if(m_removeSelf) {
                msg() << MSG::WARNING << "This EF muon has neither a combined or extrapolated muon and removeSelf is requested, do not process further" << endmsg;
//                return StatusCode::FAILURE;
            }//m_removeSelf
            msg() << MSG::WARNING << "This EF muon has neither a combined, extrapolated or MS muon, do not process further" << endmsg;
//            return StatusCode::FAILURE;
        }
        else {
            muon = *(efmuon->muonSpectrometerTrackParticleLink());
            selfpt_isLRT = muon->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0);
        }//extrapmuon
    }//not combined



    /// consistency checks
    if (selfpt == 0.0) {
        msg() << MSG::WARNING << "Inconsistency, removeSelf requested, but selfpt = 0" << endmsg;
    }
    if (!muon) {
        msg() << MSG::WARNING << "Could not find a muon to update with the isolation, skipping this muon" << endmsg;
        efmuon->auxdecor<float>("myptcone30") = 0;
        efmuon->auxdecor<float>("myptcone30noLRT") = 0;
        return StatusCode::SUCCESS;
    }


    // Calculate isolation by hand
    float myptcone30 = 0;
    float myptcone30noLRT = 0;
    for (const xAOD::TrackParticle *track : *tracks) {
        // Snippet from
        // https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/Trigger/TrigAlgorithms/TrigMuonEF/src/TrigMuonEFTrackIsolationTool.cxx?v=21.0
        // check if trk within cone
        double dr = 0;
        if (muon_idtrk) { //use ID track for dR if available
            dr = track->p4().DeltaR(muon_idtrk->p4());
        } else { //use the muon
            dr = track->p4().DeltaR(muon->p4());
        }

        // If the current track is within a dR cone of 0.3 AND is not the same track as the muon primary track
        if (dr < 0.3) {
            ANA_MSG_DEBUG("dr: " << dr << "  pt: " << track->pt());
            myptcone30 += track->pt();
            // Calculate the isolation without using large-radius tracks
            if (not track->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0)) {
                myptcone30noLRT += track->pt();
            }
        }
    }

    myptcone30 -= selfpt;
    if (not selfpt_isLRT) myptcone30noLRT -= selfpt;

    efmuon->auxdecor<float>("myptcone30") = myptcone30;
    efmuon->auxdecor<float>("myptcone30noLRT") = myptcone30noLRT;

    return StatusCode::SUCCESS;
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

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
#include "xAODMetaData/FileMetaData.h"

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

    m_allJetContainerName = "AntiKt4EMTopoJets";
    m_inMuContainerName = "Muons";
    m_inElContainerName = "Electrons";
    // Muons
    // Prompt muons
    m_mu1PtMin = 28.0 * GeV;
    m_mu1AbsEtaMax = 2.5;
    m_mu1Types = {xAOD::Muon::Combined};
    m_mu1IsoType = xAOD::Iso::ptcone30;
    m_mu1IsoCutIsRel = true; // "Cut is on relative isolation"
    m_mu1IsoCut = 0.05;
    // Displaced muons
    m_mu2PtMin = 5.0 * GeV;
    m_mu2AbsEtaMax = 2.5;
    m_mu2Types = {xAOD::Muon::Combined, xAOD::Muon::MuonStandAlone};
    m_mu2IsoType = xAOD::Iso::ptcone30;
    m_mu2IsoCutIsRel = true; // "Cut is on relative isolation"
    m_mu2IsoCut = 9999.;
    m_mu2d0Min = 0.1; // "Unit is mm"

    // Electrons
    // Prompt electrons
    m_el1PtMin = 28.0 * GeV;
    m_el1AbsEtaMax = 2.5;
    m_el1IDKey = "LHLoose";
    m_el1IsoType = xAOD::Iso::ptcone30;
    m_el1IsoCutIsRel = true; // "Cut is on relative isolation"
    m_el1IsoCut = 0.05;
    // Displaced electrons
    m_el2PtMin = 5.0 * GeV;
    m_el2AbsEtaMax = 2.5;
    m_el2IsoType = xAOD::Iso::ptcone30;
    m_el2IsoCutIsRel = true; // "Cut is on relative isolation"
    m_el2IsoCut = 9999.;
    m_el2d0Min = 1.0; // "Unit is mm"

    m_dPhiMin = 0.0; // "Unit is radian"

    // VH4b
    m_msgLevel = MSG::INFO;
    m_TrackMinPt = 400;
    m_TrackZ0Max = 0.3;
    m_TrackD0Max = 0.5;
    m_jetPtCut = 20;
    m_AlphaMaxCut = 0.03;
    m_CHFCut = 0.3;
    m_electronPtCut = 27000;
    m_muonPtCut = 25000;
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
    //cout<<"DHNL execute"<<endl;

    ANA_MSG_DEBUG("execute(): Applying selection");

    ++m_eventCounter;

    bool passesFilter = applyFilter();
    ANA_MSG_DEBUG("execute() : passesFilter:  " << passesFilter);

    // bool passesVH4bFilter = applyVH4bFilter();
    // ANA_MSG_DEBUG("execute() : passesVH4bFilter:  " << passesVH4bFilter);

    return EL::StatusCode::SUCCESS;

}

// The filter itself
// adapted from https://gitlab.cern.ch/atlas/athena/blob/21.0/PhysicsAnalysis/SUSYPhys/LongLivedParticleDPDMaker/src/HnlSkimmingTool.cxx
bool DHNLFilter::applyFilter() const {
    bool acceptEventMuMu = false;
    bool acceptEventElMu = false;
    bool acceptEventElEl = false;
    bool acceptEventMuEl = false;

//    // Trigger check
//    bool passedTrigger = true;
//    if (m_triggers.size() > 0) {
//        passedTrigger = false;
//        for (const std::string &trigger : m_triggers) {
//            bool decision = m_trigDecisionTool->isPassed(trigger);
//            if (decision) {
//                passedTrigger = true;
//                break;
//            }
//        }
//    }
//    if (not passedTrigger) return acceptEvent;

    ANA_MSG_DEBUG("execute() : Get Containers");
    const xAOD::EventInfo *eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));

    const xAOD::MuonContainer *muons = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(muons, m_inMuContainerName, m_event, m_store));

    const xAOD::ElectronContainer *electrons = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(electrons, m_inElContainerName, m_event, m_store));

    // Prompt leptons
    std::vector<const xAOD::Muon *> promptMuonCandidates;
    std::vector<const xAOD::Electron *> promptElectronCandidates;
    getPromptMuonCandidates(muons, promptMuonCandidates);
//    if (promptMuonCandidates.empty()) return acceptEvent;
    getPromptElectronCandidates(electrons, promptElectronCandidates);
//    if (promptElectronCandidates.empty()) return acceptEvent;

    // Displaced leptons
    std::vector<const xAOD::Muon *> displacedMuonCandidates;
    std::vector<const xAOD::Electron *> displacedElectronCandidates;
    getDisplacedMuonCandidates(muons, displacedMuonCandidates);
//    if (displacedMuonCandidates.empty()) return acceptEvent;
    getDisplacedElectronCandidates(electrons, displacedElectronCandidates);
//    if (displacedElectronCandidates.empty()) return acceptEvent;

    // Final check: at least one pair of a prompt lepton candidate and
    // a displaced lepton candidate, which is different from the prompt lepton candidate

    // mu-mu
    for (const xAOD::Muon *promptMuonCandidate : promptMuonCandidates) {
        for (const xAOD::Muon *displacedMuonCandidate : displacedMuonCandidates) {
            if (promptMuonCandidate != displacedMuonCandidate) {
                double dPhi = promptMuonCandidate->phi() - displacedMuonCandidate->phi();
                while (dPhi >= +M_PI) dPhi -= 2. * M_PI;
                while (dPhi <= -M_PI) dPhi += 2. * M_PI;
                dPhi = std::abs(dPhi);
                if (dPhi >= m_dPhiMin) {
                    acceptEventMuMu = true;
                    ANA_MSG_DEBUG("applyFilter() : event passes Mu Mu Filter");
                    break;
                }
            }
        }
        if (acceptEventMuMu) break; // no need for more checks
    }

    // mu-e
    for (const xAOD::Muon *promptMuonCandidate : promptMuonCandidates) {
        for (const xAOD::Electron *displacedElectronCandidate : displacedElectronCandidates) {
            double dPhi = promptMuonCandidate->phi() - displacedElectronCandidate->phi();
            while (dPhi >= +M_PI) dPhi -= 2. * M_PI;
            while (dPhi <= -M_PI) dPhi += 2. * M_PI;
            dPhi = std::abs(dPhi);
            if (dPhi >= m_dPhiMin) {
                acceptEventMuEl = true;
                ANA_MSG_DEBUG("applyFilter() : event passes Mu El Filter");
                break;
            }
        }
        if (acceptEventMuEl) break; // no need for more checks
    }

    // e-mu
    for (const xAOD::Electron *promptElectronCandidate : promptElectronCandidates) {
        for (const xAOD::Muon *displacedMuonCandidate : displacedMuonCandidates) {
            double dPhi = promptElectronCandidate->phi() - displacedMuonCandidate->phi();
            while (dPhi >= +M_PI) dPhi -= 2. * M_PI;
            while (dPhi <= -M_PI) dPhi += 2. * M_PI;
            dPhi = std::abs(dPhi);
            if (dPhi >= m_dPhiMin) {
                acceptEventElMu = true;
                ANA_MSG_DEBUG("applyFilter() : event passes El Mu Filter");
                break;
            }
        }
        if (acceptEventElMu) break; // no need for more checks
    }

    // e-e
    for (const xAOD::Electron *promptElectronCandidate : promptElectronCandidates) {
        for (const xAOD::Electron *displacedElectronCandidate : displacedElectronCandidates) {
            if (promptElectronCandidate != displacedElectronCandidate) {
                double dPhi = promptElectronCandidate->phi() - displacedElectronCandidate->phi();
                while (dPhi >= +M_PI) dPhi -= 2. * M_PI;
                while (dPhi <= -M_PI) dPhi += 2. * M_PI;
                dPhi = std::abs(dPhi);
                if (dPhi >= m_dPhiMin) {
                    acceptEventElEl = true;
                    ANA_MSG_DEBUG("applyFilter() : event passes El El Filter");
                    break;
                }
            }
        }
        if (acceptEventElEl) break; // no need for more checks
    }

    // Store decision for each filter
    eventInfo->auxdecor<bool>("passesHnlMuMuFilter") = acceptEventMuMu;
    eventInfo->auxdecor<bool>("passesHnlElMuFilter") = acceptEventElMu;
    eventInfo->auxdecor<bool>("passesHnlElElFilter") = acceptEventElEl;
    eventInfo->auxdecor<bool>("passesHnlMuElFilter") = acceptEventMuEl;
    ANA_MSG_DEBUG("applyFilter() : filter decisions stored");

    return acceptEventMuMu || acceptEventElMu || acceptEventElEl || acceptEventMuEl;
}

void DHNLFilter::getPromptMuonCandidates(const xAOD::MuonContainer *muons,
                                         std::vector<const xAOD::Muon *> &promptMuonCandidates) const {
    for (const xAOD::Muon *muon : *muons) {
        // pT cut
        if (not(muon->pt() > m_mu1PtMin)) continue;

        // eta cut
        if (not(std::abs(muon->eta()) < m_mu1AbsEtaMax)) continue;

        // type cut
        bool passTypeCut = true;
        if (m_mu1Types.size() > 0) {
            passTypeCut = false;
            int type = muon->muonType();
            for (const int &allowedType : m_mu1Types) {
                if (allowedType == type) {
                    passTypeCut = true;
                    break;
                }
            }
        }
        if (not passTypeCut) continue;

//        // ignore LRT muons as these are reconstructed after the DRAW filter is run. Only applies to combined muons.
//        if (muon->muonType() == xAOD::Muon::Combined)
//            if (muon->primaryTrackParticle()->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0))
//                continue;

        // isolation cut
        bool isIso = false;
        float isoValue = 0.;
        const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu1IsoType);
        if (not m_mu1IsoCutIsRel) {
            if (muon->isolation(isoValue, isoType) and (isoValue < m_mu1IsoCut)) isIso = true;
        } else {
            if (muon->isolation(isoValue, isoType) and (isoValue / muon->pt() < m_mu1IsoCut)) isIso = true;
            if (not isIso) isIso = attemptIsoRecovery(muon, isoValue, m_mu1IsoCut);
        }
        if (not isIso) continue;

        muon->auxdecor<bool>("passesPromptCuts") = true;
        promptMuonCandidates.push_back(muon);
    }
}

void DHNLFilter::getDisplacedMuonCandidates(const xAOD::MuonContainer *muons, std::vector<const xAOD::Muon *> &displacedMuonCandidates) const {
    for (const xAOD::Muon *muon : *muons) {
        // pT cut
        if (not(muon->pt() > m_mu2PtMin)) continue;

        // eta cut
        if (not(std::abs(muon->eta()) < m_mu2AbsEtaMax)) continue;

        // type cut
        bool passTypeCut = true;
        int type = muon->muonType();
        if (m_mu2Types.size() > 0) {
            passTypeCut = false;
            for (const int &allowedType : m_mu2Types) {
                if (type == allowedType) {
                    passTypeCut = true;
                    break;
                }
            }
        }
        if (not passTypeCut) continue;

//        // ignore LRT muons as these are reconstructed after the DRAW filter is run. Only applies to combined muons.
//        if (type == xAOD::Muon::Combined)
//            if (muon->primaryTrackParticle()->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0))
//                continue;

        // isolation cut
        bool isIso = false;
        float isoValue = 0.;
        const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu2IsoType);
        if (not m_mu2IsoCutIsRel) {
            if (muon->isolation(isoValue, isoType) and (isoValue < m_mu2IsoCut)) isIso = true;
        } else {
            if (muon->isolation(isoValue, isoType) and isoValue / muon->pt() < m_mu2IsoCut) isIso = true;
        }
        if (not isIso) continue;

        // d0 cut
        bool passD0cut = true;
        if (type == xAOD::Muon::Combined) { // d0 cut is for combined muons only
            passD0cut = false;
            if (isGood(*muon)) { // if muon has a good chi2/dof
                if (std::abs(muon->primaryTrackParticle()->d0()) > m_mu2d0Min) passD0cut = true;
            } else {
                passD0cut = true;
            }
        }
        if (not passD0cut) continue;

        muon->auxdecor<bool>("passesDisplacedCuts") = true;
        displacedMuonCandidates.push_back(muon);
    }
}

void DHNLFilter::getPromptElectronCandidates(const xAOD::ElectronContainer *electrons,
                                             std::vector<const xAOD::Electron *> &promptElectronCandidates) const {
    for (const xAOD::Electron *electron : *electrons) {
        // pT cut
        if (not(electron->pt() > m_el1PtMin)) continue;

        // eta cut
        if (not(std::abs(electron->eta()) < m_el1AbsEtaMax)) continue;

        // eID cut
        bool passEID = false;
        if (not electron->passSelection(passEID, m_el1IDKey)) continue;
        if (not passEID) continue;

        // isolation cut
        bool isIso = false;
        float isoValue = 0.;
        const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_el1IsoType);
        if (not m_el1IsoCutIsRel) {
            if (electron->isolation(isoValue, isoType) and (isoValue < m_el1IsoCut)) isIso = true;
        } else {
            if (electron->isolation(isoValue, isoType) and (isoValue / electron->pt() < m_el1IsoCut)) isIso = true;
        }
        if (not isIso) continue;

        electron->auxdecor<bool>("passesPromptCuts") = true;
        promptElectronCandidates.push_back(electron);
    }
}

void DHNLFilter::getDisplacedElectronCandidates(const xAOD::ElectronContainer *electrons,
                                                std::vector<const xAOD::Electron *> &displacedElectronCandidates) const {
    for (const xAOD::Electron *electron : *electrons) {
        // pT cut
        if (not(electron->pt() > m_el2PtMin)) continue;

        // eta cut
        if (not(std::abs(electron->eta()) < m_el2AbsEtaMax)) continue;

        // isolation cut
        bool isIso = false;
        float isoValue = 0.;
        const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_el2IsoType);
        if (not m_el2IsoCutIsRel) {
            if (electron->isolation(isoValue, isoType) and (isoValue < m_el2IsoCut)) isIso = true;
        } else {
            if (electron->isolation(isoValue, isoType) and isoValue / electron->pt() < m_el2IsoCut) isIso = true;
        }
        if (not isIso) continue;

        // d0 cut
        bool passD0cut = false;
        if (std::abs(electron->trackParticle()->d0()) > m_el2d0Min) passD0cut = true;
        if (not passD0cut) continue;

        electron->auxdecor<bool>("passesDisplacedCuts") = true;
        displacedElectronCandidates.push_back(electron);
    }
}

// Check for the chi2 cut
bool DHNLFilter::isGood(const xAOD::Muon &mu) const {
    if (mu.muonType() != xAOD::Muon::Combined) return false;

    float chi2 = 0.;
    if (not mu.parameter(chi2, xAOD::Muon::msInnerMatchChi2)) return false;

    int dof = 1;
    if (not mu.parameter(dof, xAOD::Muon::msInnerMatchDOF)) return false;
    if (dof == 0) dof = 1;

    return ((chi2 / static_cast<float>(dof)) < 5.);
}

// VH4b filter implementation
bool DHNLFilter::applyVH4bFilter() {

    ANA_MSG_DEBUG("execute() : Get Containers");
    const xAOD::EventInfo *eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));

    const xAOD::VertexContainer *vertices = 0;
    ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));

    const xAOD::MuonContainer *allMuons = 0;
    const xAOD::ElectronContainer *allElectrons = 0;
    ANA_CHECK (HelperFunctions::retrieve(allMuons, m_inMuContainerName, m_event, m_store));
    ANA_CHECK (HelperFunctions::retrieve(allElectrons, m_inElContainerName, m_event, m_store));

    const xAOD::JetContainer *allJets = 0;
    ANA_CHECK (HelperFunctions::retrieve(allJets, m_allJetContainerName, m_event, m_store));

//    pass = this->executeFilter( eventInfo, allJets, allMuons, allElectrons, vertices);
    /////////////////////////// Begin Selections  ///////////////////////////////

    //int evtNum = eventInfo->eventNumber();

    bool passesFilter = false;
    bool passesJet = false;
    bool passesEl = false;
    bool passesMu = false;


    // New Containers for Jets, Electrons and Muons

    newJetContainers(allJets, allElectrons);
    const xAOD::JetContainer *good_jets;
    const xAOD::JetContainer *selected_jets;
    const xAOD::JetContainer *signal_jets;
    ANA_CHECK (HelperFunctions::retrieve(good_jets, "good_jets", m_event, m_store));
    ANA_CHECK (HelperFunctions::retrieve(selected_jets, "selected_jets", m_event, m_store));
    ANA_CHECK (HelperFunctions::retrieve(signal_jets, "signal_jets", m_event, m_store));

    newElectronContainers(allElectrons, eventInfo, vertices);

    const xAOD::ElectronContainer *good_electrons;
    const xAOD::ElectronContainer *selected_electrons;
    ANA_CHECK (HelperFunctions::retrieve(good_electrons, "good_electrons", m_event, m_store));
    ANA_CHECK (HelperFunctions::retrieve(selected_electrons, "selected_electrons", m_event, m_store));

    newMuonContainers(allMuons, eventInfo, vertices);

    const xAOD::MuonContainer *good_muons;
    const xAOD::MuonContainer *selected_muons;
    ANA_CHECK (HelperFunctions::retrieve(good_muons, "good_muons", m_event, m_store));
    ANA_CHECK (HelperFunctions::retrieve(selected_muons, "selected_muons", m_event, m_store));

    if (selected_electrons->size() > 0) { passesEl = true; }
    if (selected_muons->size() > 0) { passesMu = true; }


    int nJets = 0;


    //Calculating alpha max and charged hadron fraction
    int j = 0;
    for (auto jet : *signal_jets) {
        j++;
        TLorentzVector VJet = TLorentzVector(0.0, 0.0, 0.0, 0.0);
        VJet.SetPtEtaPhiE(jet->pt(), jet->eta(), jet->phi(), jet->e());

        TLorentzVector CHFNum = TLorentzVector(0.0, 0.0, 0.0, 0.0);

        int jetIndex = j - 1;

//        Particles associated_tracks = btagAssociatedTracks(jet, jetIndex);
        // Copying this function to in-line definition
        const xAOD::BTagging *bjet(nullptr);
        bjet = jet->btagging();

        TrackLinks assocTracks = bjet->auxdata<TrackLinks>("BTagTrackToJetAssociator");
        Particles selectedTracks;

        for (unsigned int iT = 0; iT < assocTracks.size(); iT++) {
            if (!assocTracks.at(iT).isValid()) continue;
            else {
                const xAOD::TrackParticle *tmpTrk = *(assocTracks.at(iT));
                tmpTrk->auxdecor<int>("isAssoc") = 1;
                tmpTrk->auxdecor<int>("jetIndex") = jetIndex;
                selectedTracks.push_back(tmpTrk);
            }
        }


        Particles tracks(selectedTracks.begin(), selectedTracks.end());
        // end in-line function
        Particles associated_tracks = tracks;

        float alpha_max = -9;
        int i = 0;

        for (auto vertex : *vertices) {
            TLorentzVector alphaDen = TLorentzVector(0.0, 0.0, 0.0, 0.0);
            TLorentzVector alphaNum = TLorentzVector(0.0, 0.0, 0.0, 0.0);
            float alpha;
            for (auto track : associated_tracks) {
                if (track->pt() < m_TrackMinPt) continue;
                // skip over LRTs
                const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = track->patternRecoInfo();
                if (patternReco.test(49)) continue;
                TLorentzVector VTrack = TLorentzVector(0.0, 0.0, 0.0, 0.0);
                VTrack.SetPtEtaPhiE(track->pt(), track->eta(), track->phi(), track->e());
                alphaDen = alphaDen + VTrack;
                if (track->d0() > m_TrackD0Max) continue;
                float z0 = track->z0() + track->vz() - vertex->z();
                float theta = track->theta();
                if (fabs(z0 * sin(theta)) < m_TrackZ0Max) {
                    alphaNum = alphaNum + VTrack;
                }
            }
            if (alphaDen.Pt() == 0) { alpha = -99; }
            else {
                alpha = alphaNum.Pt() / alphaDen.Pt();
            }
            if (alpha > alpha_max) { alpha_max = alpha; }
            i++;
        }

        for (auto track : associated_tracks) {
            TLorentzVector VTrack = TLorentzVector(0.0, 0.0, 0.0, 0.0);
            if (track->d0() > m_TrackD0Max) continue;
            // skip over LRTs
            const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = track->patternRecoInfo();
            if (patternReco.test(49)) continue;
            if (track->pt() < m_TrackMinPt) continue;
            VTrack.SetPtEtaPhiE(track->pt(), track->eta(), track->phi(), track->e());
            CHFNum = CHFNum + VTrack;
        }

        float chf = CHFNum.Pt() / VJet.Pt();
        jet->auxdecor<float>("chf") = chf;
        jet->auxdecor<float>("alpha_max") = alpha_max;
        jet->auxdecor<int>("alpha_max_pass") = (alpha_max < m_AlphaMaxCut) * 1;
        jet->auxdecor<int>("chf_pass") = (chf < m_CHFCut) * 1;

        if (((chf < m_CHFCut) || (alpha_max < m_AlphaMaxCut)) && j <= 2) { nJets++; }

    }
    if (nJets > 0) { passesJet = true; }


    eventInfo->auxdecor<bool>("passesMuonFilterVH4b") = passesMu;
    eventInfo->auxdecor<bool>("passesElecFilterVH4b") = passesEl;

    if (passesJet && (passesEl || passesMu)) {
        passesFilter = true;
    }
    eventInfo->auxdecor<bool>("passesVH4bFilter") = passesFilter;

    return passesFilter;
}

void DHNLFilter::newJetContainers(const xAOD::JetContainer *input, const xAOD::ElectronContainer *electrons) {

    ConstDataVector<xAOD::JetContainer> *goodJets(nullptr);
    ConstDataVector<xAOD::JetContainer> *selectedJets(nullptr);
    ConstDataVector<xAOD::JetContainer> *signalJets(nullptr);
    goodJets = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
    selectedJets = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);
    signalJets = new ConstDataVector<xAOD::JetContainer>(SG::VIEW_ELEMENTS);

    for (auto jet: *input) {
        int good = 0;
        int selected = 0;
        int signal = 0;
        TLorentzVector VJet = jet->p4();
        float minDR = 100;

        for (auto elec: *electrons) {

            if (elec->pt() < 20000) continue;
            if (fabs(elec->eta()) > 2.47) continue;
            bool passLoose = false;
            if (!elec->passSelection(passLoose, "Loose")) { continue; }
            if (!passLoose) continue;

            TLorentzVector VElectron = elec->p4();
            float DR = VElectron.DeltaR(VJet);

            if (DR < minDR) { minDR = DR; }
        }
        if (minDR > 0.2) {
            goodJets->push_back(jet);
            good = 1;

            if (jet->pt() > 20000) {

                if (fabs(jet->eta()) < 2.8) {
                    selectedJets->push_back(jet);
                    selected = 1;
                }

                if (fabs(jet->eta()) < 2.1) {
                    signalJets->push_back(jet);
                    signal = 1;
                }
            }
        }
        jet->auxdecor<int>("good_jet") = good;
        jet->auxdecor<int>("selected_jet") = selected;
        jet->auxdecor<int>("signal_jet") = signal;
    }
    m_store->record(goodJets, "good_jets");
    m_store->record(selectedJets, "selected_jets");
    m_store->record(signalJets, "signal_jets");
}

void DHNLFilter::newElectronContainers(const xAOD::ElectronContainer *input, const xAOD::EventInfo *eventInfo, const xAOD::VertexContainer *vertices) {

    ConstDataVector<xAOD::ElectronContainer> *goodElectrons(nullptr);
    ConstDataVector<xAOD::ElectronContainer> *selectedElectrons(nullptr);
    goodElectrons = new ConstDataVector<xAOD::ElectronContainer>(SG::VIEW_ELEMENTS);
    selectedElectrons = new ConstDataVector<xAOD::ElectronContainer>(SG::VIEW_ELEMENTS);

    for (auto elec: *input) {
        int good = 0;
        int selected = 0;

        if (elec->passSelection(m_electronLHWP.c_str()) && elec->isolation(xAOD::Iso::topoetcone20) / elec->pt() < 0.2) {
            goodElectrons->push_back(elec);
            good = 1;

            for (xAOD::VertexContainer::const_iterator vxIter2 = vertices->begin(); vxIter2 != vertices->end(); ++vxIter2) {
                // Select good primary vertex
                if ((*vxIter2)->vertexType() == xAOD::VxType::PriVtx) {
                    const xAOD::TrackParticle *tp = elec->trackParticle(); //your input track particle from the electron
                    float sigd0 = fabs(xAOD::TrackingHelpers::d0significance(tp, eventInfo->beamPosSigmaX(), eventInfo->beamPosSigmaY(), eventInfo->beamPosSigmaXY()));
                    double delta_z0 = tp->z0() + tp->vz() - (*vxIter2)->z();
                    double theta = tp->theta();
                    float z0sintheta = fabs(delta_z0 * sin(theta));

                    if ((fabs(elec->caloCluster()->etaBE(2)) > 1.37 && fabs(elec->caloCluster()->etaBE(2) < 1.52)) || fabs(elec->caloCluster()->etaBE(2)) > 2.47) continue;

                    if ((sigd0 < 5 && z0sintheta < 0.5 && elec->pt() > m_electronPtCut)) {
                        selectedElectrons->push_back(elec);
                        selected = 1;
                    }
                }
            }
        }
        elec->auxdecor<int>("good_electron") = good;
        elec->auxdecor<int>("selected_electron") = selected;
    }
    m_store->record(goodElectrons, "good_electrons");
    m_store->record(selectedElectrons, "selected_electrons");
}

void DHNLFilter::newMuonContainers(const xAOD::MuonContainer *input, const xAOD::EventInfo *eventInfo, const xAOD::VertexContainer *vertices) {

    ConstDataVector<xAOD::MuonContainer> *goodMuons(nullptr);
    ConstDataVector<xAOD::MuonContainer> *selectedMuons(nullptr);
    goodMuons = new ConstDataVector<xAOD::MuonContainer>(SG::VIEW_ELEMENTS);
    selectedMuons = new ConstDataVector<xAOD::MuonContainer>(SG::VIEW_ELEMENTS);

    for (auto muon: *input) {
        int good = 0;
        int selected = 0;

        if (!m_muonSelectionTool_handle->passedMuonCuts(*muon)) continue;
        if (muon->muonType() != xAOD::Muon::Combined) continue;
        if (muon->isolation(xAOD::Iso::topoetcone20) / muon->pt() < 0.3) {
            goodMuons->push_back(muon);
            good = 1;
            for (xAOD::VertexContainer::const_iterator vxIter3 = vertices->begin(); vxIter3 != vertices->end(); ++vxIter3) {
                // Select good primary vertex
                if ((*vxIter3)->vertexType() == xAOD::VxType::PriVtx) {
                    const xAOD::TrackParticle *tp = muon->primaryTrackParticle(); //your input track particle from the muon
                    double d0sig = xAOD::TrackingHelpers::d0significance(tp, eventInfo->beamPosSigmaX(), eventInfo->beamPosSigmaY(), eventInfo->beamPosSigmaXY());
                    float delta_z0 = tp->z0() + tp->vz() - (*vxIter3)->z();
                    float theta = tp->theta();
                    double z0sintheta = delta_z0 * sin(theta);

                    if (muon->pt() > m_muonPtCut && fabs(muon->eta()) < 2.5 && fabs(d0sig) < 3 && fabs(z0sintheta) < 0.5) {
                        selectedMuons->push_back(muon);
                        selected = 1;
                    }
                }
            }
        }
        muon->auxdecor<int>("good_muon") = good;
        muon->auxdecor<int>("selected_muon") = selected;
    }
    m_store->record(goodMuons, "good_muons");
    m_store->record(selectedMuons, "selected_muons");
}
// End VH4b filter implementation




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

bool DHNLFilter::attemptIsoRecovery(const xAOD::Muon *muon, float isoValue, float isoCut) const {
    const xAOD::TrackParticleContainer *tracks = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(tracks, "InDetTrackParticles", m_event, m_store));
    const xAOD::TrackParticle *muon_idtrk = *(muon->inDetTrackParticleLink());

    for (const xAOD::TrackParticle *track : *tracks) {
        double dr = 0;
        if (muon_idtrk) { //use ID track for dR if available
            dr = track->p4().DeltaR(muon_idtrk->p4());
        } else { //use the muon
            dr = track->p4().DeltaR(muon->p4());
        }

        if (dr < 0.3) {
            if (track->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0)) {
                ANA_MSG_DEBUG("subtracting track pt " << track->pt() << " from isoValue" << isoValue);
                isoValue -= track->pt();
            }
        }
    }

    float ptc30pt = isoValue / muon->pt();
    if (ptc30pt < isoCut) {
        ANA_MSG_DEBUG("isolation cut recovered subtracing LRT from isolation");
        return true;
    } else return false;
}

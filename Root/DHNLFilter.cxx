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
    m_mu2Types = {xAOD::Muon::Combined, xAOD::Muon::MuonStandAlone, xAOD::Muon::SegmentTagged};
    m_mu2IsoType = xAOD::Iso::ptcone30;
    m_mu2IsoCutIsRel = true; // "Cut is on relative isolation"
    m_mu2IsoCut = 1.;
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
    m_el2IsoCut = 1.;
    m_el2d0Min = 1.0; // "Unit is mm"

    m_dPhiMin = 0.0; // "Unit is radian"


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
    //cout<<"DHNL execute"<<endl;

    ANA_MSG_DEBUG("execute(): Applying selection");

    ++m_eventCounter;

    bool passesFilter = applyFilter();
    ANA_MSG_DEBUG("execute() : passesFilter:  " << passesFilter);

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

        // isolation cut
        bool isIso = false;
        float isoValue = 0.;
        const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(m_mu1IsoType);
        if (not m_mu1IsoCutIsRel) {
            if (muon->isolation(isoValue, isoType) and (isoValue < m_mu1IsoCut)) isIso = true;
        } else {
            if (muon->isolation(isoValue, isoType) and (isoValue / muon->pt() < m_mu1IsoCut)) isIso = true;
        }
        if (not isIso) continue;

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

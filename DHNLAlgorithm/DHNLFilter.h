#ifndef DHNL_DHNLFILTER_H
#define DHNL_DHNLFILTER_H

// Framework include(s)
#include <EventLoop/StatusCode.h>
#include <EventLoop/Algorithm.h>

//algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include <xAODAnaHelpers/JetHists.h>
#include "xAODTracking/TrackParticleContainer.h"

#include "AsgTools/AnaToolHandle.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"

class DHNLFilter : public xAH::Algorithm {

public:
    //configuration variables
    std::string m_allJetContainerName;// input container name
    std::string m_inMuContainerName;    // input Muon container name
    std::string m_inElContainerName;    // input Electron container name

    // Muons
//    std::string m_muonSGKey;
    // Prompt muons
    float m_mu1PtMin;
    float m_mu1AbsEtaMax;
    std::vector<int> m_mu1Types;
    int m_mu1IsoType; //http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODPrimitives/xAODPrimitives/IsolationType.h
    bool m_mu1IsoCutIsRel; //is the isolation cut relative or absolute
    float m_mu1IsoCut; //cut value
    // Displaced muons
    float m_mu2PtMin;
    float m_mu2AbsEtaMax;
    std::vector<int> m_mu2Types;
    int m_mu2IsoType;
    bool m_mu2IsoCutIsRel;
    float m_mu2IsoCut;
    float m_mu2d0Min;

    // Electrons
//    std::string m_electronSGKey;
    // Prompt electrons
    float m_el1PtMin;
    float m_el1AbsEtaMax;
    std::string m_el1IDKey;
    int m_el1IsoType; //http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Event/xAOD/xAODPrimitives/xAODPrimitives/IsolationType.h
    bool m_el1IsoCutIsRel; //is the isolation cut relative or absolute
    float m_el1IsoCut; //cut value
    // Displaced electrons
    float m_el2PtMin;
    float m_el2AbsEtaMax;
    int m_el2IsoType;
    bool m_el2IsoCutIsRel;
    float m_el2IsoCut;
    float m_el2d0Min;

    float m_dPhiMin;


    float m_TrackMinPt;              //minimum track pt for jet_alpha_max and jet_chf
    float m_TrackZ0Max;              //maximum z0sintheta for jet_alpha_max
    float m_TrackD0Max;              //maximum d0 for jet_alpha_max and jet_chf
    float m_jetPtCut;
    float m_AlphaMaxCut;
    float m_CHFCut;
    float m_electronPtCut;
    float m_muonPtCut;

private:
    int m_eventCounter;     //!
//    asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool_handle{"CP::MuonSelectionTool/MuonSelectionTool", this};

//    // Prompt muon, displaced muon
//    bool HnlMuMuFilter(const xAOD::MuonContainer *allMuons);
//
//    // Prompt electron, displaced muon
//    bool HnlElMuFilter(const xAOD::MuonContainer *allMuons, const xAOD::ElectronContainer *allElectrons);
//
//    // Prompt electron, displaced electron
//    bool HnlElElFilter(const xAOD::ElectronContainer *allElectrons);
//
//    // Prompt muon, displaced electron
//    bool HnlMuElFilter(const xAOD::MuonContainer *allMuons, const xAOD::ElectronContainer *allElectrons);

    bool applyFilter() const;

    void getPromptMuonCandidates(const xAOD::MuonContainer *muons, std::vector<const xAOD::Muon *> &promptMuonCandidates) const;

    void getDisplacedMuonCandidates(const xAOD::MuonContainer *muons, std::vector<const xAOD::Muon *> &displacedMuonCandidates) const;

    void getPromptElectronCandidates(const xAOD::ElectronContainer *electrons, std::vector<const xAOD::Electron *> &promptElectronCandidates) const;

    void getDisplacedElectronCandidates(const xAOD::ElectronContainer *electrons, std::vector<const xAOD::Electron *> &displacedElectronCandidates) const;

    bool isGood(const xAOD::Muon &mu) const;

public:
    // this is a standard constructor
    DHNLFilter();

    // these are the functions inherited from Algorithm
    virtual EL::StatusCode setupJob(EL::Job &job);

    virtual EL::StatusCode fileExecute();

    virtual EL::StatusCode histInitialize();

    virtual EL::StatusCode changeInput(bool firstFile);

    virtual EL::StatusCode initialize();

    virtual EL::StatusCode execute();

    virtual EL::StatusCode postExecute();

    virtual EL::StatusCode finalize();

    virtual EL::StatusCode histFinalize();

    // these are the functions not inherited from Algorithm
    virtual EL::StatusCode configure();


ClassDef(DHNLFilter, 1);

};

#endif //DHNL_DHNLFILTER_H

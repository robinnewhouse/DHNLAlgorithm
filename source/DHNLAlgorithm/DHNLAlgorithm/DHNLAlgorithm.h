#ifndef DHNL_DHNLALGORITHM_H
#define DHNL_DHNLALGORITHM_H

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
//#include "DHNLAlgorithm/DHNLMiniTree.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s):
#include "TH1D.h"

#include <sstream>

class DHNLAlgorithm : public xAH::Algorithm {

    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
public:

    //configuration variables
    std::string m_inJetContainerName;   // input Jet container name
    std::string m_inputAlgo;          // input algo for when running systs
    std::string m_allJetContainerName;// input container name
    std::string m_allJetInputAlgo;    // input algo for when running systs
    std::string m_inMETContainerName;  // input MET container name
    std::string m_inMETTrkContainerName;  // input METTrk container name
    std::string m_inMuContainerName;    // input Muon container name
    std::string m_inElContainerName;    // input Electron container name
    bool m_isMC;                      // Is MC
    bool m_useCutFlow;                // true will write out cutflow histograms
    std::string m_MCPileupCheckContainer; // Name of truth container for MC Pileup Check
    bool m_useMCPileupCheck;          // determined by name of MCPileupCheckContainer
    float m_leadingJetPtCut;          // Leading jet Pt cut
    float m_subleadingJetPtCut;          // Leading jet Pt cut
    uint m_jetMultiplicity;          // Leading jet Pt cut
    bool m_truthLevelOnly;            // truthLevelOnly info
    bool m_backgroundEstimationBranches; // do we add branches required for background estimation
    bool m_backgroundEstimationNoParticleData; // for the second step with fake event where we are missing a lot of data.
    float m_metCut;

private:

    TH1D *m_cutflowHist;    //!
    TH1D *m_cutflowHistW;   //!

public:
    // this is a standard constructor
    DHNLAlgorithm();

    EL::StatusCode initialize() override;

    EL::StatusCode execute() override;

    EL::StatusCode finalize() override;
    // this is needed to distribute the algorithm to the workers
ClassDef(DHNLAlgorithm, 1);

    const xAOD::Muon *matchTrackToMuon(const xAOD::TrackParticle *track, const xAOD::MuonContainer *inMuons);

    const xAOD::Electron *matchTrackToElectron(const xAOD::TrackParticle *track, const xAOD::ElectronContainer *inElectrons);

    StatusCode calculateIsolation(const xAOD::TrackParticleContainer *tracks, const xAOD::Muon *muon) const;
};


#endif //DHNL_DHNLALGORITHM_H

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
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "PMGAnalysisInterfaces/IPMGCrossSectionTool.h"

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
    std::string m_secondaryVertexContainerNameList;   // input Secondary Vertices container name
    std::string m_AugmentationVersionStringList; // string list of aug strings
    std::string m_inDetTrackParticlesContainerName; // Track container
    std::vector<std::string> m_secondaryVertexContainerNameKeys;  // list of the secondary vertex in a vector
    std::vector<std::string> m_AugmentationVersionStringKeys; // list of the aug strings in a vector
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
    bool m_doInverseLeptonControlRegion;            // do control region cuts
    bool m_doPromptLeptonCut;            // do signal region cuts
    bool m_fakeAOD; // running on fake AOD
    float m_metCut;
    bool m_doSkipTracks;
    std::string m_trackingCalibFile; // calibration file for tracking systmeatics

private:

    TH1D *m_cutflowHist;    //!
    TH1D *m_cutflowHistW;   //!
    StatusCode eventSelection();

    bool acceptTrack(const xAOD::TrackParticle &trk) const;
    float getFractionDropped(float fDefault, TH2 *histogram, float pt, float eta) const;

    asg::AnaToolHandle<PMGTools::IPMGCrossSectionTool> m_PMGCrossSectionTool_handle{"PMGCrossSectionTool", this}; //!
    float m_mcEventWeight;  //!
    std::string m_comEnergy; //!

    float m_weight;  //!
    float m_weight_xs;  //!

    TH2* m_trkEffHistLooseGlobal = nullptr;
    TH2* m_trkEffHistLooseIBL = nullptr;
    TH2* m_trkEffHistLoosePP0 = nullptr;
    TH2* m_trkEffHistLoosePhysModel = nullptr;
    TH2* m_trkEffHistTightGlobal = nullptr;
    TH2* m_trkEffHistTightIBL = nullptr;
    TH2* m_trkEffHistTightPP0 = nullptr;
    TH2* m_trkEffHistTightPhysModel = nullptr;

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

};


#endif //DHNL_DHNLALGORITHM_H

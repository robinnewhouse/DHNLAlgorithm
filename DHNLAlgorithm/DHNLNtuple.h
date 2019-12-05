#ifndef DHNL_DHNLNTUPLE_H
#define DHNL_DHNLNTUPLE_H

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
#include "xAODAnaHelpers/JetHists.h"
#include "DHNLAlgorithm/DHNLMiniTree.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s):
#include "TH1D.h"

#include <sstream>

class DHNLNtuple : public xAH::Algorithm {
public:

    //configuration variables
    bool m_isMC;                      // Is MC
    bool m_useCutFlow;                // true will write out cutflow histograms
    bool m_writeTree;                 // true will write out a TTree
    std::string m_MCPileupCheckContainer; // Name of truth container for MC Pileup Check
    bool m_useMCPileupCheck;          // determined by name of MCPileupCheckContainer
    float m_leadingJetPtCut;          // Leading jet Pt cut
    float m_subleadingJetPtCut;          // Leading jet Pt cut
    uint m_jetMultiplicity;          // Leading jet Pt cut
    bool m_truthLevelOnly;            // truthLevelOnly info

    std::string m_inJetContainerName;   // input Jet container name
    std::string m_inputAlgo;          // input algo for when running systs
    std::string m_allJetContainerName;// input container name
    std::string m_allJetInputAlgo;    // input algo for when running systs
    std::string m_inMETContainerName;  // input MET container name
    std::string m_inMETTrkContainerName;  // input METTrk container name
    std::string m_inMuContainerName;    // input Muon container name
    std::string m_inElContainerName;    // input Electron container name
    std::string m_truthVertexContainerName;
    std::string m_secondaryVertexContainerName;
    std::string m_inTruthParticleContainerName;

    std::string m_eventDetailStr;     // event info add to tree
    std::string m_trigDetailStr;      // trigger info add to tree
    std::string m_jetDetailStr;       // jet info add to tree
    std::string m_jetFilterDetailStr;       // jet info add to tree
    std::string m_jetDetailStrSyst;   // jetsyst info add to tree
    std::string m_muDetailStr;       // muon info add to tree
    std::string m_elDetailStr;        // electron info add to tree
    std::string m_muFilterDetailStr;       // muon info add to tree
    std::string m_elFilterDetailStr;        // electron info add to tree
    std::string m_metDetailStr;      // met info add to tree
    std::string m_metTrkDetailStr;      // track met info add to tree
    std::string m_trackDetailStr;      // trigger info add to tree
    std::string m_truthVertexDetailStr;
    std::string m_secondaryVertexDetailStr;
    std::string m_truthParticleDetailString;

    std::string m_secondaryVertexBranchName;
    std::string m_truthVertexBranchName;

private:
    int m_eventCounter;     //!

    TH1D *m_cutflowHist;    //!
    TH1D *m_cutflowHistW;   //!
    int m_cutflowFirst;     //!
    int m_iCutflow;         //!
    float m_mcEventWeight;  //!
    std::string m_comEnergy; //!

    std::string m_treeStream;

    std::map<std::string, DHNLMiniTree *> m_myTrees; //!

public:
    // this is a standard constructor
    DHNLNtuple();

    // these are the functions inherited from Algorithm
    EL::StatusCode initialize() override;

    EL::StatusCode setupJob(EL::Job &job) override;

    EL::StatusCode execute() override;

    EL::StatusCode finalize() override;

    void AddTree(std::string name);

    // these are the functions not inherited from Algorithm
    virtual EL::StatusCode configure();

//    bool fillTree(const xAOD::EventInfo *eventInfo,
//                  const xAOD::JetContainer *signalJets,
//                  const xAOD::JetContainer *truthJets,
//                  const xAOD::JetContainer *allJets,
//                  const xAOD::MuonContainer *allMuons,
//                  const xAOD::ElectronContainer *allElectrons,
//                  const xAOD::VertexContainer *vertices,
//                  const xAOD::TrackParticleContainer *tracks,
//                  const xAOD::TruthParticleContainer *TruthPart,
//                  const xAOD::MissingETContainer *Met,
//                  const xAOD::MissingETContainer *MetTrk,
//                  bool count,
//                  std::string systName = "");

    // this is needed to distribute the algorithm to the workers
ClassDef(DHNLNtuple, 1);
};

#endif //DHNL_DHNLNTUPLE_H

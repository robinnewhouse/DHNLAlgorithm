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

    std::string m_inMuContainerName;    // input Muon container name
    bool m_truthLevelOnly;            // truthLevelOnly info
    bool m_isMC;                      // Is MC
    bool m_writeTree;                 // true will write out a TTree
    std::string m_MCPileupCheckContainer; // Name of truth container for MC Pileup Check
    bool m_useMCPileupCheck;          // determined by name of MCPileupCheckContainer
    std::string m_muDetailStr;       // muon info add to tree


private:
    int m_eventCounter;     //!

    TH1D *m_cutflowHist;    //!
    TH1D *m_cutflowHistW;   //!
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

    // this is needed to distribute the algorithm to the workers
ClassDef(DHNLNtuple, 1);
};

#endif //DHNL_DHNLNTUPLE_H

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
//#include "Hto4bLLPAlgorithm/Hto4bLLPMiniTree.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// ROOT include(s):
#include "TH1D.h"

#include <sstream>

class DHNLAlgorithm : public xAH::Algorithm {

    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
public:

    std::string m_inMuContainerName;    // input Muon container name


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
};


#endif //DHNL_DHNLALGORITHM_H

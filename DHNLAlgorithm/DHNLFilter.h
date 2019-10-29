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
    asg::AnaToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool_handle{"CP::MuonSelectionTool/MuonSelectionTool", this};

    bool muonMuonFilter(const xAOD::MuonContainer *allMuons);

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

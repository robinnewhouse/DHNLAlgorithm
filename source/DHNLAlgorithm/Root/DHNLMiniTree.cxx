#include "DHNLAlgorithm/DHNLMiniTree.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/HelperFunctions.h"

DHNLMiniTree::DHNLMiniTree(xAOD::TEvent *event, TTree *tree, TFile *file, xAOD::TStore *store /* = 0 */) :
        HelpTreeBase(event, tree, file, 1e3) {
    Info("DHNLMiniTree", "Creating output TTree");
    tree->SetAutoSave(1000000000); // Why?
    m_firstEvent = true;
    m_store = store;

}

DHNLMiniTree::~DHNLMiniTree() {
    // Clean memory
    for (auto vtx: m_secVerts)
        delete vtx.second;

    for (auto vtx: m_truthVerts)
        delete vtx.second;

}

//////////////////// Connect Defined variables to branches here /////////////////////////////
void DHNLMiniTree::AddEventUser(const std::string &detailStr) {
    (void) detailStr; // suppress warning

    // event variables

    // weights
    m_tree->Branch("weight", &m_weight, "weight/F");

    m_tree->Branch("passesHnlMuMuFilter", &m_passesHnlMuMuFilter);
    m_tree->Branch("passesHnlElMuFilter", &m_passesHnlElMuFilter);
    m_tree->Branch("passesHnlElElFilter", &m_passesHnlElElFilter);
    m_tree->Branch("passesHnlMuElFilter", &m_passesHnlMuElFilter);
    m_tree->Branch("passesVH4bFilter", &m_passesVH4bFilter);

    // weights
//    m_tree->Branch("weight", &m_weight, "weight/F");
//    m_tree->Branch("weight_xs", &m_weight_xs, "weight_xs/F");
//    m_tree->Branch("weight_prescale", &m_weight_prescale, "weight_prescale/F");
//    m_tree->Branch("weight_resonanceKFactor", &m_weight_resonanceKFactor, "weight_resonanceKFactor/F");

}

void DHNLMiniTree::AddMuonsUser(const std::string &detailStr, const std::string &muonName) {
    (void) detailStr; // suppress warning

    std::string name = muonName + "_";
    m_tree->Branch((name + "index").c_str(), &m_muon_index);
    m_tree->Branch((name + "type").c_str(), &m_muon_type);
    m_tree->Branch((name + "passesPromptCuts").c_str(), &m_muon_passesPromptCuts);
    m_tree->Branch((name + "passesDisplacedCuts").c_str(), &m_muon_passesDisplacedCuts);
    m_tree->Branch((name + "px").c_str(), &m_muon_px);
    m_tree->Branch((name + "py").c_str(), &m_muon_py);
    m_tree->Branch((name + "pz").c_str(), &m_muon_pz);
    m_tree->Branch((name + "chi2").c_str(), &m_muon_chi2);
    m_tree->Branch((name + "msInnerMatchDOF").c_str(), &m_muon_msInnerMatchDOF);
    m_tree->Branch((name + "isLRT").c_str(), &m_muon_isLRT);
    m_tree->Branch((name + "myptcone30").c_str(), &m_muon_myptcone30);
    m_tree->Branch((name + "myptcone30noLRT").c_str(), &m_muon_myptcone30noLRT);
}

void DHNLMiniTree::AddElectronsUser(const std::string &detailStr, const std::string &elecName) {
    (void) detailStr; // suppress warning

    std::string name = elecName + "_";
    m_tree->Branch((name + "index").c_str(), &m_electron_index);
    m_tree->Branch((name + "passesPromptCuts").c_str(), &m_electron_passesPromptCuts);
    m_tree->Branch((name + "passesDisplacedCuts").c_str(), &m_electron_passesDisplacedCuts);
    m_tree->Branch((name + "px").c_str(), &m_electron_px);
    m_tree->Branch((name + "py").c_str(), &m_electron_py);
    m_tree->Branch((name + "pz").c_str(), &m_electron_pz);
//    m_tree->Branch("electron_ptC30", &m_electron_ptC30);
}

void DHNLMiniTree::AddTracksUser(const std::string &detailStr, const std::string &trkName) {
    (void) detailStr; // suppress warning

    std::string name = trkName + "_";
    m_tree->Branch((name+ "toSave").c_str(), &m_track_toSave);
    m_tree->Branch((name + "eventNumber").c_str(), &m_track_eventNumber);
    m_tree->Branch((name + "runNumber").c_str(), &m_track_runNumber);
    m_tree->Branch((name + "type").c_str(), &m_track_type);
    m_tree->Branch((name + "quality").c_str(), &m_track_quality);

    m_tree->Branch((name+ "qOverP").c_str(), &m_track_qOverP);
    m_tree->Branch((name+ "theta").c_str(), &m_track_theta);
    m_tree->Branch((name+ "phi").c_str(), &m_track_phi);
    m_tree->Branch((name+ "d0").c_str(), &m_track_d0);
    m_tree->Branch((name+ "z0").c_str(), &m_track_z0);

    m_tree->Branch((name+ "definingParametersCovMatrixVec").c_str(), &m_track_definingParametersCovMatrixVec);

    m_tree->Branch((name+ "vx").c_str(), &m_track_vx);
    m_tree->Branch((name+ "vy").c_str(), &m_track_vy);
    m_tree->Branch((name+ "vz").c_str(), &m_track_vz);

    m_tree->Branch((name+ "beamlineTiltX").c_str(), &m_track_beamlineTiltX);
    m_tree->Branch((name+ "beamlineTiltY").c_str(), &m_track_beamlineTiltY);

    m_tree->Branch((name+ "numberDoF").c_str(), &m_track_numberDoF);
    m_tree->Branch((name+ "chiSquared").c_str(), &m_track_chiSquared);
    m_tree->Branch((name+ "PixelHits").c_str(), &m_track_PixelHits);
    m_tree->Branch((name+ "SCTHits").c_str(), &m_track_SCTHits);
    m_tree->Branch((name+ "BLayHits").c_str(), &m_track_BLayHits);
    m_tree->Branch((name+ "PixShare").c_str(), &m_track_PixShare);
    m_tree->Branch((name+ "SCTShare").c_str(), &m_track_SCTShare);
    m_tree->Branch((name+ "TRTHits").c_str(), &m_track_TRTHits);

    m_tree->Branch((name+ "hitPattern").c_str(), &m_track_hitPattern);
    m_tree->Branch((name+ "px").c_str(), &m_track_px);
    m_tree->Branch((name+ "py").c_str(), &m_track_py);
    m_tree->Branch((name+ "pz").c_str(), &m_track_pz);
    m_tree->Branch((name+ "e").c_str(), &m_track_e);

//    m_tree->Branch("electron_ptC30", &m_electron_ptC30);
}


/////////////////// Assign values to defined event variables here ////////////////////////
void DHNLMiniTree::FillEventUser(const xAOD::EventInfo *eventInfo) {
    // Event level info
    if (eventInfo->isAvailable<bool>("passesHnlMuMuFilter"))
        m_passesHnlMuMuFilter = eventInfo->auxdecor<bool>("passesHnlMuMuFilter");
    if (eventInfo->isAvailable<bool>("passesHnlElMuFilter"))
        m_passesHnlElMuFilter = eventInfo->auxdecor<bool>("passesHnlElMuFilter");
    if (eventInfo->isAvailable<bool>("passesHnlElElFilter"))
        m_passesHnlElElFilter = eventInfo->auxdecor<bool>("passesHnlElElFilter");
    if (eventInfo->isAvailable<bool>("passesHnlMuElFilter"))
        m_passesHnlMuElFilter = eventInfo->auxdecor<bool>("passesHnlMuElFilter");

    if (eventInfo->isAvailable<bool>("passesVH4bFilter"))
        m_passesVH4bFilter = eventInfo->auxdecor<bool>("passesVH4bFilter");
}

void DHNLMiniTree::FillMuonsUser(const xAOD::Muon *muon, const std::string &muonName) {
    (void) muonName; // suppress warning

    if (m_debug) std::cout << muonName;

    if (muon->isAvailable<int>("index"))
        m_muon_index.push_back(muon->auxdecor<int>("index"));

    if (muon->isAvailable<int>("type"))
        m_muon_type.push_back(muon->auxdecor<int>("type"));

    if (muon->isAvailable<bool>("passesPromptCuts"))
        m_muon_passesPromptCuts.push_back(muon->auxdecor<bool>("passesPromptCuts"));
    else m_muon_passesPromptCuts.push_back(false);

    if (muon->isAvailable<bool>("passesDisplacedCuts"))
        m_muon_passesDisplacedCuts.push_back(muon->auxdecor<bool>("passesDisplacedCuts"));
    else m_muon_passesDisplacedCuts.push_back(false);

    if (muon->isAvailable<float>("px"))
        m_muon_px.push_back(muon->auxdecor<float>("px"));

    if (muon->isAvailable<float>("py"))
        m_muon_py.push_back(muon->auxdecor<float>("py"));

    if (muon->isAvailable<float>("pz"))
        m_muon_pz.push_back(muon->auxdecor<float>("pz"));

    if (muon->isAvailable<float>("myptcone30"))
        m_muon_myptcone30.push_back(muon->auxdecor<float>("myptcone30"));

    if (muon->isAvailable<float>("myptcone30noLRT"))
        m_muon_myptcone30noLRT.push_back(muon->auxdecor<float>("myptcone30noLRT"));

    if (muon->isAvailable<float>("chi2"))
        m_muon_chi2.push_back(muon->auxdecor<float>("chi2"));

    if (muon->isAvailable<int>("msDOF"))
        m_muon_msInnerMatchDOF.push_back(muon->auxdecor<int>("msDOF"));

    if (muon->isAvailable<bool>("isLRT"))
        m_muon_isLRT.push_back(muon->auxdecor<bool>("isLRT"));

}

void DHNLMiniTree::FillElectronsUser(const xAOD::Electron *electron, const std::string &electronName) {
    (void) electronName; // suppress warning

    if (electron->isAvailable<int>("index"))
        m_electron_index.push_back(electron->auxdecor<int>("index"));

    if (electron->isAvailable<bool>("passesPromptCuts"))
        m_electron_passesPromptCuts.push_back(electron->auxdecor<bool>("passesPromptCuts"));
    else m_electron_passesPromptCuts.push_back(false);

    if (electron->isAvailable<bool>("passesDisplacedCuts"))
        m_electron_passesDisplacedCuts.push_back(electron->auxdecor<bool>("passesDisplacedCuts"));
    else m_electron_passesDisplacedCuts.push_back(false);

    if (electron->isAvailable<float>("px"))
        m_electron_px.push_back(electron->auxdecor<float>("px"));

    if (electron->isAvailable<float>("py"))
        m_electron_py.push_back(electron->auxdecor<float>("py"));

    if (electron->isAvailable<float>("pz"))
        m_electron_pz.push_back(electron->auxdecor<float>("pz"));

//    if (electron->isAvailable<float>("ptC30"))
//        m_electron_ptC30.push_back(electron->auxdecor<float>("ptC30"));

}


//////////////////// Clear any defined vectors here ////////////////////////////
void DHNLMiniTree::ClearEventUser() {
    m_passesHnlMuMuFilter = false;
    m_passesHnlElMuFilter = false;
    m_passesHnlElElFilter = false;
    m_passesHnlMuElFilter = false;
    m_passesVH4bFilter = false;
}

void DHNLMiniTree::ClearMuonsUser(const std::string &muonName) {
    (void) muonName; // suppress warning
    m_muon_index.clear();
    m_muon_type.clear();
    m_muon_passesPromptCuts.clear();
    m_muon_passesDisplacedCuts.clear();
    m_muon_px.clear();
    m_muon_py.clear();
    m_muon_pz.clear();
    m_muon_chi2.clear();
    m_muon_msInnerMatchDOF.clear();
    m_muon_isLRT.clear();
    m_muon_myptcone30.clear();
    m_muon_myptcone30noLRT.clear();
}

void DHNLMiniTree::ClearElectronsUser(const std::string &electronName) {
    (void) electronName; // suppress warning
    m_electron_index.clear();
    m_electron_passesPromptCuts.clear();
    m_electron_passesDisplacedCuts.clear();
    m_electron_px.clear();
    m_electron_py.clear();
    m_electron_pz.clear();
//    m_electron_ptC30.clear();
}


//////////////////// TRUTH VERTICES //////////////////////////////////////////

void DHNLMiniTree::AddTruthVerts(const std::string detailStr, const std::string truthVtxName) {
    if (m_debug) Info("DVsHelpTreeBase::AddTruthVerts()", "adding truth vertex variables");

    m_truthVerts[truthVtxName] = new DVs::TruthVertexContainer(truthVtxName, detailStr, m_units, m_isMC, false, false);
    DVs::TruthVertexContainer *thisTruthVtx = m_truthVerts[truthVtxName];
    thisTruthVtx->setBranches(m_tree);
}

void DHNLMiniTree::FillTruthVerts(const xAOD::TruthVertexContainer *truthVerts, const std::string truthVtxName) {
    this->ClearTruthVerts(truthVtxName);

    for (const auto &truthVtx : *truthVerts)
        this->FillTruthVertex(truthVtx, truthVtxName);
}

void DHNLMiniTree::FillTruthVertex(const xAOD::TruthVertex *truthVtx, const std::string truthVtxName) {
    std::string treeName = m_tree->GetName();
    DVs::TruthVertexContainer *thisTruthVtx = m_truthVerts[truthVtxName];
    thisTruthVtx->FillTruthVertex(truthVtx, treeName);
}

void DHNLMiniTree::ClearTruthVerts(const std::string truthVtxName) {
    DVs::TruthVertexContainer *thisTruthVtx = m_truthVerts[truthVtxName];
    thisTruthVtx->clear();
}


//////////////////// SECONDARY VERTICES //////////////////////////////////////////
void DHNLMiniTree::AddSecondaryVerts(const std::string detailStr, const std::string secVtxName, const std::string& AugmentationVersionString) {
    if (m_debug) Info("DHNLMiniTree::AddSecondaryVerts()", "adding secondary vertex variables");

    m_secVerts[secVtxName] = new DVs::SecondaryVertexContainer(secVtxName, detailStr, m_units, m_isMC, false, false, AugmentationVersionString);
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->setBranches(m_tree);
}


void DHNLMiniTree::FillSecondaryVerts(const xAOD::VertexContainer *secVerts, const std::string secVtxName, bool suppressFilter) {
    this->ClearSecondaryVerts(secVtxName);

    for (const auto &secVtx : *secVerts)
        this->FillSecondaryVertex(secVtx, secVtxName, suppressFilter);
}


void DHNLMiniTree::FillSecondaryVertex(const xAOD::Vertex *secVtx, const std::string secVtxName, bool suppressFilter) {
    std::string treeName = m_tree->GetName();
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->FillSecondaryVertex(secVtx, "", treeName, suppressFilter);

}

void DHNLMiniTree::ClearSecondaryVerts(const std::string secVtxName) {
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->clear();
}


//////////////////// TRACKS (USED FOR BACKGROUND ESTIMATION) //////////////////////////////////////////

void DHNLMiniTree::FillTracksUser(const xAOD::TrackParticle *track, const std::string &trackName) {
    (void) trackName; // suppress warning

    if (track->isAvailable<bool>("be_toSave"))
        m_track_toSave.push_back(track->auxdecor<bool>("be_toSave"));

    if (track->isAvailable<int>("be_type"))
        m_track_type.push_back(track->auxdecor<int>("be_type"));

    if (track->isAvailable<int>("be_quality"))
        m_track_quality.push_back(track->auxdecor<int>("be_quality"));

    if (track->isAvailable<uint32_t>("be_runNumber"))
        m_track_runNumber.push_back(track->auxdecor<uint32_t>("be_runNumber"));

    if (track->isAvailable<unsigned long long>("be_eventNumber"))
        m_track_eventNumber.push_back(track->auxdecor<unsigned long long>("be_eventNumber"));
    
    
    // Track details

    if (track->isAvailable<float_t>("be_qOverP"))
        m_track_qOverP.push_back(track->auxdecor<float_t>("be_qOverP"));
    if (track->isAvailable<float_t>("be_theta"))
        m_track_theta.push_back(track->auxdecor<float_t>("be_theta"));
    if (track->isAvailable<float_t>("be_phi"))
        m_track_phi.push_back(track->auxdecor<float_t>("be_phi"));
    if (track->isAvailable<float_t>("be_d0"))
        m_track_d0.push_back(track->auxdecor<float_t>("be_d0"));
    if (track->isAvailable<float_t>("be_z0"))
        m_track_z0.push_back(track->auxdecor<float_t>("be_z0"));

    if (track->isAvailable<std::vector< float >>("be_definingParametersCovMatrixVec"))
        m_track_definingParametersCovMatrixVec.push_back(track->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec"));

    if (track->isAvailable<float_t>("be_vx"))
        m_track_vx.push_back(track->auxdecor<float_t>("be_vx"));
    if (track->isAvailable<float_t>("be_vy"))
        m_track_vy.push_back(track->auxdecor<float_t>("be_vy"));
    if (track->isAvailable<float_t>("be_vz"))
        m_track_vz.push_back(track->auxdecor<float_t>("be_vz"));

    if (track->isAvailable<float_t>("be_beamlineTiltX"))
        m_track_beamlineTiltX.push_back(track->auxdecor<float_t>("be_beamlineTiltX"));
    if (track->isAvailable<float_t>("be_beamlineTiltY"))
        m_track_beamlineTiltY.push_back(track->auxdecor<float_t>("be_beamlineTiltY"));

    if (track->isAvailable<float_t>("be_numberDoF"))
        m_track_numberDoF.push_back(track->auxdecor<float_t>("be_numberDoF"));
    if (track->isAvailable<float_t>("be_chiSquared"))
        m_track_chiSquared.push_back(track->auxdecor<float_t>("be_chiSquared"));

    if (track->isAvailable<uint8_t>("be_PixelHits"))
        m_track_PixelHits.push_back(track->auxdecor<uint8_t>("be_PixelHits"));
    if (track->isAvailable<uint8_t>("be_SCTHits"))
        m_track_SCTHits.push_back(track->auxdecor<uint8_t>("be_SCTHits"));
    if (track->isAvailable<uint8_t>("be_BLayHits"))
        m_track_BLayHits.push_back(track->auxdecor<uint8_t>("be_BLayHits"));
    if (track->isAvailable<uint8_t>("be_PixShare"))
        m_track_PixShare.push_back(track->auxdecor<uint8_t>("be_PixShare"));
    if (track->isAvailable<uint8_t>("be_SCTShare"))
        m_track_SCTShare.push_back(track->auxdecor<uint8_t>("be_SCTShare"));
    if (track->isAvailable<uint8_t>("be_TRTHits"))
        m_track_TRTHits.push_back(track->auxdecor<uint8_t>("be_TRTHits"));

    if (track->isAvailable<uint32_t>("be_hitPattern"))
        m_track_hitPattern.push_back(track->auxdecor<uint32_t>("be_hitPattern"));
    
    if (track->isAvailable<Double_t>("be_px"))
        m_track_px.push_back(track->auxdecor<Double_t>("be_px"));
    if (track->isAvailable<Double_t>("be_py"))
        m_track_py.push_back(track->auxdecor<Double_t>("be_py"));
    if (track->isAvailable<Double_t>("be_pz"))
        m_track_pz.push_back(track->auxdecor<Double_t>("be_pz"));
    if (track->isAvailable<Double_t>("be_e"))
        m_track_e.push_back(track->auxdecor<Double_t>("be_e"));

}

void DHNLMiniTree::ClearTracksUser(const std::string &trackName) {
    (void) trackName; // suppress warning
    m_track_toSave.clear();
    m_track_type.clear();
    m_track_quality.clear();
    m_track_runNumber.clear();
    m_track_eventNumber.clear();

    m_track_qOverP.clear();
    m_track_theta.clear();
    m_track_phi.clear();
    m_track_d0.clear();
    m_track_z0.clear();

    m_track_definingParametersCovMatrixVec.clear();

    m_track_vx.clear();
    m_track_vy.clear();
    m_track_vz.clear();

    m_track_beamlineTiltX.clear();
    m_track_beamlineTiltY.clear();

    m_track_numberDoF.clear();
    m_track_chiSquared.clear();
    m_track_PixelHits.clear();
    m_track_SCTHits.clear();
    m_track_BLayHits.clear();
    m_track_PixShare.clear();
    m_track_SCTShare.clear();
    m_track_TRTHits.clear();

    m_track_hitPattern.clear();
    m_track_px.clear();
    m_track_py.clear();
    m_track_pz.clear();
    m_track_e.clear();

}

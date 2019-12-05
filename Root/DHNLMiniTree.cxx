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

}

//////////////////// Connect Defined variables to branches here /////////////////////////////
void DHNLMiniTree::AddEventUser(const std::string detailStr) {

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

void DHNLMiniTree::AddMuonsUser(std::string detailStr, std::string muonName) {
    m_tree->Branch("muon_index", &m_muon_index);
    m_tree->Branch("muon_type", &m_muon_type);
    m_tree->Branch("muon_passesPromptCuts", &m_muon_passesPromptCuts);
    m_tree->Branch("muon_passesDisplacedCuts", &m_muon_passesDisplacedCuts);
    m_tree->Branch("muon_px", &m_muon_px);
    m_tree->Branch("muon_py", &m_muon_py);
    m_tree->Branch("muon_pz", &m_muon_pz);
//    m_tree->Branch("muon_ptC30", &m_muon_ptC30);
}

void DHNLMiniTree::AddElectronsUser(std::string detailStr, std::string electronName) {
    m_tree->Branch("electron_index", &m_electron_index);
    m_tree->Branch("electron_passesPromptCuts", &m_electron_passesPromptCuts);
    m_tree->Branch("electron_passesDisplacedCuts", &m_electron_passesDisplacedCuts);
    m_tree->Branch("electron_px", &m_electron_px);
    m_tree->Branch("electron_py", &m_electron_py);
    m_tree->Branch("electron_pz", &m_electron_pz);
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

void DHNLMiniTree::FillMuonsUser(const xAOD::Muon *muon, const std::string muonName) {
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

//    if (muon->isAvailable<float>("ptC30"))
//        m_muon_ptC30.push_back(muon->auxdecor<float>("ptC30"));
}

void DHNLMiniTree::FillElectronsUser(const xAOD::Electron *electron, const std::string electronName) {
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
    m_secVtxTrackParticleIndex.clear();
    m_secVtxMuonIndex.clear();
    m_secVtxElectronIndex.clear();
}

void DHNLMiniTree::ClearMuonsUser(std::string muonName) {
    m_muon_index.clear();
    m_muon_type.clear();
    m_muon_passesPromptCuts.clear();
    m_muon_passesDisplacedCuts.clear();
    m_muon_px.clear();
    m_muon_py.clear();
    m_muon_pz.clear();
//    m_muon_ptC30.clear();
}

void DHNLMiniTree::ClearElectronsUser(std::string electronName) {
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

void DHNLMiniTree::AddSecondaryVerts(const std::string detailStr, const std::string secVtxName) {
    if (m_debug) Info("DHNLMiniTree::AddSecondaryVerts()", "adding secondary vertex variables");

    m_secVerts[secVtxName] = new DVs::SecondaryVertexContainer(secVtxName, detailStr, m_units, m_isMC, false, false);
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->setBranches(m_tree);
}

void DHNLMiniTree::FillSecondaryVerts(const xAOD::VertexContainer *secVerts, const std::string secVtxName) {
    this->ClearSecondaryVerts(secVtxName);

    for (const auto &secVtx : *secVerts)
        this->FillSecondaryVertex(secVtx, secVtxName);
}

void DHNLMiniTree::FillSecondaryVertex(const xAOD::Vertex *secVtx, const std::string secVtxName,  bool suppressFilter) { // Added suppressFilter -Dominique
    std::string treeName = m_tree->GetName();
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->FillSecondaryVertex(secVtx, treeName, suppressFilter);
}

void DHNLMiniTree::ClearSecondaryVerts(const std::string secVtxName) {
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->clear();
}


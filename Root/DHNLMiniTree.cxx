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

    m_tree->Branch("passesFilter", &m_passesFilter);


    m_tree->Branch("secVtxTrackParticleIndex", &m_secVtxTrackParticleIndex);
    m_tree->Branch("secVtxMuonIndex", &m_secVtxMuonIndex);
    m_tree->Branch("secVtxMuonPt", &m_secVtxMuonPt);
    m_tree->Branch("secVtxElectronIndex", &m_secVtxElectronIndex);


//    m_tree->Branch("passesElecFilter", &m_passesElecFilter);
//    m_tree->Branch("passesMuonFilter", &m_passesMuonFilter);
//    m_tree->Branch("Met", &m_Met);

    // weights
//    m_tree->Branch("weight", &m_weight, "weight/F");
//    m_tree->Branch("weight_xs", &m_weight_xs, "weight_xs/F");
//    m_tree->Branch("weight_prescale", &m_weight_prescale, "weight_prescale/F");
//    m_tree->Branch("weight_resonanceKFactor", &m_weight_resonanceKFactor, "weight_resonanceKFactor/F");

}

void DHNLMiniTree::AddMuonsUser(std::string detailStr, std::string muonName) {
    m_tree->Branch("muon_index", &m_muon_index);
    m_tree->Branch("muon_type", &m_muon_type);
}

void DHNLMiniTree::AddElectronsUser(std::string detailStr, std::string electronName) {
    m_tree->Branch("electron_index", &m_electron_index);
}

/////////////////// Assign values to defined event variables here ////////////////////////
void DHNLMiniTree::FillEventUser(const xAOD::EventInfo *eventInfo) {
    // Event level info
    if (eventInfo->isAvailable<int>("passesFilter"))
        m_passesFilter = eventInfo->auxdecor<int>("passesFilter");

    // Displaced Vertex Info // Temporary, moving this to a more logical place
    if (eventInfo->isAvailable<std::vector<std::vector<int>>>("secVtxTrackParticleIndex"))
        m_secVtxTrackParticleIndex = eventInfo->auxdecor<std::vector<std::vector<int>>>("secVtxTrackParticleIndex");
    if (eventInfo->isAvailable<std::vector<std::vector<int>>>("secVtxMuonIndex"))
        m_secVtxMuonIndex = eventInfo->auxdecor<std::vector<std::vector<int>>>("secVtxMuonIndex");
    if (eventInfo->isAvailable<std::vector<std::vector<int>>>("secVtxElectronIndex"))
        m_secVtxElectronIndex = eventInfo->auxdecor<std::vector<std::vector<int>>>("secVtxElectronIndex");
}


void DHNLMiniTree::FillMuonsUser(const xAOD::Muon *muon, const std::string muonName) {
    if (muon->isAvailable<int>("index"))
        m_muon_index.push_back(muon->auxdecor<int>("index"));
    if (muon->isAvailable<int>("type"))
        m_muon_type.push_back(muon->auxdecor<int>("type"));
}

void DHNLMiniTree::FillElectronsUser(const xAOD::Electron *electron, const std::string electronName) {
    if (electron->isAvailable<int>("index"))
        m_electron_index.push_back(electron->auxdecor<int>("index"));
}


//////////////////// Clear any defined vectors here ////////////////////////////
void DHNLMiniTree::ClearEventUser() {
    m_passesFilter = -999;
    m_secVtxTrackParticleIndex.clear();
    m_secVtxMuonIndex.clear();
    m_secVtxElectronIndex.clear();
}

void DHNLMiniTree::ClearMuonsUser(std::string muonName) {
    m_muon_index.clear();
    m_muon_type.clear();
}

void DHNLMiniTree::ClearElectronsUser(std::string electronName) {
    m_electron_index.clear();
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

void DHNLMiniTree::FillSecondaryVertex(const xAOD::Vertex *secVtx, const std::string secVtxName) {
    std::string treeName = m_tree->GetName();
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->FillSecondaryVertex(secVtx, treeName);
}

void DHNLMiniTree::ClearSecondaryVerts(const std::string secVtxName) {
    DVs::SecondaryVertexContainer *thisSecVtx = m_secVerts[secVtxName];
    thisSecVtx->clear();
}


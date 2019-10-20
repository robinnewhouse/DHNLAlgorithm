#include "DHNLAlgorithm/DHNLMiniTree.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/HelperFunctions.h"

DHNLMiniTree::DHNLMiniTree(xAOD::TEvent *event, TTree *tree, TFile *file, xAOD::TStore *store /* = 0 */) :
        HelpTreeBase(event, tree, file, 1e3) {
    Info("Hto4bLLPMiniTree", "Creating output TTree");
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

}
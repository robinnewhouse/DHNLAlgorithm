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

}

//////////////////// TRUTH VERTICES //////////////////////////////////////////

void DHNLMiniTree :: AddTruthVerts ( const std::string detailStr, const std::string truthVtxName )
{
    if ( m_debug ) Info( "DVsHelpTreeBase::AddTruthVerts()", "adding truth vertex variables" );

    m_truthVerts[ truthVtxName ] = new DVs::TruthVertexContainer( truthVtxName, detailStr, m_units, m_isMC, false, false );
    DVs::TruthVertexContainer* thisTruthVtx = m_truthVerts[ truthVtxName ];
    thisTruthVtx->setBranches( m_tree );
}

void DHNLMiniTree :: FillTruthVerts ( const xAOD::TruthVertexContainer* truthVerts, const std::string truthVtxName )
{
    this->ClearTruthVerts ( truthVtxName );

    for ( const auto& truthVtx : *truthVerts )
        this->FillTruthVertex( truthVtx, truthVtxName );
}

void DHNLMiniTree :: FillTruthVertex ( const xAOD::TruthVertex* truthVtx, const std::string truthVtxName )
{
    std::string treeName = m_tree->GetName();
    DVs::TruthVertexContainer* thisTruthVtx = m_truthVerts[ truthVtxName ];
    thisTruthVtx->FillTruthVertex( truthVtx, treeName );
}

void DHNLMiniTree :: ClearTruthVerts ( const std::string truthVtxName )
{
    DVs::TruthVertexContainer* thisTruthVtx = m_truthVerts[ truthVtxName ];
    thisTruthVtx->clear();
}


//////////////////// SECONDARY VERTICES //////////////////////////////////////////

void DHNLMiniTree::AddSecondaryVerts ( const std::string detailStr, const std::string secVtxName )
{
    if ( m_debug ) Info( "DHNLMiniTree::AddSecondaryVerts()", "adding secondary vertex variables" );

    m_secVerts[ secVtxName ] = new DVs::SecondaryVertexContainer( secVtxName, detailStr, m_units, m_isMC, false, false );
    DVs::SecondaryVertexContainer* thisSecVtx = m_secVerts[ secVtxName ];
    thisSecVtx->setBranches( m_tree );
}

void DHNLMiniTree::FillSecondaryVerts ( const xAOD::VertexContainer* secVerts, const std::string secVtxName )
{
    this->ClearSecondaryVerts ( secVtxName );

    for ( const auto& secVtx : *secVerts )
        this->FillSecondaryVertex( secVtx, secVtxName );
}

void DHNLMiniTree::FillSecondaryVertex ( const xAOD::Vertex* secVtx, const std::string secVtxName )
{
    std::string treeName = m_tree->GetName();
    DVs::SecondaryVertexContainer* thisSecVtx = m_secVerts[ secVtxName ];
    thisSecVtx->FillSecondaryVertex( secVtx, treeName );
}

void DHNLMiniTree:: ClearSecondaryVerts ( const std::string secVtxName )
{
    DVs::SecondaryVertexContainer* thisSecVtx = m_secVerts[ secVtxName ];
    thisSecVtx->clear();
}


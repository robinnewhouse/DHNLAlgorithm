#ifndef DHNL_DHNLMINITREE_H
#define DHNL_DHNLMINITREE_H

#include "xAODAnaHelpers/HelpTreeBase.h"
#include "TTree.h"

#include "DVAnalysisBase/SecondaryVertexContainer.h"
#include "DVAnalysisBase/TruthVertexContainer.h"

class DHNLMiniTree : public HelpTreeBase {

private:
    bool m_firstEvent;

    float m_weight;

public:
    DHNLMiniTree(xAOD::TEvent *event, TTree *tree, TFile *file, xAOD::TStore *store = nullptr);

    ~DHNLMiniTree() override;

    void AddEventUser(std::string detailStr = "") ;

    void AddTruthVerts   ( const std::string detailStr = "",             const std::string truthVtxName = "truthVtx" );
    void FillTruthVerts  ( const xAOD::TruthVertexContainer* truthVerts, const std::string truthVtxName = "truthVtx" );
    void FillTruthVertex ( const xAOD::TruthVertex* truthVtx,            const std::string truthVtxName = "truthVtx" );
    void ClearTruthVerts ( const std::string truthVtxName = "truthVtx"                                               );

    void AddSecondaryVerts   ( const std::string detailStr = "",      const std::string secVtxName = "secVtx" );
    void FillSecondaryVerts  ( const xAOD::VertexContainer* secVerts, const std::string secVtxName = "secVtx" );
    void FillSecondaryVertex ( const xAOD::Vertex* secVtx,            const std::string secVtxName = "secVtx" );
    void ClearSecondaryVerts ( const std::string secVtxName = "secVtx"                                        );

protected:
    std::map<std::string, DVs::TruthVertexContainer *> m_truthVerts;
    std::map<std::string, DVs::SecondaryVertexContainer *> m_secVerts;
};


#endif //DHNL_DHNLMINITREE_H

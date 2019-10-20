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

protected:
    std::map<std::string, DVs::TruthVertexContainer *> m_truthVerts;
    std::map<std::string, DVs::SecondaryVertexContainer *> m_secVerts;
};


#endif //DHNL_DHNLMINITREE_H

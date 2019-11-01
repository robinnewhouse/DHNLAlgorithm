#ifndef DHNL_DHNLMINITREE_H
#define DHNL_DHNLMINITREE_H

#include "xAODAnaHelpers/HelpTreeBase.h"
#include "TTree.h"

#include "DVAnalysisBase/SecondaryVertexContainer.h"
#include "DVAnalysisBase/TruthVertexContainer.h"

class DHNLMiniTree : public HelpTreeBase {

private:
    bool m_firstEvent;

    int m_passesFilter;
    std::vector<std::vector<int>> m_secVtxTrackParticleIndex;
    std::vector<std::vector<int>> m_secVtxMuonIndex;
    std::vector<std::vector<double>> m_secVtxMuonPt;
    std::vector<std::vector<int>> m_secVtxElectronIndex;
    std::vector<int> m_muon_index;
    std::vector<int> m_muon_type;
    std::vector<int> m_electron_index;

    float m_weight;

public:
    DHNLMiniTree(xAOD::TEvent *event, TTree *tree, TFile *file, xAOD::TStore *store = nullptr);

    ~DHNLMiniTree() override;

    void AddEventUser(std::string detailStr = "") override;

    void FillEventUser(const xAOD::EventInfo *eventInfo) override;

    void ClearEventUser() override;

    void AddMuonsUser(std::string detailStr = "", std::string muonName = "muon") override;

    void FillMuonsUser(const xAOD::Muon *, std::string muonName = "muon") override;

    void ClearMuonsUser( std::string muonName = "muon") override;

    void AddElectronsUser(std::string detailStr = "", std::string electronName = "electron") override;

    void FillElectronsUser(const xAOD::Electron *, std::string electronName = "electron") override;

    void ClearElectronsUser( std::string electronName = "electron") override;

    void AddTruthVerts(const std::string detailStr = "", const std::string truthVtxName = "truthVtx");

    void FillTruthVerts(const xAOD::TruthVertexContainer *truthVerts, const std::string truthVtxName = "truthVtx");

    void FillTruthVertex(const xAOD::TruthVertex *truthVtx, const std::string truthVtxName = "truthVtx");

    void ClearTruthVerts(const std::string truthVtxName = "truthVtx");

    void AddSecondaryVerts(const std::string detailStr = "", const std::string secVtxName = "secVtx");

    void FillSecondaryVerts(const xAOD::VertexContainer *secVerts, const std::string secVtxName = "secVtx");

    void FillSecondaryVertex(const xAOD::Vertex *secVtx, const std::string secVtxName = "secVtx");

    void ClearSecondaryVerts(const std::string secVtxName = "secVtx");

protected:
    std::map<std::string, DVs::TruthVertexContainer *> m_truthVerts;
    std::map<std::string, DVs::SecondaryVertexContainer *> m_secVerts;

};


#endif //DHNL_DHNLMINITREE_H

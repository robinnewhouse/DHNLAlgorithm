#ifndef DHNL_DHNLMINITREE_H
#define DHNL_DHNLMINITREE_H

#include "xAODAnaHelpers/HelpTreeBase.h"
#include "TTree.h"

#include "DVAnalysisBase/SecondaryVertexContainer.h"
#include "DVAnalysisBase/TruthVertexContainer.h"

#include "xAODAnaHelpers/ParticlePIDManager.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronLikelihoodTool.h"


class DHNLMiniTree : public HelpTreeBase {

private:
    bool m_firstEvent;

    bool m_passesHnlMuMuFilter;
    bool m_passesHnlElMuFilter;
    bool m_passesHnlElElFilter;
    bool m_passesHnlMuElFilter;
    bool m_passesVH4bFilter;

    std::vector<int> m_muon_index;
    std::vector<int> m_muon_type;
    std::vector<bool> m_muon_passesPromptCuts;
    std::vector<bool> m_muon_passesDisplacedCuts;
    std::vector<double> m_muon_px;
    std::vector<double> m_muon_py;
    std::vector<double> m_muon_pz;
    std::vector<double> m_muon_myptcone30;
    std::vector<double> m_muon_myptcone30noLRT;
    std::vector<double> m_muon_chi2;
    std::vector<int> m_muon_msInnerMatchDOF;
    std::vector<bool> m_muon_isLRT;

    std::vector<int> m_electron_index;
    std::vector<bool> m_electron_passesPromptCuts;
    std::vector<bool> m_electron_passesDisplacedCuts;
    std::vector<bool> m_electron_isVeryLoose;
    std::vector<bool> m_electron_isVeryVeryLoose;
    std::vector<bool> m_electron_isVeryVeryLooseSi;
    std::vector<double> m_electron_px;
    std::vector<double> m_electron_py;
    std::vector<double> m_electron_pz;
    std::vector<double> m_electron_ptC30;

    std::vector<bool> m_track_toSave;
    std::vector<unsigned long long> m_track_eventNumber;
    std::vector<std::uint32_t> m_track_runNumber;
    std::vector<int> m_track_type;
    std::vector<int> m_track_muonType;
    std::vector<int> m_track_quality;
    std::vector<bool> m_track_isLRT;
    std::vector<bool> m_track_fromPV;

    std::vector<int>m_track_isTight;
    std::vector<int>m_track_isMedium;
    std::vector<int>m_track_isLoose;

    std::vector<int>m_track_isVeryLoose;
    std::vector<int>m_track_isVeryVeryLoose;
    std::vector<int>m_track_isVeryVeryLooseSi;



    std::vector<std::vector< float >> m_track_definingParametersCovMatrixVec;

    std::vector<float_t> m_track_vx;
    std::vector<float_t> m_track_vy;
    std::vector<float_t> m_track_vz;

    std::vector<float_t> m_track_beamlineTiltX;
    std::vector<float_t> m_track_beamlineTiltY;

    std::vector<uint32_t> m_track_hitPattern;

    std::vector<Double_t> m_track_px;
    std::vector<Double_t> m_track_py;
    std::vector<Double_t> m_track_pz;
    std::vector<Double_t> m_track_e;

    std::map<std::string,std::vector<int>> m_secVerts_muons_per_event;
    std::map<std::string,std::vector<int>> m_secVerts_electrons_per_event;
    std::map<std::string,std::vector<bool>> m_secVerts_shuffled;

    float m_weight;
    float m_weight_xs;


    //Likelihood tools: */
    
    ElectronLHPIDManager*      m_el_LH_PIDManager = nullptr;        //!
    AsgElectronLikelihoodTool* m_LHToolVeryLoose; //
    AsgElectronLikelihoodTool* m_LHToolVeryVeryLoose; // 
    AsgElectronLikelihoodTool* m_LHToolVeryVeryLooseSi; // 


public:
    DHNLMiniTree(xAOD::TEvent *event, TTree *tree, TFile *file, xAOD::TStore *store = nullptr);

    ~DHNLMiniTree() override;

    void AddEventUser(const std::string &detailStr) override;

    void FillEventUser(const xAOD::EventInfo *eventInfo) override;

    void ClearEventUser() override;

    void AddMuonsUser(const std::string &detailStr, const std::string &muonName) override;

    void FillMuonsUser(const xAOD::Muon *, const std::string &muonName) override;

    void ClearMuonsUser(const std::string &muonName) override;

    void AddElectronsUser(const std::string &detailStr, const std::string &elecName) override;

    void FillElectronsUser(const xAOD::Electron *, const std::string &electronName) override;

    void ClearElectronsUser(const std::string &electronName) override;

    void AddTruthVerts(const std::string detailStr = "", const std::string truthVtxName = "truthVtx");

    void FillTruthVerts(const xAOD::TruthVertexContainer *truthVerts, const std::string truthVtxName = "truthVtx");

    void FillTruthVertex(const xAOD::TruthVertex *truthVtx, const std::string truthVtxName = "truthVtx");

    void ClearTruthVerts(const std::string truthVtxName = "truthVtx");

    void AddSecondaryVerts(std::string detailStr = "", std::string secVtxName = "secVtx", const std::string& AugmentationVersionString = "");

    void FillSecondaryVerts(const xAOD::VertexContainer *secVerts, const std::string secVtxName = "secVtx", bool suppressFilter = true);

    void FillSecondaryVertex(const xAOD::Vertex *secVtx, const std::string secVtxName = "secVtx", bool suppressFilter = true);

    void ClearSecondaryVerts(const std::string secVtxName = "secVtx");


    void AddTracksUser(const std::string &detailStr, const std::string &trkName);

    void FillTracksUser(const xAOD::TrackParticle *track, const std::string &trackName);

    void ClearTracksUser(const std::string &trkName);
protected:
    std::map<std::string, DVs::TruthVertexContainer *> m_truthVerts;
    std::map<std::string, DVs::SecondaryVertexContainer *> m_secVerts;

};


#endif //DHNL_DHNLMINITREE_H

//
// Created by Sean Galantzan on 10/02/2021.
//

#ifndef DHNL_VSITRACKSELECTION_H
#define DHNL_VSITRACKSELECTION_H

// ROOT include(s):
#include "TH1D.h"

// Framework include(s)
#include <EventLoop/StatusCode.h>
#include <EventLoop/Algorithm.h>

//algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"

class VSITrackSelection : public xAH::Algorithm
{
    // put your configuration variables here as public variables.
    // that way they can be set directly from CINT and python.
public:
    // configuration variables
    /// @brief input container name
    std::string m_inDetTrackParticlesContainerName = "";
    /// @brief input container name
    std::string m_inElContainerName = "";
    /// @brief input container name
    std::string m_inMuContainerName = "";
    /// @brief input container name
    std::string m_vertexContainerName = "";
    /// @brief output container name
    std::string m_outContainerName = "";



    bool   m_jp_passThroughTrackSelection;

    // track selection conditions
    bool   m_jp_SAloneTRT;

    /* impact parameters */
    bool   m_jp_do_PVvetoCut;
    bool   m_jp_do_d0Cut;
    bool   m_jp_do_z0Cut;
    bool   m_jp_do_d0errCut;
    bool   m_jp_do_z0errCut;
    bool   m_jp_do_d0signifCut;
    bool   m_jp_do_z0signifCut;

    double m_jp_d0TrkPVDstMinCut;
    double m_jp_d0TrkPVDstMaxCut;
    double m_jp_z0TrkPVDstMinCut;
    double m_jp_z0TrkPVDstMaxCut;
    double m_jp_d0TrkErrorCut;
    double m_jp_z0TrkErrorCut;

    /* pT anc chi2 */
    double m_jp_TrkChi2Cut;
    double m_jp_TrkPtCut;

    /* hit requirements */
    bool m_jp_doTRTPixCut; // Kazuki
    int  m_jp_CutSctHits;
    int  m_jp_CutPixelHits;
    int  m_jp_CutSiHits;
    int  m_jp_CutBLayHits;
    int  m_jp_CutSharedHits;
    int  m_jp_CutTRTHits; // Kazuki
    int  m_jp_CutTightSCTHits;
    int  m_jp_CutTightTRTHits;


    // vertexing using muons (test implementation)
    bool m_jp_doSelectTracksFromMuons;
    bool m_jp_doRemoveCaloTaggedMuons;
    bool m_jp_doSelectTracksFromElectrons;
    bool m_jp_addInDetHadrons;

    // When doSelectTracksWithLRTCuts is set to true, the addtional track cuts
    // be applied to the selected tracks to reduce the number of fake tracks in
    // the selected track collected. These cuts are inspired by the improvments that
    // were implmented for LRT Run 3.
    bool m_jp_doSelectTracksWithLRTCuts ;



private:
    int m_numEvent;         //!

    ConstDataVector<xAOD::TrackParticleContainer>* m_selectedTracks = 0; //!

    using TrackSelectionAlg = StatusCode (VSITrackSelection::*)();  //!
    std::vector<TrackSelectionAlg> m_trackSelectionAlgs; //!

    /** track selection */
    using CutFunc = bool (VSITrackSelection::*) ( const xAOD::TrackParticle* ) const;  //!
    std::vector<CutFunc> m_trackSelectionFuncs;  //!

    const xAOD::VertexContainer*  m_primaryVertices;  //!


    // variables that don't get filled at submission time should be
    // protected from being send from the submission node to the worker
    // node (done by the //!)

    // Track selection decorator for track after the track is decorated
    std::unique_ptr< SG::AuxElement::Decorator< char > > m_decor_isSelected;


public:


    // this is a standard constructor
    VSITrackSelection ();  //!

    // these are the functions inherited from Algorithm
    virtual EL::StatusCode configure();  //!
    virtual EL::StatusCode setupJob (EL::Job& job);  //!
    virtual EL::StatusCode fileExecute ();  //!
    virtual EL::StatusCode histInitialize ();  //!
    virtual EL::StatusCode changeInput (bool firstFile);  //!
    virtual EL::StatusCode initialize ();  //!
    virtual EL::StatusCode execute ();  //!
    virtual EL::StatusCode postExecute ();  //!
    virtual EL::StatusCode finalize ();  //!
    virtual EL::StatusCode histFinalize ();  //!

    // added functions not from Algorithm

    /** track-by-track selection strategies */
    bool selectTrack_notPVassociated ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_pTCut           ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_chi2Cut         ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_hitPattern      ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_hitPatternTight ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_d0Cut           ( const xAOD::TrackParticle* ) const;  //!
    bool selectTrack_z0Cut           ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_d0errCut        ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_z0errCut        ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_d0signifCut     ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_z0signifCut     ( const xAOD::TrackParticle* ) const; //!
    bool selectTrack_LRTR3Cut        ( const xAOD::TrackParticle* ) const; //!

    /** select tracks which become seeds for vertex finding */
    void selectTrack( const xAOD::TrackParticle* ); //!
    StatusCode selectTracksInDet(); //!
    StatusCode selectTracksFromMuons(); //!
    StatusCode selectTracksFromElectrons(); //!
    StatusCode selectTracksInDetHadronOverlay(); //!
    


    /// @cond
    // this is needed to distribute the algorithm to the workers
    ClassDef(VSITrackSelection, 1);
    /// @endcond
};

#endif //DHNL_VSITRACKSELECTION_H

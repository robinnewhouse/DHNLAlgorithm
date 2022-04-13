//
// Created by Sean Galantzan on 10/02/2021.
// Copied from https://gitlab.cern.ch/dtrischu/athena/-/blob/be_framework_for_HNL/Reconstruction/VKalVrt/VrtSecInclusive/src/TrackSelectionAlgs.cxx
// And from https://github.com/UCATLAS/xAODAnaHelpers/blob/master/Root/TrackSelector.cxx
// This algorithm allows the user to select tracks exactly as is done in the VSI track selection code
// This algorithm is configurable to set up any vertex configuration that the user want to run
// An addtional function has been added to save all non-lepton tracks to the output if you want to uses
        // the original non-lepton tracks in the VSI track attatchment.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "DHNLAlgorithm/VSITrackSelection.h"

#include "xAODEgamma/ElectronxAODHelpers.h"
#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"

#include <DHNLAlgorithm/DHNLFunctions.h>

#include "DVAnalysisBase/DVHelperFunctions.h"
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODAnaHelpers/HelperFunctions.h>

// #include "FactoryTools/AlgConsts.h"

#include <iostream>

#include "TH1D.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TROOT.h"

//-------------------------------------------------

namespace VKalVrtAthena {

    //____________________________________________________________________________________________________
    bool isAssociatedToVertices(const xAOD::TrackParticle *trk, const xAOD::VertexContainer *vertices ) {
        // checks whether the track is associated to a PV (this check is not correct for GSF track, but is currently what is run in data) -DT
        bool is_pv_associated = false;
        for (auto *vtx : *vertices) {
            for (size_t iv = 0; iv < vtx->nTrackParticles(); iv++) {
                auto *pvtrk = vtx->trackParticle(iv); // gets the track associated to the primary vertex
                if (trk == pvtrk) {
                    is_pv_associated = true;
                    break;
                }
            }
        }
        return is_pv_associated;
    }

    //____________________________________________________________________________________________________
    bool isAssociatedToVertices_withGSF(const xAOD::TrackParticle *trk, const xAOD::VertexContainer *vertices ) {
        // checks whether the track is associated to a PV, including the proper check for GSF tracks -DT
        bool is_pv_associated = false;
        const xAOD::TrackParticle *id_trk;
        id_trk = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(trk);

        for (auto *vtx : *vertices) {
            for (size_t iv = 0; iv < vtx->nTrackParticles(); iv++) {
                auto *pvtrk = vtx->trackParticle(iv); // gets the track associated to the primary vertex
                 if ((pvtrk != nullptr) // sometimes the PV track is a nullptr (?), skip these tracks
                     and ((id_trk == pvtrk) or (trk == pvtrk))) { // when vertexing with electron tracks, need to compare the orginal track particle from GSF to the track in the PV
                    is_pv_associated = true;
                    break;
                }
            }
        }
        return is_pv_associated;
    }


    //____________________________________________________________________________________________________
    double vtxVtxDistance(const Amg::Vector3D &v1, const Amg::Vector3D &v2) {
        return (v1 - v2).norm();
    }
}

using namespace std;

    // this is needed to distribute the algorithm to the workers
ClassImp(VSITrackSelection)

VSITrackSelection::VSITrackSelection() : Algorithm("VSITrackSelection") {
// Here you put any code for the base initialization of variables,
// e.g. initialize all pointers to 0.  Note that you should only put
// the most basic initialization here, since this method will be
// called on both the submission and the worker node.  Most of your
// initialization code will go into histInitialize() and
// initialize().
    ANA_MSG_INFO("VSITrackSelection() : Calling constructor");
}


EL::StatusCode VSITrackSelection::configure() {
    ANA_MSG_INFO("configure() : Configuring VSITrackSelection Interface.");

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode VSITrackSelection::setupJob(EL::Job &job) {
    // Here you put code that sets up the job on the submission object
    // so that it is ready to work with your algorithm, e.g. you can
    // request the D3PDReader service or add output files.  Any code you
    // put here could instead also go into the submission script.  The
    // sole advantage of putting it here is that it gets automatically
    // activated/deactivated when you add/remove the algorithm from your
    // job, which may or may not be of value to you.

    job.useXAOD ();;
    xAOD::Init("VSITrackSelection").ignore(); // call before opening first file

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode VSITrackSelection::histInitialize() {
    // Here you do everything that needs to be done at the very
    // beginning on each worker node, e.g. create histograms and output
    // trees.  This method gets called before any input files are
    // connected.
    ANA_MSG_INFO("histInitialize() : Calling histInitialize \n");

    return EL::StatusCode::SUCCESS;
}


EL::StatusCode VSITrackSelection::fileExecute() {
    // Here you do everything that needs to be done exactly once for every
    // single file, e.g. collect a list of all lumi-blocks processed
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode VSITrackSelection::changeInput(bool /*firstFile*/) {
    // Here you do everything you need to do when we change input files,
    // e.g. resetting branch addresses on trees.  If you are using
    // D3PDReader or a similar service this method is not needed.
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode VSITrackSelection::initialize() {
    // Here you do everything that you need to do after the first input
    // file has been connected and before the first event is processed,
    // e.g. create additional histograms based on which variables are
    // available in the input files.  You can also create all of your
    // histograms and trees in here, but be aware that this method
    // doesn't get called if no events are processed.  So any objects
    // you create here won't be available in the output if you have no
    // input events.

    if( m_inDetTrackParticlesContainerName.empty() || m_inElContainerName.empty() || m_inMuContainerName.empty() || m_vertexContainerName.empty() ) {
        ANA_MSG_ERROR( "InputContainer is empty!");
        return EL::StatusCode::FAILURE;
    }

    if ( m_outContainerName.empty() ) {
        ANA_MSG_ERROR( "OutputContainer is empty!");
        return EL::StatusCode::FAILURE;
    }

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();

    m_numEvent      = 0;


    // Track selection algorithm configuration
    if( m_jp_doSelectTracksFromMuons )     { m_trackSelectionAlgs.emplace_back( &VSITrackSelection::selectTracksFromMuons );     }
    if( m_jp_doSelectTracksFromElectrons ) { m_trackSelectionAlgs.emplace_back( &VSITrackSelection::selectTracksFromElectrons ); }
    // if none of the above two flags are activated, use ID tracks (default)
    if( !m_jp_doSelectTracksFromMuons && !m_jp_doSelectTracksFromElectrons ) {
        m_trackSelectionAlgs.emplace_back( &VSITrackSelection::selectTracksInDet );
    }
    if (m_jp_addInDetHadrons) {
         m_trackSelectionAlgs.emplace_back( &VSITrackSelection::selectTracksInDetHadronOverlay ); // add hadrons only to track selection
    }


    return EL::StatusCode::SUCCESS;
}

EL::StatusCode VSITrackSelection::execute() {
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.

    ++m_numEvent;

    const xAOD::EventInfo* eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));
    m_runNumber =  eventInfo->runNumber();
    m_eventNumber =  eventInfo->eventNumber();


    // get primary vertex
    ANA_CHECK( HelperFunctions::retrieve(m_primaryVertices, m_vertexContainerName, m_event, m_store, msg()) );

    // create output container (if requested) - deep copy
    m_selectedTracks = new ConstDataVector<xAOD::TrackParticleContainer>(SG::VIEW_ELEMENTS);

    // Perform track selection and store it to selectedBaseTracks
    for( auto alg : m_trackSelectionAlgs ) {
        ATH_CHECK( (this->*alg)() );
    }

    // add output container to TStore
    ANA_CHECK( m_store->record( m_selectedTracks, m_outContainerName ));

    return EL::StatusCode::SUCCESS;

}


EL::StatusCode VSITrackSelection::postExecute() {
    // Here you do everything that needs to be done after the main event
    // processing.  This is typically very rare, particularly in user
    // code.  It is mainly used in implementing the NTupleSvc.
    return EL::StatusCode::SUCCESS;
}


EL::StatusCode VSITrackSelection::finalize() {
    // This method is the mirror image of initialize(), meaning it gets
    // called after the last event has been processed on the worker node
    // and allows you to finish up any objects you created in
    // initialize() before they are written to disk.  This is actually
    // fairly rare, since this happens separately for each worker node.
    // Most of the time you want to do your post-processing on the
    // submission node after all your histogram outputs have been
    // merged.  This is different from histFinalize() in that it only
    // gets called on worker nodes that processed input events.

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode VSITrackSelection::histFinalize() {
    // This method is the mirror image of histInitialize(), meaning it
    // gets called after the last event has been processed on the worker
    // node and allows you to finish up any objects you created in
    // histInitialize() before they are written to disk.  This is
    // actually fairly rare, since this happens separately for each
    // worker node.  Most of the time you want to do your
    // post-processing on the submission node after all your histogram
    // outputs have been merged.  This is different from finalize() in
    // that it gets called on all worker nodes regardless of whether
    // they processed input events.

    return EL::StatusCode::SUCCESS;
}

//____________________________________________________________________________________________________
bool VSITrackSelection::selectTrack_d0Cut       ( const xAOD::TrackParticle* trk ) const { return ( fabs( trk->d0() ) > m_jp_d0TrkPVDstMinCut && fabs( trk->d0() ) < m_jp_d0TrkPVDstMaxCut ); }
bool VSITrackSelection::selectTrack_z0Cut       ( const xAOD::TrackParticle* trk ) const { return ( fabs( trk->z0() ) > m_jp_z0TrkPVDstMinCut && fabs( trk->z0() ) < m_jp_z0TrkPVDstMaxCut ); }
bool VSITrackSelection::selectTrack_d0errCut    ( const xAOD::TrackParticle* trk ) const { const double cov11 = trk->definingParametersCovMatrix()(0,0); return cov11 < m_jp_d0TrkErrorCut*m_jp_d0TrkErrorCut; }
bool VSITrackSelection::selectTrack_z0errCut    ( const xAOD::TrackParticle* trk ) const { const double cov22 = trk->definingParametersCovMatrix()(1,1); return cov22 < m_jp_z0TrkErrorCut*m_jp_z0TrkErrorCut; }
bool VSITrackSelection::selectTrack_d0signifCut ( const xAOD::TrackParticle*     ) const { return true; }
bool VSITrackSelection::selectTrack_z0signifCut ( const xAOD::TrackParticle*     ) const { return true; }
bool VSITrackSelection::selectTrack_pTCut       ( const xAOD::TrackParticle* trk ) const { return trk->pt() > m_jp_TrkPtCut; }
bool VSITrackSelection::selectTrack_chi2Cut     ( const xAOD::TrackParticle* trk ) const { return trk->chiSquared() / (trk->numberDoF()+1.e-9) < m_jp_TrkChi2Cut; }

//____________________________________________________________________________________________________
bool VSITrackSelection::selectTrack_hitPattern( const xAOD::TrackParticle* trk ) const {

    uint8_t PixelHits = 0;
    uint8_t SCTHits   = 0;
    uint8_t BLayHits  = 0;
    uint8_t PixShare  = 0;
    uint8_t SCTShare  = 0;
    uint8_t TRTHits   = 0;

    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SCTHits,   xAOD::numberOfSCTHits                 ) ) ) SCTHits   =0;
    if( !(trk->summaryValue( BLayHits,  xAOD::numberOfInnermostPixelLayerHits ) ) ) BLayHits  =0;
    if( !(trk->summaryValue( PixShare,  xAOD::numberOfPixelSharedHits         ) ) ) PixShare  =0;
    if( !(trk->summaryValue( SCTShare,  xAOD::numberOfSCTSharedHits           ) ) ) SCTShare  =0;
    if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits                 ) ) ) TRTHits   =0;

    uint8_t SharedHits = PixShare + SCTShare;

    // do Pixel/SCT/SiHits only if we exclude StandAlone TRT hits
    if( !m_jp_SAloneTRT ) {
        if(PixelHits           < m_jp_CutPixelHits)  return false;
        if(SCTHits             < m_jp_CutSctHits)    return false;
        if((PixelHits+SCTHits) < m_jp_CutSiHits)     return false;
        if(BLayHits            < m_jp_CutBLayHits)   return false;
        if(SharedHits          > m_jp_CutSharedHits) return false;
    }

    // The folloing part reproduces the track selection in RPVDixpVrt
    if( m_jp_doTRTPixCut ) {
        if(TRTHits == 0 && PixelHits < 2)           return false;
    }

    if( PixelHits == 0 && SCTHits < 6 ) return false;

    return true;
}

//____________________________________________________________________________________________________
bool VSITrackSelection::selectTrack_hitPatternTight( const xAOD::TrackParticle* trk ) const {
    uint8_t PixelHits = 0;
    uint8_t SCTHits   = 0;
    uint8_t TRTHits   = 0;

    if( !(trk->summaryValue( PixelHits, xAOD::numberOfPixelHits               ) ) ) PixelHits =0;
    if( !(trk->summaryValue( SCTHits,   xAOD::numberOfSCTHits                 ) ) ) SCTHits   =0;
    if( !(trk->summaryValue( TRTHits,   xAOD::numberOfTRTHits                 ) ) ) TRTHits   =0;

    if( trk->pt() > 20.e3 ) return true;

    if( SCTHits < m_jp_CutTightSCTHits                   ) return false;

    if( fabs( trk->eta() ) < 1.7 ) {
        if( TRTHits < m_jp_CutTightTRTHits ) return false;
    }

    return true;
}

//____________________________________________________________________________________________________
bool VSITrackSelection::selectTrack_notPVassociated  ( const xAOD::TrackParticle* trk ) const {
    return !( VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ) );
}

//____________________________________________________________________________________________________
bool VSITrackSelection::selectTrack_LRTR3Cut( const xAOD::TrackParticle* trk ) const {
    uint8_t npix = 0;
    trk->summaryValue(npix, xAOD::numberOfPixelHits);
    uint8_t nsct = 0;
    trk->summaryValue(nsct, xAOD::numberOfSCTHits);
    uint8_t nSiHits = npix + nsct ;
    uint8_t nSCTHoles=0;
    trk->summaryValue(nSCTHoles, xAOD::numberOfSCTHoles);

    double dTheta = std::fabs(TMath::ATan2(std::fabs(trk->d0()),trk->z0())-2*std::atan(std::exp(-1*trk->eta())));
    bool geometric_cut = dTheta < 1. || std::fabs(trk->z0()) < 200. ;

    bool z0_cut = trk->z0() <= 500. ;
    bool chi2_cut = (trk->chiSquared()/ (trk->numberDoF()+1.e-9)) <= 9. ;
    bool NSiHits_cut = nSiHits >=8 ;
    bool NSCTHits_cut = nsct >= 7 ;
    bool NSCTHoles_cut = nSCTHoles <= 1;

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": z0_cut, chi2_cut, NSiHits_cut, NSCTHits_cut, NSCTHoles_cut  = " <<z0_cut<<", "<<chi2_cut<<", "<<NSiHits_cut<<", "<<NSCTHits_cut<<", "<< NSCTHoles_cut );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": npix, nsct, nSiHits, nSCTHoles, dTheta, z0, d0, chi2  = " <<unsigned(npix)<<", "<<unsigned(nsct)<<", "<<unsigned(nSiHits)<<", "<<unsigned(nSCTHoles)<<", "<< dTheta<<", "<< trk->z0()<<", "<< trk->d0()<<", " <<trk->chiSquared() ) ;

    const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = trk->patternRecoInfo();
    bool isLRT = patternReco.test(49) ;
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track is LRT  = " << isLRT ) ;
    if (isLRT) {  // apply all cuts to LRT tracks
        return (z0_cut && chi2_cut && NSiHits_cut && NSCTHits_cut && NSCTHoles_cut && geometric_cut);
    }
    else{ // not LRT track; only apply SiHit cut
        return NSiHits_cut ;
    }
}


//____________________________________________________________________________________________________
void VSITrackSelection::selectTrack( const xAOD::TrackParticle* trk ) {

    // Setup cut functions
    if( 0 == m_trackSelectionFuncs.size() && !m_jp_passThroughTrackSelection ) {

        // These cuts are optional. Specified by JobProperty
        if( m_jp_do_PVvetoCut )             m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_notPVassociated );
        if( m_jp_do_d0Cut )                 m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_d0Cut );
        if( m_jp_do_z0Cut )                 m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_z0Cut );
        if( m_jp_do_d0errCut )              m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_d0errCut );
        if( m_jp_do_z0errCut )              m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_z0errCut );
        if (m_jp_doSelectTracksWithLRTCuts) m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_LRTR3Cut );
       

        // These cuts are used by default
        m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_hitPattern );
        m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_hitPatternTight );
        m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_chi2Cut );
        m_trackSelectionFuncs.emplace_back( &VSITrackSelection::selectTrack_pTCut );

    }

    if(  std::find( m_selectedTracks->begin(), m_selectedTracks->end(), trk ) != m_selectedTracks->end() ) return;

    std::vector<bool> cutBits;

    for( auto func : m_trackSelectionFuncs ) cutBits.emplace_back( (this->*func)( trk ) );

    // Good track should not find any false bit
    bool isGood_standard = ( std::find( cutBits.begin(), cutBits.end(), false ) == cutBits.end() );

    if( isGood_standard ) {
        trk->auxdecor<bool>("be_toSave") = true;
        if (m_jp_doSelectTracksFromElectrons) {  
            const xAOD::TrackParticle *id_tr;
            id_tr = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(trk);
            if (id_tr != nullptr){
                // do not decorte inner detector particle again if the muon was already decorated.
                if (id_tr->isAvailable<bool>("be_toSave") ) {
                    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ":  ID track matched to selected GSF track be_toSave: " << id_tr->auxdecor<bool>("be_toSave") );
                }
                if ( !id_tr->isAvailable<bool>("be_toSave") or (id_tr->isAvailable<bool>("be_toSave") and !id_tr->auxdecor<bool>("be_toSave")) ) { 
                ATH_MSG_DEBUG( " > " << __FUNCTION__ << ":  Decorating the ID track matched to selected GSF track!" );
                id_tr->auxdecor<bool>("be_toSave") = true; 
                }
            }
        }
        
        // Store the selected track to the new m_selectedTracks
        // Here we firstly need to register the empty pointer to the m_selectedTracks,
        // then need to do deep copy after then. This is the feature of xAOD.

        m_selectedTracks->emplace_back( trk );
    } else{ trk->auxdecor<bool>("be_toSave") = false;}

}

//____________________________________________________________________________________________________
StatusCode  VSITrackSelection::selectTracksInDet() {

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": begin"  );

    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //--------------------------------------------------------

    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_inDetTrackParticlesContainerName) );

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );

    // Loop over tracks
    for( auto *trk : *trackParticleContainer ) {
        selectTrack( trk );

        if (trk->isAvailable<bool>("be_toSave") && trk->auxdecor<bool>("be_toSave")) {
            // This is either a muon or an electron, which were already added.
            continue;
        }
        const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = trk->patternRecoInfo();
        bool isLRT = patternReco.test(49) ;
        TLorentzVector p4 = trk->p4();

        trk->auxdecor<bool>("be_isLRT") = isLRT;
        trk->auxdecor<int>("be_type") = (int) TrackType::NON_LEPTON;
        trk->auxdecor<int>("be_quality") = -999;
        trk->auxdecor<int>("be_muonType") = -999;
        trk->auxdecor<float_t>("be_vx") = trk->vx();
        trk->auxdecor<float_t>("be_vy") = trk->vy();
        trk->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = trk->definingParametersCovMatrixVec();
        trk->auxdecor<float_t>("be_beamlineTiltX") = trk->beamlineTiltX();
        trk->auxdecor<float_t>("be_beamlineTiltY") = trk->beamlineTiltY();
        trk->auxdecor<uint32_t>("be_hitPattern") = trk->hitPattern();
        trk->auxdecor<Double_t>("be_px") = p4.Px();
        trk->auxdecor<Double_t>("be_py") = p4.Py();
        trk->auxdecor<Double_t>("be_pz") = p4.Pz();
        trk->auxdecor<Double_t>("be_e") = p4.E();
        trk->auxdecor<uint32_t>("be_runNumber") = m_runNumber;
        trk->auxdecor<unsigned long long>("be_eventNumber") = m_eventNumber;
        trk->auxdecor<bool>("be_fromPV") = VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ); // this is the info that VSI currently uses to veto "PV" tracks
        // track is not a lepton so no lepton quality information
        trk->auxdecor<int>("be_isTight") = -1;
        trk->auxdecor<int>("be_isMedium") = -1;
        trk->auxdecor<int>("be_isLoose") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose_mod1") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose_modSi") = -1;


    }

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total ID tracks   = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );

    return StatusCode::SUCCESS;
}


//____________________________________________________________________________________________________
StatusCode  VSITrackSelection::selectTracksInDetHadronOverlay() {

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": begin"  );

    //--------------------------------------------------------
    //  Extract tracks from xAOD::TrackParticle container
    //--------------------------------------------------------

    const xAOD::TrackParticleContainer* trackParticleContainer ( nullptr );
    ATH_CHECK( evtStore()->retrieve( trackParticleContainer, m_inDetTrackParticlesContainerName) );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Retrieving Track Collection =" << m_inDetTrackParticlesContainerName );

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Extracted xAOD::TrackParticle number=" << trackParticleContainer->size() );

    // Loop over tracks
    for( auto *trk : *trackParticleContainer ) { 

        if (trk->isAvailable<bool>("be_toSave") and trk->auxdecor<bool>("be_toSave") ) {
            // This is either a muon or an electron, which were already added.
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track type = " << trk->auxdecor<int>("be_type"));
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Skipping Already Selected InDetTrack (pt,eta,phi) = " << trk->pt() << " , "<< trk->eta() <<" , " << trk->phi());
            continue;
        } 
        // check to make sure you dont add a track already in the selected tracks function. 
        if(  std::find( m_selectedTracks->begin(), m_selectedTracks->end(), trk ) != m_selectedTracks->end() ) continue; 
       
        TLorentzVector p4 = trk->p4();
        const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = trk->patternRecoInfo();
        bool isLRT = patternReco.test(49) ;

        // decorate hadron tracks with the information we need in the background framework
        trk->auxdecor<bool>("be_toSave") = true; // saving all hadrons if we're running this function!
        trk->auxdecor<bool>("be_isLRT") = isLRT;
        trk->auxdecor<int>("be_type") = (int) TrackType::NON_LEPTON;
        trk->auxdecor<int>("be_quality") = -999;
        trk->auxdecor<int>("be_muonType") = -999;
        trk->auxdecor<float_t>("be_vx") = trk->vx();
        trk->auxdecor<float_t>("be_vy") = trk->vy();
        trk->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = trk->definingParametersCovMatrixVec();
        trk->auxdecor<float_t>("be_beamlineTiltX") = trk->beamlineTiltX();
        trk->auxdecor<float_t>("be_beamlineTiltY") = trk->beamlineTiltY();
        trk->auxdecor<uint32_t>("be_hitPattern") = trk->hitPattern();
        trk->auxdecor<Double_t>("be_px") = p4.Px();
        trk->auxdecor<Double_t>("be_py") = p4.Py();
        trk->auxdecor<Double_t>("be_pz") = p4.Pz();
        trk->auxdecor<Double_t>("be_e") = p4.E();
        trk->auxdecor<uint32_t>("be_runNumber") = m_runNumber;
        trk->auxdecor<unsigned long long>("be_eventNumber") = m_eventNumber;
        trk->auxdecor<bool>("be_fromPV") = VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ); // this is the info that VSI currently uses to veto "PV" tracks
        // track is not a lepton so no lepton quality information
        trk->auxdecor<int>("be_isTight") = -1;
        trk->auxdecor<int>("be_isMedium") = -1;
        trk->auxdecor<int>("be_isLoose") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose_mod1") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose_modSi") = -1;


        m_selectedTracks->emplace_back( trk );
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Added Hadron (pt,eta,phi) = " << trk->pt() << " , "<< trk->eta() <<" , " << trk->phi());
        ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track Type Hadron = " << trk->auxdecor<int>("be_type") );
    }


    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total ID tracks   = " << trackParticleContainer->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );

    return StatusCode::SUCCESS;
}


//____________________________________________________________________________________________________
StatusCode  VSITrackSelection::selectTracksFromMuons() {

    const xAOD::MuonContainer* muons ( nullptr );
    ATH_CHECK( evtStore()->retrieve( muons, m_inMuContainerName) );


    for( const auto& muon : *muons ) {
        const auto* trk = muon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle );

        if( !trk ) continue;
        // skip calo-tagged muons when selecting muons
        if (m_jp_doRemoveCaloTaggedMuons) {
            if (muon->muonType() == xAOD::Muon::CaloTagged) continue;
        }
        const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = trk->patternRecoInfo();
        bool isLRT = patternReco.test(49) ;
        TLorentzVector p4 = muon->p4();
        
        // decorate the muon tracks with the information we need in the background framework
        trk->auxdecor<bool>("be_isLRT") = isLRT;
        trk->auxdecor<int>("be_quality") = (int) muon->quality();
        trk->auxdecor<int>("be_type") = (int) TrackType::MUON;
        trk->auxdecor<int>("be_muonType") = muon->muonType();
        trk->auxdecor<float_t>("be_vx") = trk->vx();
        trk->auxdecor<float_t>("be_vy") = trk->vy();
        trk->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = trk->definingParametersCovMatrixVec();
        trk->auxdecor<float_t>("be_beamlineTiltX") = trk->beamlineTiltX();
        trk->auxdecor<float_t>("be_beamlineTiltY") = trk->beamlineTiltY();
        trk->auxdecor<uint32_t>("be_hitPattern") = trk->hitPattern();
        trk->auxdecor<Double_t>("be_px") = p4.Px();
        trk->auxdecor<Double_t>("be_py") = p4.Py();
        trk->auxdecor<Double_t>("be_pz") = p4.Pz();
        trk->auxdecor<Double_t>("be_e") = p4.E();
        trk->auxdecor<uint32_t>("be_runNumber") = m_runNumber;
        trk->auxdecor<unsigned long long>("be_eventNumber") = m_eventNumber;
        trk->auxdecor<bool>("be_fromPV") = VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ); // this is the info that VSI currently uses to veto "PV" tracks
        trk->auxdecor<bool>("be_prompt_lepton") = VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ); // this is the actual information about whether the lepton is a PV
        //Add default lepton quality information 
        trk->auxdecor<int>("be_isTight") = ((int)muon->auxdataConst<char>("isTightQ") );
        trk->auxdecor<int>("be_isMedium") = ((int)muon->auxdataConst<char>("isMediumQ") );
        trk->auxdecor<int>("be_isLoose") = ((int)muon->auxdataConst<char>("isLooseQ") );
        // track is not an electron so no custom quality 
        trk->auxdecor<int>("be_isLHVeryLoose") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose_mod1") = -1;
        trk->auxdecor<int>("be_isLHVeryLoose_modSi") = -1;



        selectTrack( trk );
        
        // if we selected the track then the toSave bool is true
        if (trk->isAvailable<bool>("be_toSave") and trk->auxdecor<bool>("be_toSave") ) {
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Selected Muon (pt,eta,phi) = " << trk->pt() << " , "<< trk->eta() <<" , " << trk->phi());
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track Type Muon = " << trk->auxdecor<int>("be_type") );
        } 
        

    }

    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total muons       = " << muons->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );

    return StatusCode::SUCCESS;
}


//____________________________________________________________________________________________________
StatusCode  VSITrackSelection::selectTracksFromElectrons() {

    const xAOD::ElectronContainer *electrons( nullptr );
    ATH_CHECK( evtStore()->retrieve( electrons, m_inElContainerName ) );

    for( const auto& electron : *electrons ) {
        if( 0 == electron->nTrackParticles() ) continue;

        // The first track is the best-matched track
        const auto* trk = electron->trackParticle(0);
        if( !trk ) continue;
        const std::bitset<xAOD::NumberOfTrackRecoInfo> patternReco = trk->patternRecoInfo();
        bool isLRT = patternReco.test(49) ;
        TLorentzVector p4 = electron->p4();

        // decorate the electron tracks with the information we need in the background framework
        trk->auxdecor<bool>("be_isLRT") = isLRT;
        trk->auxdecor<int>("be_quality") = -998;
        trk->auxdecor<int>("be_type") = (int) TrackType::ELECTRON;
        trk->auxdecor<int>("be_muonType") = -999;
        trk->auxdecor<float_t>("be_vx") = trk->vx();
        trk->auxdecor<float_t>("be_vy") = trk->vy();
        trk->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = trk->definingParametersCovMatrixVec();
        trk->auxdecor<float_t>("be_beamlineTiltX") = trk->beamlineTiltX();
        trk->auxdecor<float_t>("be_beamlineTiltY") = trk->beamlineTiltY();
        trk->auxdecor<uint32_t>("be_hitPattern") = trk->hitPattern();
        trk->auxdecor<Double_t>("be_px") = p4.Px();
        trk->auxdecor<Double_t>("be_py") = p4.Py();
        trk->auxdecor<Double_t>("be_pz") = p4.Pz();
        trk->auxdecor<Double_t>("be_e") = p4.E();
        trk->auxdecor<uint32_t>("be_runNumber") = m_runNumber;
        trk->auxdecor<unsigned long long>("be_eventNumber") = m_eventNumber;
        trk->auxdecor<bool>("be_fromPV") = VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ); // this is the info that VSI currently uses to veto "PV" tracks
        trk->auxdecor<bool>("be_prompt_lepton") = VKalVrtAthena::isAssociatedToVertices_withGSF( trk, m_primaryVertices ); // this is the actual information about whether the lepton is a PV
        //Add default lepton quality information 
        trk->auxdecor<int>("be_isTight") = (int)electron->auxdataConst<char>("LHTight");
        trk->auxdecor<int>("be_isMedium") = (int)electron->auxdataConst<char>("LHMedium");
        trk->auxdecor<int>("be_isLoose") = (int)electron->auxdataConst<char>("LHLoose");
        // custom electron WP currently decorated in MiniTree FillElectronUsers, this is not ideal,  
        // but I couldnt figure out how to initialize the electron likelihood tool in this code... -DT


        selectTrack( trk );

         // if we selected the track then the toSave bool is true
        if (trk->isAvailable<bool>("be_toSave") and trk->auxdecor<bool>("be_toSave") ) {
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Selected GSF Electron (pt,eta,phi) = " << trk->pt() << " , "<< trk->eta() <<" , " << trk->phi());
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Track Type Electron = " << trk->auxdecor<int>("be_type") );
            const xAOD::TrackParticle *id_tr;
            id_tr = xAOD::EgammaHelpers::getOriginalTrackParticleFromGSF(trk);
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Selected InDetTrack matched GSF electron (pt,eta,phi) = " << id_tr->pt() << " , "<< id_tr->eta() <<" , " << id_tr->phi());
            ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": InDetTrack matched to GSF electron is selected = " << id_tr->auxdecor<bool>("be_toSave") );
 
        }     
    }


    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of total electrons   = " << electrons->size() );
    ATH_MSG_DEBUG( " > " << __FUNCTION__ << ": Number of selected tracks   = " << m_selectedTracks->size() );

    return StatusCode::SUCCESS;
}



#include <DHNLAlgorithm/DHNLAlgorithm.h>

#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include "AthContainers/ConstDataVector.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include <DHNLAlgorithm/DHNLFunctions.h>
#include "DVAnalysisBase/DVHelperFunctions.h"
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODTruth/TruthVertex.h>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include <PMGTools/PMGCrossSectionTool.h>
#include <SampleHandler/MetaFields.h>
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODEgamma/ElectronxAODHelpers.h>

#include "TFile.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

static float GeV = 1000.;

// this is needed to distribute the algorithm to the workers
ClassImp(DHNLAlgorithm)


DHNLAlgorithm::DHNLAlgorithm() :
        m_cutflowHist(nullptr),
        m_cutflowHistW(nullptr) {
// Here you put any code for the base initialization of variables,
// e.g. initialize all pointers to 0.  This is also where you
// declare all properties for your algorithm.  Note that things like
// resetting statistics variables or booking histograms should
// rather go into the initialize() function.


    ANA_MSG_INFO("DHNLAlgorithm() : Calling constructor");
	


    m_inJetContainerName = "";
    m_inputAlgo = "";
    m_allJetContainerName = "";
    m_allJetInputAlgo = "";
    m_inMETContainerName = "";
    m_inMETTrkContainerName = "";
    m_secondaryVertexContainerNameList = "";
    m_inDetTrackParticlesContainerName = "InDetTrackParticles";
    m_msgLevel = MSG::INFO;
    m_useCutFlow = true;
    m_MCPileupCheckContainer = "AntiKt4TruthJets";
    m_leadingJetPtCut = 225;
    m_subleadingJetPtCut = 225;
    m_jetMultiplicity = 3;
    m_truthLevelOnly = false;
    m_backgroundEstimationBranches = false;
    m_backgroundEstimationNoParticleData = false;
    m_doInverseLeptonControlRegion = false;
    m_metCut = 0;

}


EL::StatusCode DHNLAlgorithm::initialize() {
    // Here you do everything that needs to be done at the very
    // beginning on each worker node, e.g. create histograms and output
    // trees.  This method gets called before any input files are
    // connected.

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();
    // m_eventCounter = -1;

    const xAOD::EventInfo* eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));

    // if ( this->configure() == EL::StatusCode::FAILURE ) {
    //     ANA_MSG_ERROR("initialize() : Failed to properly configure. Exiting." );
    //     return EL::StatusCode::FAILURE;
    // }

    // m_truthLevelOnly is set in config so need to do this after configure is called
    if( m_truthLevelOnly ) { m_isMC = true; }
    else {
        m_isMC = ( eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ) ? true : false;
    }

    uint32_t dsid= m_isMC ? eventInfo->mcChannelNumber() : -1;

    //
    // x-sec tool
    ANA_CHECK( ASG_MAKE_ANA_TOOL(m_PMGCrossSectionTool_handle, PMGTools::PMGCrossSectionTool) );
    ANA_CHECK( m_PMGCrossSectionTool_handle.retrieve() );
    m_PMGCrossSectionTool_handle->readInfosFromDir(PathResolverFindCalibDirectory("dev/PMGTools"));
    std::vector<int> loadedDSIDs=m_PMGCrossSectionTool_handle->getLoadedDSIDs();
    
    m_weight_xs = 1.;
    if (m_isMC) {
        double xs;
        double eff;
        double kfactor;
        if (std::find(loadedDSIDs.begin(), loadedDSIDs.end(), dsid) != loadedDSIDs.end()) { // Have data, use it
            xs = m_PMGCrossSectionTool_handle->getAMIXsection(dsid);
            eff = m_PMGCrossSectionTool_handle->getFilterEff(dsid);
            kfactor = m_PMGCrossSectionTool_handle->getKfactor(dsid);
        } else { // Try sample information
            xs = wk()->metaData()->castDouble(SH::MetaFields::crossSection, 1);
            eff = wk()->metaData()->castDouble(SH::MetaFields::filterEfficiency, 1);
            kfactor = wk()->metaData()->castDouble(SH::MetaFields::kfactor, 1);
        }
        m_weight_xs = xs * eff * kfactor;
    }

    return EL::StatusCode::SUCCESS;
}


const double D0_CUT = 3.0;
const double Z0_SIN_THETA_CUT = 0.5;

EL::StatusCode DHNLAlgorithm::eventSelection() {

    int nPromptMuons = 0;
    int nPromptElectrons = 0;

    if (m_doInverseLeptonControlRegion) {

        ANA_MSG_DEBUG ("in eventSelection:m_doInverseLeptonControlRegion. Inspecting muons.");
        const xAOD::MuonContainer *inMuons = nullptr;
        ANA_CHECK(HelperFunctions::retrieve(inMuons, m_inMuContainerName, m_event, m_store, msg()));
        for (const xAOD::Muon *muon : *inMuons) {

            // Check that a muon has at lest some quality
            if (not(muon->quality() == xAOD::Muon_v1::Quality::Loose ||
                    muon->quality() == xAOD::Muon_v1::Quality::Medium ||
                    muon->quality() == xAOD::Muon_v1::Quality::Tight)) {
                ANA_MSG_DEBUG ("Muon doesn't satisfy any quality. Ignore this muon.");
                continue;
            }
            // 	check that the muon satisfies prompt lepton requirements
            const xAOD::TrackParticle *muonPrimaryTrackParticle = muon->primaryTrackParticle();
            if (muonPrimaryTrackParticle == nullptr) {
                ANA_MSG_DEBUG ("Muon primary track particle not found. Ignore this muon.");
                continue;
            }
            static SG::AuxElement::Accessor<float> z0sinthetaAcc("z0sintheta");
            if (not z0sinthetaAcc.isAvailable(*muon)) {
                ANA_MSG_DEBUG ("Muon z0sintheta not available. Ignore this muon.");
                continue;
            }
            if ((abs(muonPrimaryTrackParticle->d0()) < D0_CUT) and
                (abs(z0sinthetaAcc(*muon)) < Z0_SIN_THETA_CUT)) {
                nPromptMuons++;
                ANA_MSG_DEBUG ("Quality prompt muon found. The event is outside of the control region. Skipping.");
                wk()->skipEvent();
            }
        } // for (const xAOD::Muon *muon : *inMuons)

        ANA_MSG_DEBUG ("in eventSelection:m_doInverseLeptonControlRegion. Inspecting electrons.");
        const xAOD::ElectronContainer *inElectrons = nullptr;
        ANA_CHECK(HelperFunctions::retrieve(inElectrons, m_inElContainerName, m_event, m_store, msg()));
        for (const xAOD::Electron *electron : *inElectrons) {

            // Check that an electron has at lest some quality
            // Quality checking is different than for muons. Of course.
            bool passLHLoose = false;
            bool passLHMedium = false;
            bool passLHTight = false;
            electron->passSelection(passLHLoose, "LHLoose");
            electron->passSelection(passLHMedium, "LHMedium");
            electron->passSelection(passLHTight, "LHTight");
            if (not(passLHLoose || passLHMedium || passLHTight)) {
                ANA_MSG_DEBUG ("Electron doesn't satisfy any quality. Ignore this electron.");
                continue;
            }
            // 	check that the electron satisfies prompt lepton requirements
            // This is how the track particle is retrieved in xAODAnaHelpers::ElectronContainer
            const xAOD::TrackParticle *electronTrackParticle = electron->trackParticle();
            if (electronTrackParticle == nullptr) {
                ANA_MSG_DEBUG ("Electron track particle not found. Ignore this electron.");
                continue;
            }
            static SG::AuxElement::Accessor<float> z0sinthetaAcc("z0sintheta");
            if (not z0sinthetaAcc.isAvailable(*electron)) {
                ANA_MSG_DEBUG ("Electron z0sintheta not available. Ignore this electron.");
                continue;
            }
            if ((abs(electronTrackParticle->d0()) < D0_CUT) and
                (abs(z0sinthetaAcc(*electron)) < Z0_SIN_THETA_CUT)) {
                nPromptElectrons++;
                ANA_MSG_DEBUG ("Quality prompt electron found. The event is outside of the control region. Skipping.");
                wk()->skipEvent();
            }
        } // for (const xAOD::Electron *electron : *inElectrons)
        ANA_MSG_DEBUG ("Prompt leptons with at least 'Loose' or 'LHLoose' quality found: " << nPromptMuons + nPromptElectrons);
        ANA_MSG_DEBUG ("Event accepted for control region: ");
    }

    return EL::StatusCode::SUCCESS;

}


EL::StatusCode DHNLAlgorithm::execute() {
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.

    ANA_MSG_DEBUG("execute() : Get Containers");

    // retrieve the eventInfo object from the event store
    const xAOD::EventInfo *eventInfo = nullptr;
    ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

    // print out run and event number from retrieved object
    ANA_MSG_DEBUG ("in execute, runNumber = " << eventInfo->runNumber() << ", eventNumber = " << eventInfo->eventNumber());
    // if (eventInfo->eventNumber() % 100 == 0)
    //     ANA_MSG_INFO ("in execute, runNumber = " << eventInfo->runNumber() << ", eventNumber = " << eventInfo->eventNumber());

    //////////////////// Apply event selection including control region if requested //////////////////////
    ANA_CHECK(eventSelection());

    //////////////////// Calculate event weight //////////////////////
    if (m_isMC) {
        m_mcEventWeight = eventInfo->mcEventWeight();
    } else {
        m_mcEventWeight = 1.0;
    }

    eventInfo->auxdecor<float>("weight_xs") = m_weight_xs;
    eventInfo->auxdecor<float>("weight") = m_mcEventWeight * m_weight_xs;


    //////////////////// Store lepton information //////////////////////

    const xAOD::MuonContainer *inMuons = nullptr;
    if (!m_inMuContainerName.empty()) ANA_CHECK(HelperFunctions::retrieve(inMuons, m_inMuContainerName, m_event, m_store, msg()));

    const xAOD::ElectronContainer *inElectrons = nullptr;
    if(!m_inElContainerName.empty())
        ANA_CHECK(HelperFunctions::retrieve(inElectrons, m_inElContainerName, m_event, m_store, msg()));
    
    /*const xAOD::VertexContainer *inVSIVertices = nullptr;
    if(!m_inVSIContainerName.empty())
        ANA_CHECK(HelperFunctions::retrieve(inVSIVertices, m_inVSIContainerName, m_event, m_store, msg()));
	*/
	
	
    int MuonsPerEvent = 0;
    int ElectronsPerEvent = 0;
    
    // Copy over the aux data containing filter pass information
    // We think this should be done automatically in the shallow copy od MuonCalibrator.cxx, but it appears not to be.
    // Be careful with these hardcoded collection names.
    const xAOD::MuonContainer *inMuonsUncalibrated = nullptr;
    if (!m_backgroundEstimationNoParticleData) ANA_CHECK(HelperFunctions::retrieve(inMuonsUncalibrated, "Muons", m_event, m_store, msg()));

    const xAOD::ElectronContainer *inElectronsUncalibrated = nullptr;
    if (!m_backgroundEstimationNoParticleData) ANA_CHECK(HelperFunctions::retrieve(inElectronsUncalibrated, "Electrons", m_event, m_store, msg()));

    const xAOD::TrackParticleContainer *tracks = nullptr;
    if (!m_backgroundEstimationNoParticleData) ANA_CHECK (HelperFunctions::retrieve(tracks, m_inDetTrackParticlesContainerName, m_event, m_store));

    if (inMuons) {
        for (const xAOD::Muon *muon : *inMuons) {
            muon->auxdecor<int>("index") = muon->index();
            muon->auxdecor<int>("type") = muon->muonType();
            muon->auxdecor<float>("px") = muon->p4().Px() / GeV;
            muon->auxdecor<float>("py") = muon->p4().Py() / GeV;
            muon->auxdecor<float>("pz") = muon->p4().Pz() / GeV;
		    MuonsPerEvent+=1;
//        muon->auxdecor<float>("ptC30") = muon->isolation(xAOD::Iso::ptcone30);
            if (not(m_inMuContainerName == "Muons")) {
                muon->auxdecor<bool>("passesPromptCuts") = inMuonsUncalibrated->at(muon->index())->auxdecor<bool>("passesPromptCuts");
                muon->auxdecor<bool>("passesDisplacedCuts") = inMuonsUncalibrated->at(muon->index())->auxdecor<bool>("passesDisplacedCuts");
            }

            float chi2;
            if (not muon->parameter(chi2, xAOD::Muon::msInnerMatchChi2)) {
                chi2 = -1;
            }
            muon->auxdecor<float>("chi2") = chi2;

            int msInnerMatchDOF;
            if (not muon->parameter(msInnerMatchDOF, xAOD::Muon::msInnerMatchDOF)) {
                msInnerMatchDOF = -1;
            }
            muon->auxdecor<int>("msDOF") = msInnerMatchDOF;

            if (muon->primaryTrackParticle()->isAvailable<unsigned long>("patternRecoInfo")) {
                muon->auxdecor<bool>("isLRT") = muon->primaryTrackParticle()->patternRecoInfo().test(xAOD::SiSpacePointsSeedMaker_LargeD0);
            }
        }
    }

    if (inElectrons) {
        for (const xAOD::Electron *electron : *inElectrons) {
            electron->auxdecor<int>("index") = electron->index();
            electron->auxdecor<float>("px") = electron->p4().Px() / GeV;
            electron->auxdecor<float>("py") = electron->p4().Py() / GeV;
            electron->auxdecor<float>("pz") = electron->p4().Pz() / GeV;
			ElectronsPerEvent+=1;
            if (not(m_inElContainerName == "Electrons")) {
                electron->auxdecor<bool>("passesPromptCuts") = inElectronsUncalibrated->at(electron->index())->auxdecor<bool>("passesPromptCuts");
                electron->auxdecor<bool>("passesDisplacedCuts") = inElectronsUncalibrated->at(electron->index())->auxdecor<bool>("passesDisplacedCuts");
            }
        }
    }

    //////////////////// Store track information (for Background Estimation) //////////////////////

    TLorentzVector p4;
    if (m_backgroundEstimationBranches) {
        // muon tracks
        for (const xAOD::Muon *muon : *inMuons) {
            // susy15 derivation does not keep sufficient track information for SiliconAssociatedForwardMuon types
            if (muon->muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) {
                const xAOD::TrackParticle *track = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
                if (track == nullptr) continue;

                track->auxdecor<bool>("be_toSave") = false;
                track->auxdecor<int>("be_type") = (int) TrackType::MUON;

                continue;
            };

            const xAOD::TrackParticle *track = muon->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

            if (track == nullptr) continue;

            // Quality data
            track->auxdecor<int>("be_quality") = (int) muon->quality();

            track->auxdecor<bool>("be_toSave") = true;
            track->auxdecor<int>("be_type") = (int) TrackType::MUON;

            track->auxdecor<float_t>("be_vx") = track->vx();
            track->auxdecor<float_t>("be_vy") = track->vy();
            track->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = track->definingParametersCovMatrixVec();

            track->auxdecor<float_t>("be_beamlineTiltX") = track->beamlineTiltX();
            track->auxdecor<float_t>("be_beamlineTiltY") = track->beamlineTiltY();

            track->auxdecor<uint32_t>("be_hitPattern") = track->hitPattern();

            p4 = muon->p4();
            track->auxdecor<Double_t>("be_px") = p4.Px();
            track->auxdecor<Double_t>("be_py") = p4.Py();
            track->auxdecor<Double_t>("be_pz") = p4.Pz();
            track->auxdecor<Double_t>("be_e") = p4.E();

            track->auxdecor<uint32_t>("be_runNumber") = eventInfo->runNumber();
            track->auxdecor<unsigned long long>("be_eventNumber") = eventInfo->eventNumber();
            track->auxdecor<bool>("be_fromPV") = false;
        
            //MuonsPerEvent+=1;
        }
		

        // electron tracks
        for (const xAOD::Electron *electron : *inElectrons) {
            const xAOD::TrackParticle *track = xAOD::EgammaHelpers::getOriginalTrackParticle(electron);

            if (track == nullptr) continue;

            // Quality data
            track->auxdecor<int>("be_quality") = -998;

            track->auxdecor<bool>("be_toSave") = true;
            track->auxdecor<int>("be_type") = (int) TrackType::ELECTRON;

            track->auxdecor<float_t>("be_vx") = track->vx();
            track->auxdecor<float_t>("be_vy") = track->vy();
            track->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = track->definingParametersCovMatrixVec();

            track->auxdecor<float_t>("be_beamlineTiltX") = track->beamlineTiltX();
            track->auxdecor<float_t>("be_beamlineTiltY") = track->beamlineTiltY();

            track->auxdecor<uint32_t>("be_hitPattern") = track->hitPattern();

            p4 = electron->p4();
            track->auxdecor<Double_t>("be_px") = p4.Px();
            track->auxdecor<Double_t>("be_py") = p4.Py();
            track->auxdecor<Double_t>("be_pz") = p4.Pz();
            track->auxdecor<Double_t>("be_e") = p4.E();

            track->auxdecor<uint32_t>("be_runNumber") = eventInfo->runNumber();
            track->auxdecor<unsigned long long>("be_eventNumber") = eventInfo->eventNumber();
            track->auxdecor<bool>("be_fromPV") = false;
        
            //ElectronsPerEvent+=1;
        }

        // non-lepton tracks
        for (const xAOD::TrackParticle *track : *tracks) {
            if (track->isAvailable<bool>("be_toSave") && track->auxdecor<bool>("be_toSave")) {
                // This is either a muon or an electron, which were already added.
                continue;
            }

            track->auxdecor<bool>("be_toSave") = true;
            track->auxdecor<int>("be_type") = (int) TrackType::NON_LEPTON;
            track->auxdecor<int>("be_quality") = -999;

            track->auxdecor<float_t>("be_vx") = track->vx();
            track->auxdecor<float_t>("be_vy") = track->vy();
            track->auxdecor<std::vector< float >>("be_definingParametersCovMatrixVec")  = track->definingParametersCovMatrixVec();

            track->auxdecor<float_t>("be_beamlineTiltX") = track->beamlineTiltX();
            track->auxdecor<float_t>("be_beamlineTiltY") = track->beamlineTiltY();

            track->auxdecor<uint32_t>("be_hitPattern") = track->hitPattern();

            p4 = track->p4();
            track->auxdecor<Double_t>("be_px") = p4.Px();
            track->auxdecor<Double_t>("be_py") = p4.Py();
            track->auxdecor<Double_t>("be_pz") = p4.Pz();
            track->auxdecor<Double_t>("be_e") = p4.E();

            track->auxdecor<uint32_t>("be_runNumber") = eventInfo->runNumber();
            track->auxdecor<unsigned long long>("be_eventNumber") = eventInfo->eventNumber();
            track->auxdecor<bool>("be_fromPV") = false;

        }   
	}
    
	std::string secondaryVertexContainerName_token;
    std::istringstream sv(m_secondaryVertexContainerNameList);
	while ( std::getline(sv, secondaryVertexContainerName_token, ',') ) {
		const xAOD::VertexContainer *inVSIVertices = nullptr;
		ANA_CHECK(HelperFunctions::retrieve(inVSIVertices, secondaryVertexContainerName_token, m_event, m_store, msg()));
		for (const xAOD::Vertex *vertex: *inVSIVertices){
            vertex->auxdecor<int>("Muons_Per_Event") = MuonsPerEvent;
            vertex->auxdecor<int>("Electrons_Per_Event") = ElectronsPerEvent;

            // check if this vertex contains tracks from different original events (shuffled vertex)
            vertex->auxdecor<bool>("shuffled") = false;
            if (vertex->trackParticle(0)->isAvailable<int>("trackOriginalRun") &&
                vertex->trackParticle(0)->auxdataConst<int>("trackOriginalEvent") &&
                vertex->trackParticle(1)->auxdataConst<int>("trackOriginalRun") &&
                vertex->trackParticle(1)->auxdataConst<int>("trackOriginalEvent")) {

                int runNr_0 = vertex->trackParticle(0)->auxdataConst<int>("trackOriginalRun");
                int evtNr_0 = vertex->trackParticle(0)->auxdataConst<int>("trackOriginalEvent");

                int runNr_1 = vertex->trackParticle(1)->auxdataConst<int>("trackOriginalRun");
                int evtNr_1 = vertex->trackParticle(1)->auxdataConst<int>("trackOriginalEvent");

                vertex->auxdecor<bool>("shuffled") = (runNr_0 != runNr_1 || evtNr_0 != evtNr_1);
            }
		}
	}

    //////////////////// Store primary vertex information //////////////////////

    SG::AuxElement::ConstAccessor<float> NPVAccessor("NPV");
    const xAOD::VertexContainer *vertices = nullptr;
    if (!m_truthLevelOnly && !m_backgroundEstimationNoParticleData) {
        ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));
    }
    if (!m_truthLevelOnly && !NPVAccessor.isAvailable(*eventInfo) && vertices) { // NPV might already be available
        // number of PVs with 2 or more tracks
        //eventInfo->auxdecor< int >( "NPV" ) = HelperFunctions::countPrimaryVertices(vertices, 2);
        // TMP for JetUncertainties uses the same variable
        eventInfo->auxdecor<float>("NPV") = HelperFunctions::countPrimaryVertices(vertices, 2);
    }

    if (m_backgroundEstimationBranches) {
        // This logic is similar to https://gitlab.cern.ch/dtrischu/athena/-/blob/vrtSecInclusive-21.2-hnl/Reconstruction/VKalVrt/VrtSecInclusive/src/Utilities.cxx#L37 so we could later filter only tracks that are not from PV before shuffling
        const xAOD::Vertex *primaryVertex = HelperFunctions::getPrimaryVertex(vertices, msg());
        if(primaryVertex){
            for( size_t iv = 0; iv < primaryVertex->nTrackParticles(); iv++ ) {
                auto* pvtrk = primaryVertex->trackParticle( iv );
                if (pvtrk)
                    pvtrk->auxdecor<bool>("be_fromPV") = true;
            }
        }
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLAlgorithm::finalize() {
    // This method is the mirror image of initialize(), meaning it gets
    // called after the last event has been processed on the worker node
    // and allows you to finish up any objects you created in
    // initialize() before they are written to disk.  This is actually
    // fairly rare, since this happens separately for each worker node.
    // Most of the time you want to do your post-processing on the
    // submission node after all your histogram outputs have been
    // merged.
    return EL::StatusCode::SUCCESS;
}

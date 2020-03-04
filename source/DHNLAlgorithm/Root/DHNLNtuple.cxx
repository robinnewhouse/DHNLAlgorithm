#include <DHNLAlgorithm/DHNLNtuple.h>
#include <DHNLAlgorithm/DHNLFunctions.h>
#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include "AthContainers/ConstDataVector.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include <xAODAnaHelpers/HelperFunctions.h>
#include <xAODTruth/TruthVertex.h>
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

#include "TFile.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;


// this is needed to distribute the algorithm to the workers
ClassImp(DHNLNtuple)

DHNLNtuple::DHNLNtuple() :
        m_cutflowHist(0),
        m_cutflowHistW(0),
        m_treeStream("tree") {
// Here you put any code for the base initialization of variables,
// e.g. initialize all pointers to 0.  Note that you should only put
// the most basic initialization here, since this method will be
// called on both the submission and the worker node.  Most of your
// initialization code will go into histInitialize() and
// initialize().

    ANA_MSG_INFO("DHNLNtuple() : Calling constructor");

    m_inJetContainerName = "";
    m_jetInputAlgo = "";
    m_allJetContainerName = "";
    m_allJetInputAlgo = "";
    m_inMETContainerName = "";
    m_inMETTrkContainerName = "";
    m_inMuContainerName = "";
    m_inElContainerName = "";
    m_inTruthParticleContainerName = "";
    m_msgLevel = MSG::INFO;
    m_useCutFlow = true;
    m_writeTree = true;
    m_MCPileupCheckContainer = "AntiKt4TruthJets";
    m_truthLevelOnly = false;
    m_eventDetailStr = "truth pileup";
    m_trigDetailStr = "";
    m_jetDetailStr = "kinematic clean energy truth flavorTag";
    m_jetDetailStrSyst = "kinematic clean energy";
    m_elDetailStr = "kinematic clean energy truth flavorTag";
    m_muDetailStr = "kinematic clean energy truth flavorTag";
    m_metDetailStr = "metClus sigClus";
    m_metTrkDetailStr = "metTrk sigTrk";
    m_trackDetailStr = "";
    m_secondaryVertexDetailStr = "";
    m_truthVertexDetailStr = "";
    m_truthParticleDetailString = "";
    m_AugumentationVersionString = "";
    m_suppressTrackFilter = true;

}

EL::StatusCode DHNLNtuple::initialize() {
    // Here you do everything that you need to do after the first input
    // file has been connected and before the first event is processed,
    // e.g. create additional histograms based on which variables are
    // available in the input files.  You can also create all of your
    // histograms and trees in here, but be aware that this method
    // doesn't get called if no events are processed.  So any objects
    // you create here won't be available in the output if you have no
    // input events.

    ANA_MSG_DEBUG("initialize");

    m_event = wk()->xaodEvent();
    m_store = wk()->xaodStore();
    m_eventCounter = -1;


    const xAOD::EventInfo *eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));

    if (this->configure() == EL::StatusCode::FAILURE) {
        ANA_MSG_ERROR("initialize() : Failed to properly configure. Exiting.");
        return EL::StatusCode::FAILURE;
    }

    // m_truthLevelOnly is set in config so need to do this after configure is called
    if (m_truthLevelOnly) m_isMC = true;
    else m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
    ANA_MSG_INFO("initialize() : isMC: " << m_isMC);

    if (m_useCutFlow) {

        TFile *file = wk()->getOutputFile("cutflow");
        m_cutflowHist = (TH1D *) file->Get("cutflow");
        m_cutflowHistW = (TH1D *) file->Get("cutflow_weighted");

        m_cutflowFirst = m_cutflowHist->GetXaxis()->FindBin("TriggerEfficiency");
        m_cutflowHistW->GetXaxis()->FindBin("TriggerEfficiency");

        if (m_useMCPileupCheck && m_isMC) {
            m_cutflowHist->GetXaxis()->FindBin("mcCleaning");
            m_cutflowHistW->GetXaxis()->FindBin("mcCleaning");
        }

        m_cutflowHist->GetXaxis()->FindBin("y*");
        m_cutflowHistW->GetXaxis()->FindBin("y*");
    }

    ANA_MSG_INFO("initialize() : Successfully initialized! \n");
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLNtuple::configure() {
    ANA_MSG_INFO("configure() : Configuring DHNLNtuple Interface.");

    if (m_MCPileupCheckContainer == "None") {
        m_useMCPileupCheck = false;
    }

    m_comEnergy = "13TeV";

    ANA_MSG_INFO("configure() : DHNLNtuple Interface succesfully configured! \n");

//    May want to run these checks on vertices and leptons // Robin

    if (m_inJetContainerName.empty()) {
        ANA_MSG_ERROR("configure() : InputContainer string is empty!");
        return EL::StatusCode::FAILURE;
    }

    if (!m_truthLevelOnly && m_allJetContainerName.empty()) {
        ANA_MSG_ERROR("configure() : AllJetInputContainer string is empty!");
        return EL::StatusCode::FAILURE;
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLNtuple::setupJob(EL::Job &job) {
    // Here you put code that sets up the job on the submission object
    // so that it is ready to work with your algorithm, e.g. you can
    // request the D3PDReader service or add output files.  Any code you
    // put here could instead also go into the submission script.  The
    // sole advantage of putting it here is that it gets automatically
    // activated/deactivated when you add/remove the algorithm from your
    // job, which may or may not be of value to you.
    job.useXAOD();
    xAOD::Init("DHNLNtuple").ignore(); // call before opening first file

    EL::OutputStream outForTree(m_treeStream);
    job.outputAdd(outForTree);
    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLNtuple::execute() {
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.
    ANA_MSG_DEBUG("execute(): Applying selection");
    ++m_eventCounter;

    ///////////////////////////// Retrieve Containers /////////////////////////////////////////


//    //Set this first, as m_mcEventWeight is needed by passCut()
//    if (m_isMC)
//        m_mcEventWeight = eventInfo->mcEventWeight();
//    else
//        m_mcEventWeight = 1;
//
//    if (doCutflow)
//        DHNLFunctions::passCut(m_cutflowHist, m_cutflowHistW, m_iCutflow, m_mcEventWeight); //TriggerEfficiency


    std::string systName; // This is a placeholder to be modified when we start doing the analysis with systematics
    if (m_myTrees.find(systName) == m_myTrees.end()) { AddTree(systName); } // Get tree or make a new one

    ANA_MSG_DEBUG("execute() : Get Containers");

    const xAOD::EventInfo *eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));
    if (eventInfo) { m_myTrees[systName]->FillEvent(eventInfo, m_event); }
    if (eventInfo) m_myTrees[systName]->FillTrigger(eventInfo);

    const xAOD::VertexContainer *vertices = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));
    // Should fill primary vertices here. Is this taken care of by truth vertices? // Robin

    const xAOD::TrackParticleContainer *tracks = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(tracks, "InDetTrackParticles", m_event, m_store));
    if (tracks) { m_myTrees[systName]->FillTracks(tracks); }

    const xAOD::TruthParticleContainer *TruthParts = nullptr;
    if (m_isMC) {ANA_CHECK (HelperFunctions::retrieve(TruthParts, m_inTruthParticleContainerName, m_event, m_store)); }
    if (TruthParts) { m_myTrees[systName]->FillTruth(TruthParts, "xAH_truth"); }

    const xAOD::JetContainer *truthJets = nullptr;
    if (m_useMCPileupCheck && m_isMC) {
        ANA_CHECK (HelperFunctions::retrieve(truthJets, m_MCPileupCheckContainer, m_event, m_store));
    }
    if (truthJets) { m_myTrees[systName]->FillJets(truthJets); }

    const xAOD::MissingETContainer *Met(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(Met, m_inMETContainerName, m_event, m_store));
    if (Met) { m_myTrees[systName]->FillMET(Met); }

    const xAOD::MissingETContainer *MetTrk(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(MetTrk, m_inMETTrkContainerName, m_event, m_store));
    if (MetTrk) { m_myTrees[systName]->FillMET(MetTrk); }

    const xAOD::MuonContainer *allMuons(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(allMuons, m_inMuContainerName, m_event, m_store));
    if (allMuons) m_myTrees[systName]->FillMuons(allMuons, HelperFunctions::getPrimaryVertex(vertices));

    const xAOD::ElectronContainer *allElectrons = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(allElectrons, m_inElContainerName, m_event, m_store));
    if (allElectrons) m_myTrees[systName]->FillElectrons(allElectrons, HelperFunctions::getPrimaryVertex(vertices));

    const xAOD::JetContainer *allJets = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(allJets, m_allJetContainerName, m_event, m_store));
    if (allJets) m_myTrees[systName]->FillJets(allJets, HelperFunctions::getPrimaryVertexLocation(vertices));
    if (allJets) { m_myTrees[systName]->FillJets(allJets, -1); }

    // const xAOD::JetContainer *signalJets = nullptr;
    // ANA_CHECK (HelperFunctions::retrieve(signalJets, m_inJetContainerName, m_event, m_store));

    const xAOD::TruthVertexContainer *inTruthVerts = nullptr;
    if (m_isMC) {ANA_CHECK(HelperFunctions::retrieve(inTruthVerts, m_truthVertexContainerName, m_event, m_store)); }
    if (inTruthVerts) m_myTrees[systName]->FillTruthVerts(inTruthVerts, m_truthVertexBranchName);

    if (not m_secondaryVertexContainerName.empty()) { // Useful for running framework on AODs which have no secondary vertex container
        const xAOD::VertexContainer *inSecVerts = nullptr;
        ANA_CHECK(HelperFunctions::retrieve(inSecVerts, m_secondaryVertexContainerName, m_event, m_store, msg()));
        if (inSecVerts) m_myTrees[systName]->FillSecondaryVerts(inSecVerts, m_secondaryVertexBranchName, m_suppressTrackFilter);
    }


    ANA_MSG_DEBUG("Event # " << m_eventCounter);
    m_myTrees[systName]->Fill();
    ANA_MSG_DEBUG("Tree Written");

    return EL::StatusCode::SUCCESS;
}


void DHNLNtuple::AddTree(std::string name) {

    ANA_MSG_DEBUG("AddTree");

    std::string treeName("outTree");
    // naming convention
    treeName += name; // add systematic
    auto *outTree = new TTree(treeName.c_str(), treeName.c_str());
    TFile *treeFile = wk()->getOutputFile(m_treeStream);
    outTree->SetDirectory(treeFile);

    DHNLMiniTree *miniTree = new DHNLMiniTree(m_event, outTree, treeFile, m_store); //!!j

    miniTree->AddEvent(m_eventDetailStr);
    miniTree->AddTrigger(m_trigDetailStr);
    miniTree->AddMET(m_metDetailStr);
    miniTree->AddMET(m_metTrkDetailStr, "trkMET");
    miniTree->AddTrackParts(m_trackDetailStr);
    miniTree->AddTruthParts(m_truthParticleDetailString, "xAH_truth");
    miniTree->AddJets(m_jetDetailStrSyst);
    miniTree->AddMuons(m_muDetailStr);
    miniTree->AddElectrons(m_elDetailStr);
    miniTree->AddSecondaryVerts(m_secondaryVertexDetailStr, m_secondaryVertexBranchName, m_AugumentationVersionString);
    miniTree->AddTruthVerts(m_truthVertexDetailStr, m_truthVertexBranchName);

    m_myTrees[name] = miniTree;
}

EL::StatusCode DHNLNtuple::finalize() {
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
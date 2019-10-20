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

//    m_inJetContainerName       = "";
//    m_inputAlgo                = "";
//    m_allJetContainerName      = "";
//    m_allJetInputAlgo          = "";
//    m_inMETContainerName       = "";
//    m_inMETTrkContainerName    = "";
    m_msgLevel = MSG::INFO;
//    m_useCutFlow               = true;
    m_writeTree = true;
    m_MCPileupCheckContainer = "AntiKt4TruthJets";
    m_truthLevelOnly = false;
//    m_eventDetailStr           = "truth pileup";
//    m_trigDetailStr            = "";
//    m_jetDetailStr             = "kinematic clean energy truth flavorTag";
//    m_jetDetailStrSyst         = "kinematic clean energy";
//    m_elDetailStr              = "kinematic clean energy truth flavorTag";
    m_muDetailStr = "kinematic clean energy truth flavorTag";
//    m_metDetailStr             = "metClus sigClus";
//    m_metTrkDetailStr          = "metTrk sigTrk";
//    m_trackDetailStr           = "";
//    m_secondaryVertexDetailStr = "";
//    m_truthVertexDetailStr     = "";

    ANA_MSG_INFO("DHNLNtuple() : Calling constructor");


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
    if (m_truthLevelOnly) { m_isMC = true; }
    else { m_isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION); }

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

//    if (m_inJetContainerName.empty()) {
//        ANA_MSG_ERROR("configure() : InputContainer string is empty!");
//        return EL::StatusCode::FAILURE;
//    }
//
//    if (!m_truthLevelOnly && m_allJetContainerName.empty()) {
//        ANA_MSG_ERROR("configure() : AllJetInputContainer string is empty!");
//        return EL::StatusCode::FAILURE;
//    }

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

    ANA_MSG_DEBUG("execute() : Get Containers");
    const xAOD::EventInfo *eventInfo(nullptr);
    ANA_CHECK (HelperFunctions::retrieve(eventInfo, "EventInfo", m_event, m_store));

    const xAOD::VertexContainer *vertices = nullptr;
    if (!m_truthLevelOnly) {
        ANA_CHECK (HelperFunctions::retrieve(vertices, "PrimaryVertices", m_event, m_store));
    }

    const xAOD::MuonContainer *allMuons = nullptr;
    ANA_CHECK (HelperFunctions::retrieve(allMuons, m_inMuContainerName, m_event, m_store));

    ///////////////////////////// Loop over Systematics /////////////////////////////////////////

    //Set this first, as m_mcEventWeight is needed by passCut()
    if (m_isMC) { m_mcEventWeight = eventInfo->mcEventWeight(); }
    else { m_mcEventWeight = 1; }

    // did any collection pass the cuts?
    bool pass(false);
    std::string systName = ""; // for looping later // Robin


    // Fill tree // Moved from function to inline // Robin

    // Create new tree if one does not exist already
    if (m_writeTree) {
        if (m_myTrees.find(systName) == m_myTrees.end()) { AddTree(systName); }

//        if (eventInfo) { m_myTrees[systName]->FillEvent(eventInfo, m_event); }
        if (allMuons) m_myTrees[systName]->FillMuons(allMuons, HelperFunctions::getPrimaryVertex(vertices));

        m_myTrees[systName]->Fill();

        ANA_MSG_DEBUG("Tree Written");

    }

    return EL::StatusCode::SUCCESS;

}

void DHNLNtuple::AddTree(std::string name) {

    ANA_MSG_DEBUG("AddTree");

    std::string treeName("outTree");
    // naming convention
    treeName += name; // add systematic
    auto *outTree = new TTree(treeName.c_str(), treeName.c_str());
    if (!outTree) {
        ANA_MSG_ERROR("AddTree() : Failed to get output tree!");
        // FIXME!! kill here
    }
    TFile *treeFile = wk()->getOutputFile(m_treeStream);
    outTree->SetDirectory(treeFile);

    DHNLMiniTree *miniTree = new DHNLMiniTree(m_event, outTree, treeFile, m_store); //!!j
    // only limited information available in truth xAODs
    if (m_truthLevelOnly) {
        miniTree->AddEvent("truth");
        miniTree->AddJets("kinematic");
    } else { // reconstructed xAOD
//        miniTree->AddEvent(m_eventDetailStr);
//        miniTree->AddTrigger(m_trigDetailStr);
//        miniTree->AddMET(m_metDetailStr);
//        miniTree->AddMET("trkMET", m_metTrkDetailStr);
//        miniTree->AddTrackParts("track", m_trackDetailStr);
        if (!name.empty()) { // save limited information for systematic variations
//            miniTree->AddJets(m_jetDetailStrSyst);
        } else {
//            miniTree->AddJets(m_jetDetailStr);
            miniTree->AddMuons(m_muDetailStr);
//            miniTree->AddElectrons(m_elDetailStr);
//            if (!m_secondaryVertexDetailStr.empty())
//                miniTree->AddSecondaryVerts(m_secondaryVertexDetailStr, m_secondaryVertexBranchName);
//            if (!m_truthVertexDetailStr.empty())
//                miniTree->AddTruthVerts(m_truthVertexDetailStr, m_truthVertexBranchName);
        }
    }
    m_myTrees[name] = miniTree;
    // see Worker.cxx line 134: the following function call takes ownership of the tree
    // from the treeFile; no output is written. so don't do that!
    // wk()->addOutput( outTree );

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
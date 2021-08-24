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
    m_trackParticleContainerName = "";
    m_inMuContainerName = "";
    m_inElContainerName = "";
    m_inTruthParticleContainerName = "";
    m_msgLevel = MSG::INFO;
    m_useCutFlow = true;
    m_writeTree = true;
    m_MCPileupCheckContainer = "None";
    m_truthLevelOnly = false;
    m_eventDetailStr = "truth pileup";
    m_trigDetailStr = "";
    m_jetDetailStr = "";
    m_jetDetailStrSyst = "";
    m_elDetailStr = "kinematic clean energy truth flavorTag";
    m_muDetailStr = "kinematic clean energy truth flavorTag";
    m_metDetailStr = "";
    m_metTrkDetailStr = "";
    m_trackDetailStr = "";
    m_vertexDetailStr = "";
    m_secondaryVertexDetailStr = "";
    m_truthVertexContainerName = "";
    m_truthVertexDetailStr = "";
    m_truthParticleDetailString = "";
    m_AltAugmentationVersionString = "";
    m_suppressTrackFilter = true;
    m_vertexContainerName = "";
    m_eventInfoContainerName = "";

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


      // Parse altVSIstr list, split by comma, and put into a vector for later use
      // Make sure it's not empty!
      //
      if ( m_secondaryVertexContainerNameList.empty() ) {
          ANA_MSG_ERROR("Empty secondaryVertexContainerName List!!!");
      }
       if ( m_secondaryVertexBranchNameList.empty() ) {
          ANA_MSG_ERROR("Empty secondaryVertexBranchName List!!!");
      }
       /*if ( m_AugmentationVersionStringList.empty()) {
          ANA_MSG_ERROR("Empty AugmentationVersionString List!!!");
      }*/
      std::string secondaryVertexContainerName_token;
      std::string secondaryVertexBranchName_token;
      std::string AugmentationVersionString_token;

      std::istringstream sv(m_secondaryVertexContainerNameList);
      std::istringstream sb(m_secondaryVertexBranchNameList);
      std::istringstream augstr(m_AugmentationVersionStringList);

      while ( std::getline(sv, secondaryVertexContainerName_token, ',') ) {
        m_secondaryVertexContainerNameKeys.push_back(secondaryVertexContainerName_token);
      }

      while ( std::getline(sb, secondaryVertexBranchName_token, ',') ) {
		m_secondaryVertexBranchNameKeys.push_back(secondaryVertexBranchName_token);
      }

      if (m_AugmentationVersionStringList.empty())
		  m_AugmentationVersionStringKeys.push_back("");
	  while ( std::getline(augstr, AugmentationVersionString_token, ',') ) {
		m_AugmentationVersionStringKeys.push_back(AugmentationVersionString_token);
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
    if (not job.outputHas(m_treeStream))
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

    // Prepare systematic trees
    std::vector<std::string> allSystematicTrees = {};
    vector<string> systAlgs;

    string m_inputAlgosTmp = string(m_inputAlgos);
    string delimiter = ",";
    size_t pos;
    while ((pos = m_inputAlgosTmp.find(delimiter)) != string::npos) {
        systAlgs.push_back(m_inputAlgosTmp.substr(0, pos));
        m_inputAlgosTmp.erase(0, pos + delimiter.length());
    }

//    boost::split(systAlgs, m_inputAlgos, boost::is_any_of(","));


//    m_store->print();

    for (auto alg : systAlgs) {
        ANA_MSG_DEBUG("Will add systematics from algorithm: " << alg);
        std::vector<std::string> *systNames = nullptr;
        if (m_store->contains<std::vector<std::string> >(alg)) {
            if (!m_store->retrieve(systNames, alg).isSuccess()) {
                ANA_MSG_INFO("Cannot find vector from " << alg);
                return StatusCode::FAILURE;
            }
            // Add every systematic for this object type to allSystematicTrees
            for (auto sys : *systNames) {
                if (sys.empty()) // only add nominal tree once
                    if (std::find(allSystematicTrees.begin(), allSystematicTrees.end(), "") != allSystematicTrees.end())
                        continue;
                allSystematicTrees.push_back(sys);
            }
        }

    }
    for (auto sys : allSystematicTrees)
        ANA_MSG_DEBUG("Systematic added: " << sys);


    if (m_inputAlgos.empty()) {
        fillTree(); // Fill nominal tree (no systematics)
    } else {
        for (auto systName : allSystematicTrees) {
            ANA_MSG_DEBUG("execute() : Systematic Loop " << systName);
            fillTree(systName);
        }
    }

    return EL::StatusCode::SUCCESS;
}

EL::StatusCode DHNLNtuple::fillTree(std::string systName) {

    // Get the existing ttree associated with this systematic tree. If it can't be found, create a new one.
    if (m_myTrees.find(systName) == m_myTrees.end()) {
        AddTree(systName);
    }

    ANA_MSG_DEBUG("fillTree() : Get Containers");

    ///______________________________________________________
    /// Event    
    const xAOD::EventInfo *eventInfo = nullptr;
    if (not m_eventInfoContainerName.empty())
        ANA_CHECK (HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store));
    if (eventInfo) {
        m_myTrees[systName]->FillEvent(eventInfo, m_event);
        m_myTrees[systName]->FillTrigger(eventInfo);
    }

    ///______________________________________________________
    /// Primary vertices
    const xAOD::VertexContainer *vertices = nullptr;
    if (not m_vertexContainerName.empty())
        ANA_CHECK (HelperFunctions::retrieve(vertices, m_vertexContainerName, m_event, m_store));
    if (vertices) { m_myTrees[systName]->FillVertices(vertices); }

    ///______________________________________________________
    /// Truth particles
    const xAOD::TruthParticleContainer *TruthParts = nullptr;
    if (m_isMC && not m_inTruthParticleContainerName.empty())
        ANA_CHECK (HelperFunctions::retrieve(TruthParts, m_inTruthParticleContainerName, m_event, m_store));
    if (TruthParts) { m_myTrees[systName]->FillTruth(TruthParts, "xAH_truth"); }

    ///______________________________________________________
    /// Muons
    if (!m_inMuContainerName.empty()) {
        // get container + fill branches if available
        const xAOD::MuonContainer *inMuons = nullptr;
        if (HelperFunctions::isAvailable<xAOD::MuonContainer>(m_inMuContainerName + systName, m_event, m_store, msg())) {
            ANA_MSG_DEBUG("Retrieving '" << m_inMuContainerName + systName << "' Muon container");
            ANA_CHECK(HelperFunctions::retrieve(inMuons, m_inMuContainerName + systName, m_event, m_store, msg()));
            ANA_MSG_DEBUG("Filling Muon branches");
            m_myTrees[systName]->FillMuons(inMuons, HelperFunctions::getPrimaryVertex(vertices));
        } else if (HelperFunctions::isAvailable<xAOD::MuonContainer>(m_inMuContainerName, m_event, m_store, msg())) {
            ANA_MSG_DEBUG("Retrieving '" << m_inMuContainerName << "' Muon container");
            ANA_CHECK(HelperFunctions::retrieve(inMuons, m_inMuContainerName, m_event, m_store, msg()));
            ANA_MSG_DEBUG("Filling Muon branches");
            m_myTrees[systName]->FillMuons(inMuons, HelperFunctions::getPrimaryVertex(vertices));
        } else
            ANA_MSG_DEBUG("Muon container, '" << m_inMuContainerName << ", is not available. Skipping...");
    }

    ///______________________________________________________
    /// Electrons
    if (!m_inElContainerName.empty()) {
        // get container + fill branches if available
        const xAOD::ElectronContainer *inElectrons = nullptr;
        if (HelperFunctions::isAvailable<xAOD::ElectronContainer>(m_inElContainerName + systName, m_event, m_store, msg())) {
            ANA_MSG_DEBUG("Retrieving '" << m_inElContainerName + systName << "' Electron container");
            ANA_CHECK(HelperFunctions::retrieve(inElectrons, m_inElContainerName + systName, m_event, m_store, msg()));
            ANA_MSG_DEBUG("Filling Electron branches");
            m_myTrees[systName]->FillElectrons(inElectrons, HelperFunctions::getPrimaryVertex(vertices));
        } else if (HelperFunctions::isAvailable<xAOD::ElectronContainer>(m_inElContainerName, m_event, m_store, msg())) {
            ANA_MSG_DEBUG("Retrieving '" << m_inElContainerName << "' Electron container");
            ANA_CHECK(HelperFunctions::retrieve(inElectrons, m_inElContainerName, m_event, m_store, msg()));
            ANA_MSG_DEBUG("Filling Electron branches");
            m_myTrees[systName]->FillElectrons(inElectrons, HelperFunctions::getPrimaryVertex(vertices));
        } else
            ANA_MSG_DEBUG("Electron container, '" << m_inElContainerName << ", is not available. Skipping...");
    }

    
    ///______________________________________________________
    /// Tracks
    const xAOD::TrackParticleContainer *tracks = nullptr;
    if (not m_trackParticleContainerName.empty())
        ANA_CHECK (HelperFunctions::retrieve(tracks, m_trackParticleContainerName, m_event, m_store));
    if (tracks) { m_myTrees[systName]->FillTracks(tracks, "tracks"); }

    ///______________________________________________________
    /// Truth vertices
    const xAOD::TruthVertexContainer *inTruthVerts = nullptr;
    if (m_isMC && not m_truthVertexContainerName.empty())
        ANA_CHECK(HelperFunctions::retrieve(inTruthVerts, m_truthVertexContainerName, m_event, m_store));
    if (inTruthVerts) m_myTrees[systName]->FillTruthVerts(inTruthVerts, m_truthVertexBranchName);

    ///______________________________________________________
    /// Secondary vertices
    // Fill the secondary vertices from the list
    if (!m_secondaryVertexContainerNameKeys.empty() and not m_AugmentationVersionStringKeys.empty()) {
        for (size_t i = 0; i < m_secondaryVertexContainerNameKeys.size(); i++) {
            if (m_AugmentationVersionStringKeys[i] == m_AltAugmentationVersionString and not m_AltAugmentationVersionString.empty()) continue; // check you do not fill same as alt VSI twice
            const xAOD::VertexContainer *inSecVerts = nullptr;
            ANA_CHECK(HelperFunctions::retrieve(inSecVerts, m_secondaryVertexContainerNameKeys[i], m_event, m_store, msg()));
            if (inSecVerts) m_myTrees[systName]->FillSecondaryVerts(inSecVerts, m_secondaryVertexBranchNameKeys[i], m_suppressTrackFilter);
        }
    }

    // Fill the alternative secondary vertex container from command line if required 
    if (not m_secondaryVertexContainerNameAlt.empty() and m_AltAugmentationVersionString != "None") {
        const xAOD::VertexContainer *inSecVertsAlt = nullptr;
        ANA_CHECK(HelperFunctions::retrieve(inSecVertsAlt, m_secondaryVertexContainerNameAlt, m_event, m_store, msg()));
        if (inSecVertsAlt) m_myTrees[systName]->FillSecondaryVerts(inSecVertsAlt, m_secondaryVertexBranchNameAlt, m_suppressTrackFilter);
    }

    ANA_MSG_DEBUG("Event # " << m_eventCounter);
    m_myTrees[systName]->Fill();
    ANA_MSG_DEBUG("Tree Written");

    return EL::StatusCode::SUCCESS;
}


void DHNLNtuple::AddTree(std::string name) {

    ANA_MSG_DEBUG("AddTree");

    std::string treeName("nominal");
    // naming convention
    treeName += name; // add systematic
    auto *outTree = new TTree(treeName.c_str(), treeName.c_str());
    TFile *treeFile = wk()->getOutputFile(m_treeStream);
    outTree->SetDirectory(treeFile);
    
    DHNLMiniTree *miniTree = new DHNLMiniTree(m_event, outTree, treeFile, m_store); //!!j

    // Event
    if (not m_eventDetailStr.empty()) miniTree->AddEvent(m_eventDetailStr);
    if (not m_trigDetailStr.empty()) miniTree->AddTrigger(m_trigDetailStr);
    
    // Leptons
    if (not m_muDetailStr.empty()) miniTree->AddMuons(m_muDetailStr);
    if (not m_elDetailStr.empty()) miniTree->AddElectrons(m_elDetailStr);

    // Tracks
    if (not m_trackDetailStr.empty()) miniTree->AddTrackParts(m_trackDetailStr, "tracks");

    // MET
    if (not m_metDetailStr.empty()) miniTree->AddMET(m_metDetailStr);
    if (not m_metTrkDetailStr.empty()) miniTree->AddMET(m_metTrkDetailStr, "trkMET");
    
    // Jets
    if (not m_jetDetailStrSyst.empty()) miniTree->AddJets(m_jetDetailStrSyst);

    // Vertices
    if (not m_vertexDetailStr.empty()) miniTree->AddVertices(m_vertexDetailStr);
    if (m_secondaryVertexBranchNameKeys.size()>0 and not m_secondaryVertexDetailStr.empty()) {
		
        for(size_t i=0; i < m_secondaryVertexContainerNameKeys.size(); i++){
			 miniTree->AddSecondaryVerts(m_secondaryVertexDetailStr, m_secondaryVertexBranchNameKeys[i], m_AugmentationVersionStringKeys[i]); } }
    if (m_AltAugmentationVersionString != "None" ) { 
        miniTree->AddSecondaryVerts(m_secondaryVertexDetailStr, m_secondaryVertexBranchNameAlt, m_AltAugmentationVersionString); }
    
    // Truth
    if (m_isMC){ 
        miniTree->AddTruthParts(m_truthParticleDetailString, "xAH_truth");
        miniTree->AddTruthVerts(m_truthVertexDetailStr, m_truthVertexBranchName); 
    }

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
    for (auto tree: m_myTrees)
        delete tree.second;

    return EL::StatusCode::SUCCESS;
}



EL::StatusCode DHNLNtuple :: histFinalize ()
{
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
  // if( m_writeTree ) {
    std::string thisName;
    //m_ss.str( std::string() );
    //m_ss << m_mcChannelNumber;
    TFile * treeFile = wk()->getOutputFile( m_treeStream );
    if(m_useCutFlow) {
      TH1D* thisCutflowHist = (TH1D*) m_cutflowHist->Clone();
      thisName = thisCutflowHist->GetName();
      thisCutflowHist->SetName( (thisName).c_str() );
      //thisCutflowHist->SetName( (thisName+"_"+m_ss.str()).c_str() );
      thisCutflowHist->SetDirectory( treeFile );

      // TH1F* thisCutflowHistW = (TH1F*) m_cutflowHistW->Clone();
      // thisName = thisCutflowHistW->GetName();
      // thisCutflowHistW->SetName( (thisName).c_str() );
      // //thisCutflowHistW->SetName( (thisName+"_"+m_ss.str()).c_str() );
      // thisCutflowHistW->SetDirectory( treeFile );
    
    //Get MetaData_EventCount histogram
    TFile* metaDataFile = wk()->getOutputFile( "metadata" );
    TH1D* metaDataHist = (TH1D*) metaDataFile->Get("MetaData_EventCount");
    TH1D* thisMetaDataHist = (TH1D*) metaDataHist->Clone();
    thisMetaDataHist->SetDirectory( treeFile );
  }

  return EL::StatusCode::SUCCESS;
}


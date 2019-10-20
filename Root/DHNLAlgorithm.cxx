#include <DHNLAlgorithm/DHNLAlgorithm.h>
#include <EventLoop/Job.h>
#include <EventLoop/Worker.h>
#include "EventLoop/OutputStream.h"
#include "AthContainers/ConstDataVector.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
//#include <DHNLAlgorithm/DHNLFunctions.h>
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
ClassImp(DHNLAlgorithm)

DHNLAlgorithm::DHNLAlgorithm() :
        m_cutflowHist(nullptr),
        m_cutflowHistW(nullptr) {
// Here you put any code for the base initialization of variables,
// e.g. initialize all pointers to 0.  This is also where you
// declare all properties for your algorithm.  Note that things like
// resetting statistics variables or booking histograms should
// rather go into the initialize() function.
}


EL::StatusCode DHNLAlgorithm::initialize() {
    // Here you do everything that needs to be done at the very
    // beginning on each worker node, e.g. create histograms and output
    // trees.  This method gets called before any input files are
    // connected.
    return EL::StatusCode::SUCCESS;
}


EL::StatusCode DHNLAlgorithm::execute() {
    // Here you do everything that needs to be done on every single
    // events, e.g. read input variables, apply cuts, and fill
    // histograms and trees.  This is where most of your actual analysis
    // code will go.

    ANA_MSG_INFO ("in execute");

    // retrieve the eventInfo object from the event store
    const xAOD::EventInfo *eventInfo = nullptr;
    ANA_CHECK (evtStore()->retrieve(eventInfo, "EventInfo"));

    // print out run and event number from retrieved object
    ANA_MSG_INFO (
            "in execute, runNumber = " << eventInfo->runNumber() << ", eventNumber = " << eventInfo->eventNumber());

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

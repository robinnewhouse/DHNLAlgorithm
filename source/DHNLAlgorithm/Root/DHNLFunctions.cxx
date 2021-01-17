#include "DHNLAlgorithm/DHNLFunctions.h"

/**
 * tracks are from the same event
 * @param vertex
 * @return true if tracks are from the same event
 */
bool sameEventVertex(const xAOD::Vertex *vertex) {
    runNr_0 = vertex->trackParticle(0)->auxdataConst<int>("trackOriginalRun");
    evtNr_0 = vertex->trackParticle(0)->auxdataConst<int>("trackOriginalEvent");

    runNr_1 = vertex->trackParticle(1)->auxdataConst<int>("trackOriginalRun");
    evtNr_1 = vertex->trackParticle(1)->auxdataConst<int>("trackOriginalEvent");
    return runNr_0 == runNr_1 && evtNr_0 == evtNr_1;
}


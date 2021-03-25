#ifndef DHNL_DHNLFUNCTIONS_H
#define DHNL_DHNLFUNCTIONS_H
// Framework include(s)
#include <TH2.h>
#include <iostream>
#include <fstream>
#include <string>


namespace DHNLFunctions {
    TH2 *getCalibHistogram(const std::string filename, const std::string histname);

}


enum class TrackType {
    MUON = 0,
    ELECTRON = 1,
    NON_LEPTON = 2
};

#endif //DHNL_DHNLFUNCTIONS_H

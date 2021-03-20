#include <TH2.h>
#include <TFile.h>
#include "DHNLAlgorithm/DHNLFunctions.h"
#include "PathResolver/PathResolver.h"

namespace DHNLFunctions {

    TH2 *getCalibHistogram(const std::string filename, const std::string histname) {
        // now the files are stored in the calibration area
        // filename is configurable in each tool and should include the path starting from the package name
        // but defaults to recommendation for current release
        std::string filenameWithFullPath = PathResolverFindCalibFile(filename);
        TFile *calibFile = TFile::Open(filenameWithFullPath.data(), "READ");

        if (!calibFile || calibFile->IsZombie()) {
            std::cout << "Could not open file";
            return nullptr;
        }
        TH2 *hist = nullptr;
        calibFile->GetObject(histname.data(), hist);
        if (hist == nullptr) {
            std::cout << "Could not retrieve " << histname << " from file " << filename;
            return nullptr;
        }

        calibFile->Clear();
        calibFile->Close();

        return hist;
    }

}

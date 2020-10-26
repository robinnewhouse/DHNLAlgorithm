# Make directories
mkdir run
mkdir build

# Start the environment
cd build
setupATLAS
asetup AnalysisBase,21.2.121,here
lsetup git

# To fix issues relating to the fact that `FactoryTools` was checked out with it's own copy of `xAODAnaHelpers` as a submodule within `DVAnalysisBase`
cd $TestArea/../source/DHNLAlgorithm/deps/DVAnalysisBase
source util/dependencyHack.sh
cd $TestArea/../

### Future Sessions w/ Same Install
# cd DHNLAnalysis/build
# setupATLAS
# asetup
# source */setup.sh # (wildcard since os and gcc versions may differ)

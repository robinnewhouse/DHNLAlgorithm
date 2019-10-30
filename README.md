# DHNLAlgorithm

`DHNLAlgorithm` is a framework based on `DVAnalysisBase`, 
developed for the displaced vertex heavy neutral lepton analysis. 
For installation instructions, see the "Getting Started" section below.

## Getting Started

### First-time Setup and Installation

Make sure you have a directory structure that works for a modern CMake setup.

```
mkdir DHNL; cd DHNL; 
mkdir build; 
mkdir source; 
mkdir run; 
cd source;
setupATLAS;
asetup AnalysisBase,21.2.90,here;
# you can use the following for slc6:
# asetup AnalysisBase,21.2.75,here;
```

Get `DHNLAlgorithm` with:

```
lsetup git; 
git clone --recursive ssh://git@gitlab.cern.ch:7999/atlas-phys/exot/ueh/EXOT-2017-19/DHNLAlgorithm.git;
```

To fix issues relating to the fact that `FactoryTools` was checked out with it's own copy of `xAODAnaHelpers` as a submodule within `DVAnalysisBase`, run
```
source DHNLAlgorithm/deps/DVAnalysisBase/util/dependencyHack.sh;
```

And compile

```
cd ../build/; 
cmake ../source/;
make; 
```

Then make sure you setup the new environment:

```
source x*-*gcc*-opt/setup.sh  # (wildcards since os and gcc versions may differ)
```

### Future Sessions w/ Same Install

```
asetup;
cd build/; 
source x*-*gcc*-opt/setup.sh 
```
### Running

Go to your run directory

```
cd ../run/
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files /path/to/my/DAOD_RPVLL/file --submitDir testRun direct
```


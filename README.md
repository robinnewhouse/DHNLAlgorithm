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
```

Get `DHNLAlgorithm` with:

```
lsetup git; 
git clone --recursive ssh://git@gitlab.cern.ch:7999/atlas-phys/exot/ueh/EXOT-2017-19/DHNLAlgorithm.git;
```
Please contact me if you have repository access issues at this step.


To fix issues relating to the fact that `FactoryTools` was checked out with it's own copy of `xAODAnaHelpers` as a submodule within `DVAnalysisBase`, run
```
cd DHNLAlgorithm/deps/DVAnalysisBase/
source util/dependencyHack.sh
cd ../../../
```

And compile

```
cd ../build/
cmake ../source/
make -j
```

Then make sure you setup the new environment:

```
source */setup.sh  # (wildcards since os and gcc versions may differ)
```

### Future Sessions w/ Same Install

```
cd [Analysis Directory]/DHNLAlgorithm/source/
asetup
cd ../build/
source */setup.sh 
```
### Running

Go to your run directory

```
cd ../run/
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files /path/to/my/DAOD_RPVLL/file --isMC --submitDir testRun --force direct
# the --force option will overwrite your output directory
```

The output ntuple will be stored in the  directory `testRun/data-tree/`.

To run on data, simply remove the --isMC flag

```
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files /path/to/my/DAOD_RPVLL/file --submitDir testRun --force direct
```

To run a job on the grid, use a command like the one below. Note: work on successfully submitting grid jobs is still ongoing.

```
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files data16_13TeV.00304178.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054 --inputRucio prun --optGridMergeOutput 1 --optGridOutputSampleName user.dtrischu.data16_13TeV.00304178.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054_HNLNtuple_01 --optGridNGBPerJob 4 
```

## Additional Notes
### Migration from NTupleMaker
This code is meant to replace and extend the old [NTupleMaker](https://gitlab.cern.ch/atlas-phys/exot/ueh/EXOT-2017-19/NTupleMaker) framework while still keeping all required variables present in the output ntuple.
Inevitably the name and format of many of the outputs have been changed. 

To make migration easier, a spreadsheet specifying those changes has been compiled and can be found here: [NTupleMaker to DHNLAlgorithm migration](https://docs.google.com/spreadsheets/d/1NZWwB-mfnVOWJ3HE4mIrcJmJi1nd_CEQ9nqb26g3mtM/edit?usp=sharing) 

### Analysis

To perform event selection and analysis on these generated ntuples please see the code repository: [DHNLNtupleAnalysis](https://gitlab.cern.ch/atlas-phys/exot/ueh/EXOT-2017-19/DHNLNtupleAnalysis)

### VSI Leptons 

When making an ntuple using the VSI Leptons vertex container, make sure you update the container name as described in the "Running on different vertex containers" section. Additionally you will need to change the following flag to match electrons since a different electron collection is passed to the vertexing algorithm when using the VSI Leptons vertex configuration. 

```
#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_VSILepmatch"                    : True,

```


### Running on different vertex containers

If you are trying to use a DAOD_RPVLL that has had vertexing re-run on it then the vertex container name will have been augmented by the `AugmentingVersionString` in VSI. To get the vertices and tracks from this new container change the following in the config_DHNL_Algorithm.py:

```
#%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
"m_inContainerName"      : "VrtSecInclusive_SecondaryVertices_YourAugumentingVersionString",

#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices__YourAugumentingVersionString",


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLNtuple %%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices__YourAugumentingVersionString",
"m_AugumentationVersionString"   : "__YourAugumentingVersionString",

```
This will ensure that the vertex selection and matching is done using the new container and that the appropriate track variables are written out to the ntuple. 

### Updating repository

`DHNLAlgorithm` depends on submodules which may need to be updated for the framework to build and execute.
When updating a repository, a recursive update must be run.
Because `FactoryTools` contains a duplicate of `xAODAnaHelpers`, a fix must be run when updating modules.
Please use the following commands to properly update all modules:
```
git pull --recurse-submodules
git submodule update --recursive
cd deps/DVAnalysisBase/
source util/dependencyHack.sh
cd ../../
```


### WARNING: Pileup Reweighting

When running on a new Monte Carlo sample you may see an error message from xAODAnaHelpers that says something like



```
...
CP::TPileupReweighting... ERROR   Unrecognised channelNumber 311620 for periodNumber 284500
...
```
This is likely due to missing pileup reweighting (PRW) files.
To fix this, use rucio to find the `NTUP_PILEUP` file that matches your dataset.
Reference the file location in the `PRWList` field of your configuration file.
More information about this can be found at the [Extended Pileup Reweighting](https://twiki.cern.ch/twiki/bin/view/AtlasProtected/ExtendedPileupReweighting) TWiki page.
These PRW files may need to be generated for your dataset.

#### Gritty details
Even if you run on only a sample from a single MC campaign (e.g. mc16a) you must refer to the pileup reweighting files for ALL mc campaigns.
Currently that would mean mc16a,d,e. Alternatively you may remove the lumicalc files associated with the missing campaigns.


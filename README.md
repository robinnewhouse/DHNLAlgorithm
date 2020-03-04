# DHNLAlgorithm

`DHNLAlgorithm` is a framework based on `DVAnalysisBase`, 
developed for the displaced-vertex heavy neutral lepton analysis in ATLAS. 
For installation instructions, see the "Getting Started" section below.

## Getting Started

### First-time Setup and Installation

Get `DHNLAlgorithm` with:

```bash
setupATLAS
lsetup git
git clone --recursive ssh://git@gitlab.cern.ch:7999/atlas-phys/exot/ueh/EXOT-2017-19/DHNLAlgorithm.git
cd DHNLAlgorithm
```
(If you're having trouble with using ssh to clone, please see https://dguest.github.io/atlas-gitlab/02-credentials/index.html)

To build the directory structure and set up `AnalysisBase`, simply run

```bash
source setup.sh
```

And compile

```bash
cd $TestArea
cmake ../source/
make -j
source */setup.sh  # (wildcard since os and gcc versions may differ)
```


#### Future Sessions w/ Same Install

Simply `cd DHNLAlgorithm` and run `source setup.sh` again, or if you prefer:
```bash
cd DHNLAlgorithm/build
setupATLAS
asetup
source */setup.sh
```

### Running

Go to your run directory.

```bash
cd $TestArea/../run/
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files /path/to/my/DAOD_RPVLL/file --isMC --submitDir testRun --force direct
# the --force option will overwrite your output directory
```

The output ntuple will be stored in the  directory `testRun/data-tree/`.

N.B. The above run command is configured for VSI vertexing. For details about running using VSI Leptons vertexing see additional notes below. 

To run on data, simply remove the --isMC flag

```bash
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files /path/to/my/DAOD_RPVLL/file --submitDir testRun --force direct
```

To run a job on the grid, use a command like the one below. Note: We are still working on getting the grid settings right. We have not yet be sucessful at running a job on the grid.

```bash
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

```python
#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_VSILepmatch"                    : True,

```
To run with VSI Lepton vertexing configuration, go to your run directory:

```bash
cd ../run/
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm_VSILeptons.py --files /path/to/my/DAOD_RPVLL/file --isMC --extraOptions="--VSIstr _Leptons" --submitDir testRun --force direct
# the --force option will overwrite your output directory
```

### Running on different vertex containers

If you are trying to use a DAOD_RPVLL that has had vertexing re-run on it then the vertex container name will have been augmented by the `AugmentingVersionString` in VSI. To get the vertices and tracks from this new container add the following argument to the xAH_run.py command: 

```
--extraOptions="--VSIstr AugmentingVersionString"
```

where AugmentingVersionString is the name you chose as the AugmentingVersionString during the vertexing re-running. 

This will change the name following parameters in the DHNL algorithm config:

```python
#%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
"m_inContainerName"      : "VrtSecInclusive_SecondaryVertices" + o.VSIstr,


#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices" + o.VSIstr, 



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLNtuple %%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices" + o.VSIstr, 
"m_AugumentationVersionString"   : o.VSIstr,

```
This will ensure that the vertex selection and matching is done using the new container and that the appropriate track variables are written out to the ntuple. 

### Updating repository

`DHNLAlgorithm` depends on submodules which may need to be updated for the framework to build and execute.
When updating a repository, a recursive update must be run.
Because `FactoryTools` contains a duplicate of `xAODAnaHelpers`, a fix must be run when updating modules.
Please use the following commands to properly update all modules:
```bash
git pull --recurse-submodules
git submodule update --recursive
cd $TestArea/../DHNLAlgorithm/source/DHNLAlgorithm/deps/DVAnalysisBase
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

To turn off the pilue up reweighting add the following argument to the xAH_run.py run command: 

```
--extraOptions="--noPRW"
```

This will turn off PRW in the BasicEventSelection section of the configuration file by setting `m_doPUreweighting = False`.
But be warned, you will get errors about not having a random run number for the electron and muon calibrators.
These errors will not crash the algorithm, but may give you incorrect calibrations.
If someone would like to try to debug this in xAODAnaHelpers, I'm sure the fix would be appreciated. 

#### Gritty details
Even if you run on only a sample from a single MC campaign (e.g. mc16a) you must refer to the pileup reweighting files for ALL mc campaigns.
Currently that would mean mc16a,d,e. Alternatively you may remove the lumicalc files associated with the missing campaigns.

## Continuous intigration and Docker
A simple continuous intigration script has been set up and is run by `.gitlab-ci.yml`.
This will compile the code when any changes are made to `source/` and alert you if there are any compilation errors. 

A Docker image is also built after each successful compilation that can be downloaded and run as efectively a virtual machine containing the analysis. 
This is controlled by the `Dockerfile`.

These processes are still under development.

If you're interested, I recommend these excellent ATLAS workshops on 
[Continuous Integration](https://awesome-workshop.github.io/continuous-integration-deployment-gitlab/) and 
[Docker](https://awesome-workshop.github.io/intro-to-docker/). 

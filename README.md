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

Go to your run directory and run `xAH_run.py` like one of the below examples. 
Local runs will be stored in `testRun` or what ever other directory you name.
The interesting ntuples are stored in `testRun/data-tree/<filename>.root`.

#### MC
```bash
echo /eos/home-d/dhnl/data/EXOT29/mc16_13TeV.311633.Pythia8EvtGen_A14NNPDF23LO_WmuHNL50_10G_lt10dd.deriv.DAOD_EXOT29.e7422_e5984_a875_r11891_r11748_p4482/DAOD_EXOT29.24947839._000010.pool.root.1 > files.txt
# source build/*/setup.sh
xAH_run.py \
--config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
--inputList \
--files files.txt \
--submitDir testRun \
--force \
--isMC \
--extraOptions="--isDerivation --samplePeriod mc16e" \
--nevents=100 \
direct
```

WARNING: `--samplePeriod mc16e` is for rtag: r11891. This must be modified accordingly for mc16a (rtag: r11915) and mc16d (rtag: r11916).

#### Data
To run on data, simply remove the `--isMC` flag and change the `--samplePeriod` option to `dataXX`, where `XX` is the data period year.

```bash
echo /eos/home-d/dhnl/data/EXOT29/data18_13TeV.00358985.physics_Main.deriv.DAOD_EXOT29.r11969_r11784_p4072_p4491/DAOD_EXOT29.24937680._000183.pool.root.1 > files.txt
# source build/*/setup.sh
xAH_run.py \
--config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
--inputList \
--files files.txt \
--submitDir testRun \
--force \
--extraOptions="--isDerivation --samplePeriod data18" \
--nevents=100 \
direct
```

#### Grid
To run a job on the grid, use a command like the one below. 

```bash
xAH_run.py --config ../source/DHNLAlgorithm/data/config_DHNLAlgorithm.py --files data16_13TeV.00304178.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054 --inputRucio prun --optGridMergeOutput 1 --optGridOutputSampleName user.dtrischu.data16_13TeV.00304178.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054_HNLNtuple_01 --optGridNGBPerJob 4 
```
Note: We have been running jobs successfully on the grid, but some settings may not have yet been tested. We would appreciate any feedback you have.

#### Useful flags
There are several flags that can be used in addition to the example above which control xAODAnaHelpers behaviour. The full list can be found in the [xADOAnaHelpers Documentation](https://xaodanahelpers.readthedocs.io/en/latest/UsingUs.html#optional).

A very useful flag for running smaller tests is `--nevents <X>` which completes the event loop after running `<X>` events.


## Additional Notes
### Migration from NTupleMaker
This code is meant to replace and extend the old [NTupleMaker](https://gitlab.cern.ch/atlas-phys/exot/ueh/EXOT-2017-19/NTupleMaker) framework while still keeping all required variables present in the output ntuple.
Inevitably the name and format of many of the outputs have been changed. 

To make migration easier, a spreadsheet specifying those changes has been compiled and can be found here: [NTupleMaker to DHNLAlgorithm migration](https://docs.google.com/spreadsheets/d/1NZWwB-mfnVOWJ3HE4mIrcJmJi1nd_CEQ9nqb26g3mtM/edit?usp=sharing) 

### Analysis

To perform event selection and analysis on these generated ntuples please see the code repository: [DHNLNtupleAnalysis](https://gitlab.cern.ch/atlas-phys/exot/ueh/EXOT-2017-19/DHNLNtupleAnalysis)

### Running on different vertex containers

If you are trying to use a DAOD_RPVLL that has had vertexing re-run on it then 
the vertex container name will have been augmented by the `AugmentingVersionString` in VSI. 
To get the vertices and tracks from this new container add the following argument to the xAH_run.py command: 

```bash
--extraOptions="--altVSIstr AugmentingVersionString"
```
where AugmentingVersionString is the name you chose as the AugmentingVersionString 
during the vertexing re-running. e.g. `_Leptons` or `_2`.

This will augment the alternative VSI string in several places in the DHNL algorithm config. e.g:

```python
#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices" + o.VSIstr, 

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLNtuple %%%%%%%%%%%%%%%%%%%%%%%%%%#
"m_secondaryVertexBranchName"    : "secVtx_VSI",
"m_secondaryVertexBranchNameAlt" : "secVtx_VSI" + o.altVSIstr,
```

The vertex selection and matching is done using the new container as well 
as the standard VSI container. Vertex and associated track variables are 
written out to the ntuple as `secVtx_VSI*` and `secVtx_VSI_<altVSIstr>*`.
For example `secVtx_VSI_trk_pt` and `secVtx_VSI_Leptons_trk_pt`.

### Updating repository

`DHNLAlgorithm` depends on submodules which may need to be updated for the framework to build and execute.
When updating a repository, a recursive update must be run.
Because `FactoryTools` contains a duplicate of `xAODAnaHelpers`, a fix must be run when updating modules.
Please use the following commands to properly update all modules:
```bash
git pull --recurse-submodules
git submodule update --recursive
cd $TestArea/../source/DHNLAlgorithm/deps/DVAnalysisBase
source util/dependencyHack.sh
cd $TestArea
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

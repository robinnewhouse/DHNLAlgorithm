from xAODAnaHelpers import Config
import os
import shlex
import argparse



parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isSUSY15', dest='isSUSY15', action="store_true", default=False)
parser.add_argument('--noPRW', dest='noPRW', action="store_true", default=False)
parser.add_argument('--altVSIstr', dest='altVSIstr', type=str, default="None") # alternate vertex configuration string to store in tree along with VSI 
o = parser.parse_args(shlex.split(args.extra_options))

c = Config()


# Good Run Lists
GRLList = [
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
]


# Pileup Reweighting
# The sample you're running over must have the PRW file available.
# If you are getting errors such as "Unrecognised channelNumber 311660 for periodNumber 300000" this is the place to start.
# option 1. use local PRW files (these may be deleted in the future if the central cvmfs files work well)
# from DHNLAlgorithm.prw_files import prw_files_local as PRWList
# option 2. use centrally produced CVMFS files. This is still being tested as there are potentially issues.
# see https://indico.cern.ch/event/892901/contributions/3779966/attachments/2002909/3344068/sampleRequest.pdf
from DHNLAlgorithm.prw_files import prw_files_cvmfs as PRWList


# Lumicalc Files
# Must be careful about which lines are commented and which are active.
# Note: if you want to use all lumicalc files without deactivating any,
# then the PRW files for ALL MC CAMPAIGNS must be loaded in the PRWList.
# If this isn't done the pileup reweighting tool will crash the algorithm.
# Note 2: These files are fairly large (~20 MB) so they will not be kept
# in the git repository for now. You may need to copy them from cvmfs
# and store them in $TestArea/DHNLAlgorithm/data/GRL/ to run on grid.
lumicalcList = [
    # mc16a (r-tag r10740)
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root',
    # mc16d (r-tag r10739)
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root',
    # mc16e (r-tag r10790)
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root',
]

GRL       = ",".join(GRLList)
PRW       = ",".join(PRWList)
lumicalcs = ",".join(lumicalcList)


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%% BasicEventSelection %%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
basicEventSelectionDict = {
    "m_name"                      : "BasicEventSelect",
    "m_applyGRLCut"               : not args.is_MC,
    "m_GRLxml"                    : GRL,
    #"m_derivationName"            : "SUSY15Kernel_skim",
    "m_useMetaData"               : False,
    "m_storePassHLT"              : True,
    "m_storeTrigDecisions"        : True,
    "m_storePassL1"               : True,
    "m_storeTrigKeys"             : True,
    "m_applyTriggerCut"           : False,
    "m_doPUreweighting"           : False if o.noPRW else args.is_MC,
    "m_PRWFileNames"              : PRW,
    "m_lumiCalcFileNames"         : lumicalcs,
    "m_autoconfigPRW"             : False,
    "m_triggerSelection"          : "HLT_mu20_iloose_L1MU15 || HLT_mu24_iloose || HLT_mu24_ivarloose || HLT_mu24_imedium || HLT_mu24_ivarmedium || HLT_mu26_imedium || HLT_mu26_ivarmedium || HLT_mu40 || HLT_mu50 || HLT_mu60_0eta105_msonly || HLT_e24_lhmedium_L1EM20VH || HLT_e24_lhtight_nod0_ivarloose || HLT_e26_lhtight_nod0 || HLT_e26_lhtight_nod0_ivarloose || HLT_e60_lhmedium_nod0 || HLT_e60_lhmedium ||HLT_e60_medium || HLT_e120_lhloose || HLT_e140_lhloose_nod0 || HLT_e300_etcut",
    "m_checkDuplicatesData"       : False,
    "m_applyEventCleaningCut"     : False,
    "m_applyCoreFlagsCut"         : False,
    "m_vertexContainerName"       : "PrimaryVertices",
    "m_applyPrimaryVertexCut"     : True,
    "m_PVNTrack"                   : 2,
    "m_truthLevelOnly"            : True,
    "m_msgLevel"                  : "Info",
}

c.algorithm("BasicEventSelection", basicEventSelectionDict)

# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%% Truth Vertex Selection %%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
TruthVertexSelectorDict = {
    "m_name"                      : "TruthVtxSel",
    #----------------------- Container Flow ----------------------------#
    "m_inContainerName"           : "TruthVertices",
    "m_outContainerName"          : "SelectedTruthVertices",
    "m_createSelectedContainer"   : True,
    #---------------------- Selections ---------------------------#
    "m_pdgIdList"               : "9900012, 50, 24",
    #------------------------ Other ------------------------------#
    "m_msgLevel"             : "Info",

}

if args.is_MC:
    c.algorithm("TruthVertexSelector", TruthVertexSelectorDict )    


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLNtuple %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# All possible switches for storing information can be found in
# DHNLAlgorithm/deps/DVAnalysisBase/deps/xAODAnaHelpers/Root/HelperClasses.cxx
# DHNLAlgorithm/deps/DVAnalysisBase/Root/DVHelperClasses.cxx
DHNLNtupleDict = {
    "m_name"                         : "DHNLNtup",
    "m_eventInfoContainerName"       : "",
    "m_vertexContainerName"          : "",
    "m_inMETContainerName" : "",
    "m_trackParticleContainerName" : "",
    "m_inMETTrkContainerName" : "",
    "m_allJetContainerName" : "",
    "m_inJetContainerName" : "",
    #----------------------- Container Flow ----------------------------#
    "m_inMuContainerName"            : "",
    "m_inElContainerName"            : "",
    "m_secondaryVertexContainerNameList" : "",
    "m_secondaryVertexBranchNameList" : "",
    "m_AugmentationVersionStringList" : "",
    "m_secondaryVertexContainerNameAlt" : "" ,
    "m_secondaryVertexBranchNameAlt" : "" ,
    "m_AltAugmentationVersionString" : "", # augumentation for alternate vertex container
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering 
    "m_truthVertexContainerName"     : "SelectedTruthVertices",
    "m_truthVertexBranchName"        : "truthVtx",
    "m_inTruthParticleContainerName" : "",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth", #shapeEM
    "m_elDetailStr"                  : "",
    "m_muDetailStr"                  : "",
    "m_trigDetailStr"                : "",#basic menuKeys passTriggers",
    "m_secondaryVertexDetailStr"     : "", # "tracks" linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_vertexDetailStr"              : "",
    "m_truthVertexDetailStr"         : "isMatched", # Uses pt-matching to match reconstructed vertices.
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}
c.algorithm("DHNLNtuple", DHNLNtupleDict )

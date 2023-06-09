from xAODAnaHelpers import Config
import os
import shlex
import argparse



parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isDerivation', dest='isDerivation', action="store_true", default=False)
parser.add_argument('--noPRW', dest='noPRW', action="store_true", default=False)
parser.add_argument('--altVSIstr', dest='altVSIstr', type=str, default="None") # alternate vertex configuration string to store in tree along with VSI
o = parser.parse_args(shlex.split(args.extra_options))

c = Config()


# vertex container information (by default run VSI & VSI Leptons)
secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices","VrtSecInclusive_SecondaryVertices_Leptons"]
secondaryVertexBranchNames = ["secVtx_VSI", "secVtx_VSI_Leptons"]
AugmentationVersionStrings = ["","_Leptons"]


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
    "m_useMetaData"               : True,
    "m_storePassHLT"              : True,
    "m_storeTrigDecisions"        : True,
    "m_storePassL1"               : True,
    "m_storeTrigKeys"             : True,
    "m_applyTriggerCut"           : True,
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
    "m_PVNTrack"                    : 2,
    "m_msgLevel"                  : "Info",
}

c.algorithm("BasicEventSelection", basicEventSelectionDict)


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
##%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLFilter%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLFilterDict = {
    "m_name"                    : "DHNLFilter",
    #----------------------- Container Flow ----------------------------#

    "m_allJetContainerName"     : "AntiKt4EMTopoJets"if not o.isDerivation else "AntiKt4EMTopoJets_BTagging201810",
    "m_inMuContainerName"       : "Muons",
    "m_inElContainerName"       : "Electrons",
    "m_vertexContainerName"     : "PrimaryVertices",
    "m_applyFilterCut"          : False,
    # "m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices",

    #----------------------- Selections ----------------------------#

    # All selections are stored in default parameters in filter.
    # they can still be modified here. e.g.:
    # "m_AlphaMaxCut"             : 0.03,
    "m_electronLHWP"            : "Medium" if not o.isDerivation else "DFCommonElectronsLHMedium",

    #----------------------- Other ----------------------------#
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLFilter", DHNLFilterDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%% MuonCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
MuonCalibratorDict = {
    "m_name"                      : "MuonCalibrate",
    #----------------------- Container Flow ----------------------------#
    "m_inContainerName"           : "Muons",
    "m_outContainerName"          : "Muons_Calibrate",
    #----------------------- Systematics ----------------------------#
    "m_systName"                  : "",
    "m_systVal"                   : 0,
    #----------------------- Other ----------------------------#
    "m_forceDataCalib"            : False,
    "m_sort"                      : True,
    "m_msgLevel"                  : "Info"
}

c.algorithm("MuonCalibrator", MuonCalibratorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%% MuonSelector %%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
MuonSelectorDict = {
    "m_name"                      : "MuonSelect",
    #----------------------- Container Flow ----------------------------#
    "m_inContainerName"           : "Muons_Calibrate",
    "m_outContainerName"          : "Muons_Signal",
    "m_createSelectedContainer"   : True,
    #----------------------- Systematics ----------------------------#
    "m_systName"                  : "",        ## Data
    "m_systVal"                   : 0,
    #----------------------- configurable cuts ----------------------------#
    "m_muonQualityStr"            : "VeryLoose",
    "m_pass_max"                  : -1,
    "m_pass_min"                  : -1,
    "m_pT_max"                    : 1e8,
    "m_pT_min"                    : 1,
    "m_eta_max"                   : 1e8,
    "m_d0_max"                    : 1e8,
    "m_d0sig_max"                 : 1e8,
    "m_z0sintheta_max"            : 1e8,
    #----------------------- isolation stuff ----------------------------#
    "m_MinIsoWPCut"               : "",
    "m_IsoWPList"                 : "FCLoose,FCTight" if o.isDerivation else "FixedCutHighPtTrackOnly",
    #----------------------- trigger matching stuff ----------------------------#
    "m_singleMuTrigChains"        : "HLT_mu20_iloose_L1MU15, HLT_mu24_iloose, HLT_mu24_ivarloose, HLT_mu24_imedium, HLT_mu24_ivarmedium, HLT_mu26_imedium, HLT_mu26_ivarmedium, HLT_mu60_0eta105_msonly",
    #"m_minDeltaR"                 : 0.1,
    #----------------------- Other ----------------------------#
    "m_msgLevel"                  : "Info",
    "m_removeEventBadMuon"        : False,
}

# Annoyingly, we must run the MuonSelector algorithm in order to store quality parameters even in the input container.
c.algorithm("MuonSelector", MuonSelectorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%% ElectronCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
ElectronCalibratorDict = {
    "m_name"                      : "ElectronCalibrate",
    #----------------------- Container Flow ----------------------------#
    "m_inContainerName"           : "Electrons",
    "m_outContainerName"          : "Electrons_Calibrate",
    #----------------------- Systematics ----------------------------#
    "m_systName"                  : "Nominal",            ## For data
    "m_systVal"                   : 0,                    ## For data
    "m_esModel"                   : "es2016PRE",
    "m_decorrelationModel"        : "1NP_v1",
    #----------------------- Other ----------------------------#
    "m_sort"                      : True,
    "m_msgLevel"                  : "Info"
}

c.algorithm("ElectronCalibrator", ElectronCalibratorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%% ElectronSelector %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
ElectronSelectorDict = {
    "m_name"                      : "ElectronSelect",
    #----------------------- Container Flow ----------------------------#
    "m_inContainerName"           : "Electrons_Calibrate",
    "m_outContainerName"          : "Electrons_Signal",
    "m_createSelectedContainer"   : True,
    #----------------------- PID ------------- ----------------------------#
    "m_doLHPIDcut"                : False,
    "m_LHOperatingPoint"          : "Medium",
    #----------------------- configurable cuts ----------------------------#
    "m_pass_max"                  : -1,
    "m_pass_min"                  : -1,
    "m_pT_max"                    : 1e8,
    "m_pT_min"                    : 1,
    "m_eta_max"                   : 1e8,
    "m_d0_max"                    : 1e8,
    "m_d0sig_max"                 : 1e8,
    "m_z0sintheta_max"            : 1e8,
    #----------------------- isolation stuff ----------------------------#
    "m_MinIsoWPCut"               : "",
    "m_IsoWPList"                 : "FCLoose,FCTight" if o.isDerivation else "Gradient",
    #----------------------- trigger matching stuff ----------------------------#
    "m_singleElTrigChains"        : "HLT_e24_lhmedium_L1EM20VH, HLT_e24_lhtight_nod0_ivarloose, HLT_e26_lhtight_nod0, HLT_e26_lhtight_nod0_ivarloose, HLT_e60_lhmedium_nod0, HLT_e140_lhloose_nod0",
    #----------------------- Other ----------------------------#
    # "m_IsoWPList"                 : "Gradient",
    "m_msgLevel"                  : "Info"
}
# Annoyingly, we must run the ElectronSelector algorithm in order to store quality parameters even in the input container.
c.algorithm("ElectronSelector", ElectronSelectorDict )


# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%% VSI Track Selection (All Leptons: VSILep Mod + VSILep) %%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# VSITrackSelectionDict = {
#     "m_name"                      : "VSITrackSelection",
#     #----------------------- Container Flow ----------------------------#
#     "m_inDetTrackParticlesContainerName"           : "InDetTrackParticles",
#     "m_inElContainerName"           : "Electrons_Calibrate",
#     "m_inMuContainerName"           : "Muons_Calibrate",
#     "m_vertexContainerName"         : "PrimaryVertices",
#     "m_outContainerName"            : "InDetTrackParticles_Selected",
#     #---------------------- Selections ---------------------------#
#     "m_jp_passThroughTrackSelection": True, # dont apply any track selection
#     "m_jp_SAloneTRT": False,

#     "m_jp_do_PVvetoCut": True,
#     "m_jp_do_d0Cut": False,
#     "m_jp_do_z0Cut": False,
#     "m_jp_do_d0errCut": False,
#     "m_jp_do_z0errCut": False,
#     "m_jp_do_d0signifCut": False,
#     "m_jp_do_z0signifCut": False,

#     "m_jp_d0TrkPVDstMinCut": 2,
#     "m_jp_d0TrkPVDstMaxCut": 300,
#     "m_jp_z0TrkPVDstMinCut": 0,
#     "m_jp_z0TrkPVDstMaxCut": 1500,
#     "m_jp_d0TrkErrorCut": 200000,
#     "m_jp_z0TrkErrorCut": 200000,
    
#     "m_jp_TrkChi2Cut": 50,
#     "m_jp_TrkPtCut": 1000,

#     "m_jp_doTRTPixCut": True,
#     "m_jp_CutSctHits": 2,
#     "m_jp_CutPixelHits": 0,
#     "m_jp_CutSiHits": 0,
#     "m_jp_CutBLayHits": 0,
#     "m_jp_CutSharedHits": 0,
#     "m_jp_CutTRTHits": 0,
#     "m_jp_CutTightSCTHits": 7,
#     "m_jp_CutTightTRTHits": 20,

#     "m_jp_doSelectTracksFromMuons": True,
#     "m_jp_doRemoveCaloTaggedMuons": False, # don't remove calo-tagged muons
#     "m_jp_doSelectTracksFromElectrons": True,
#     "m_jp_doSelectTracksWithLRTCuts": True,
#     "m_jp_addInDetHadrons" : False, # do not add original hadrons

#     #------------------------ Other ------------------------------#
#     "m_msgLevel"             : "Info",
# }

# c.algorithm("VSITrackSelection", VSITrackSelectionDict )

# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# # #%%%%%%%%%%%%%%%%%%%%% VSI Track Selection ( VSILep Mod ) %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# # #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
VSITrackSelectionDict = {
    "m_name"                      : "VSITrackSelection",
    #----------------------- Container Flow ----------------------------#
    "m_inDetTrackParticlesContainerName"           : "InDetTrackParticles",
    "m_inElContainerName"           : "Electrons_Calibrate",
    "m_inMuContainerName"           : "Muons_Calibrate",
    "m_vertexContainerName"         : "PrimaryVertices",
    "m_outContainerName"            : "InDetTrackParticles_Selected",
    #---------------------- Selections ---------------------------#
    "m_jp_passThroughTrackSelection": False, # apply track selection
    "m_jp_SAloneTRT": False,

    "m_jp_do_PVvetoCut": True,
    "m_jp_do_d0Cut": False,
    "m_jp_do_z0Cut": False,
    "m_jp_do_d0errCut": False,
    "m_jp_do_z0errCut": False,
    "m_jp_do_d0signifCut": False,
    "m_jp_do_z0signifCut": False,

    "m_jp_d0TrkPVDstMinCut": 2.0,
    "m_jp_d0TrkPVDstMaxCut": 300.0,
    "m_jp_z0TrkPVDstMinCut": 0.0,
    "m_jp_z0TrkPVDstMaxCut": 1500.0,
    "m_jp_d0TrkErrorCut": 200000,
    "m_jp_z0TrkErrorCut": 200000,
    
    "m_jp_TrkChi2Cut": 50,
    "m_jp_TrkPtCut": 1000,

    "m_jp_doTRTPixCut": True,
    "m_jp_CutSctHits": 2,
    "m_jp_CutPixelHits": 0,
    "m_jp_CutSiHits": 0,
    "m_jp_CutBLayHits": 0,
    "m_jp_CutSharedHits": 0,
    "m_jp_CutTRTHits": 0,
    "m_jp_CutTightSCTHits": 7,
    "m_jp_CutTightTRTHits": 20,

    "m_jp_doSelectTracksFromMuons": True,
    "m_jp_doRemoveCaloTaggedMuons": True, 
    "m_jp_doSelectTracksFromElectrons": True,
    "m_jp_doSelectTracksWithLRTCuts": True,
    "m_jp_addInDetHadrons" : False, # do not add original hadrons

    #------------------------ Other ------------------------------#
    "m_msgLevel"             : "Info",
}

c.algorithm("VSITrackSelection", VSITrackSelectionDict )


# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%% VSI Track Selection ( VSILep ) %%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# VSITrackSelectionDict = {
#     "m_name"                      : "VSITrackSelection",
#     #----------------------- Container Flow ----------------------------#
#     "m_inDetTrackParticlesContainerName"           : "InDetTrackParticles",
#     "m_inElContainerName"           : "Electrons_Calibrate",
#     "m_inMuContainerName"           : "Muons_Calibrate",
#     "m_vertexContainerName"         : "PrimaryVertices",
#     "m_outContainerName"            : "InDetTrackParticles_Selected",
#     #---------------------- Selections ---------------------------#
#     "m_jp_passThroughTrackSelection": False, # apply track selection
#     "m_jp_SAloneTRT": False,

#     "m_jp_do_PVvetoCut": True,
#     "m_jp_do_d0Cut": True,
#     "m_jp_do_z0Cut": False,
#     "m_jp_do_d0errCut": False,
#     "m_jp_do_z0errCut": False,
#     "m_jp_do_d0signifCut": False,
#     "m_jp_do_z0signifCut": False,

#     "m_jp_d0TrkPVDstMinCut": 0.0,
#     "m_jp_d0TrkPVDstMaxCut": 300.0,
#     "m_jp_z0TrkPVDstMinCut": 0.0,
#     "m_jp_z0TrkPVDstMaxCut": 1500.0,
#     "m_jp_d0TrkErrorCut": 200000,
#     "m_jp_z0TrkErrorCut": 200000,
    
#     "m_jp_TrkChi2Cut": 50,
#     "m_jp_TrkPtCut": 1000.0,

#     "m_jp_doTRTPixCut": False,
#     "m_jp_CutSctHits": 2,
#     "m_jp_CutPixelHits": 0,
#     "m_jp_CutSiHits": 0,
#     "m_jp_CutBLayHits": 0,
#     "m_jp_CutSharedHits": 0,
#     "m_jp_CutTRTHits": 0,
#     "m_jp_CutTightSCTHits": 7,
#     "m_jp_CutTightTRTHits": 20,

#     "m_jp_doSelectTracksFromMuons": True,
#     "m_jp_doRemoveCaloTaggedMuons": False, 
#     "m_jp_doSelectTracksFromElectrons": True,
#     "m_jp_doSelectTracksWithLRTCuts": False,
#     "m_jp_addInDetHadrons" : False, # do not add original hadrons

#     #------------------------ Other ------------------------------#
#     "m_msgLevel"             : "Info",
# }

# c.algorithm("VSITrackSelection", VSITrackSelectionDict )



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
##%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLAlgo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLDict = {
    "m_name"                    : "DHNLAlgo",
    #----------------------- Container Flow ----------------------------#
    "m_inJetContainerName"      : "SignalJets",
    "m_inputAlgo"               : "SignalJets_Algo",
    "m_allJetContainerName"     : "AntiKt4EMTopoJets_Calib",
    "m_allJetInputAlgo"         : "AntiKt4EMTopoJets_Calib_Algo",
    "m_inMuContainerName"       : "Muons",
    "m_inElContainerName"       : "Electrons",
    "m_inDetTrackParticlesContainerName" : "InDetTrackParticles_Selected",
    #----------------------- Selections ----------------------------#
    "m_leadingJetPtCut"         : 20,
    "m_subleadingJetPtCut"      : 20,
    "m_jetMultiplicity"         : 2,
    "m_useMCPileupCheck"        : False,
    "m_metCut"                  : 20000,
    "m_doInverseLeptonControlRegion"   : False,
    "m_backgroundEstimationBranches"   : True,
    "m_doPromptLeptonCut"   : True, # apply prompt lepton cut!
    #----------------------- Other ----------------------------#
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLAlgorithm", DHNLDict )



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLNtuple %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# All possible switches for storing information can be found in
# DHNLAlgorithm/deps/DVAnalysisBase/deps/xAODAnaHelpers/Root/HelperClasses.cxx
# DHNLAlgorithm/deps/DVAnalysisBase/Root/DVHelperClasses.cxx
DHNLNtupleDict = {
    "m_name"                         : "DHNLNtup",
    #----------------------- Container Flow ----------------------------#
    "m_inMuContainerName"            : "",
    "m_inElContainerName"            : "Electrons_Calibrate",
    "m_trackParticleContainerName"   : "InDetTrackParticles_Selected",
    "m_secondaryVertexContainerNameList" : '',
    "m_secondaryVertexContainerNameList" : "",
    "m_secondaryVertexBranchNameList" : '',
    "m_AugmentationVersionStringList" : '',
    "m_AltAugmentationVersionString" : o.altVSIstr, # augumentation for alternate vertex container
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering
    "m_truthVertexContainerName"     : "TruthVertices",
    "m_truthVertexBranchName"        : "truthVtx",
    "m_inTruthParticleContainerName" : "MuonTruthParticles",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth pileup", #shapeEM
    "m_elDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation trackparams PID PID_Loose PID_Medium PID_Tight PID_LHLoose PID_LHMedium PID_LHTight PID_MultiLepton",
    "m_muDetailStr"                  : "",
    "m_trigDetailStr"                : "basic passTriggers",#basic menuKeys passTriggers",
    "m_secondaryVertexDetailStr"     : "", # "tracks" linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_vertexDetailStr"              : "primary",
    "m_truthVertexDetailStr"         : "isMatched", # Uses pt-matching to match reconstructed vertices.
    "m_trackDetailStr"               : "numbers fitpars vertex",
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

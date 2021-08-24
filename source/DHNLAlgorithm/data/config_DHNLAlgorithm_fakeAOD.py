from xAODAnaHelpers import Config
import os
import shlex
import argparse



parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isDerivation', dest='isDerivation', action="store_true", default=False)
parser.add_argument('--noPRW', dest='noPRW', action="store_true", default=False)
parser.add_argument('--samplePeriod', dest='samplePeriod', default='',)
o = parser.parse_args(shlex.split(args.extra_options))

sample_periods = o.samplePeriod.split(',')
if sample_periods == []:
    print("You didn't specify a data or MC period. Setting to 'all'. This will give incorrect results.")
    sample_periods = ['mc16a', 'mc16d', 'mc16e', 'data15', 'data16', 'data17', 'data18',]

c = Config()

# vertex container information (by default run VSI Leptons [VSI LeptonsMod for derivations])
if o.isDerivation:
    secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices_LeptonsMod_LRTR3_1p0_fake"]
    secondaryVertexBranchNames = ["secVtx_VSI_LeptonsMod"]
    AugmentationVersionStrings = ["_LeptonsMod_LRTR3_1p0_fake"]
    # VrtSecInclusive_SecondaryVertices_FixedExtroplator in SUSY15
else:
    secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices_Leptons"]
    secondaryVertexBranchNames = ["secVtx_VSI_Leptons"]
    AugmentationVersionStrings = ["_Leptons"]

# Good Run Lists
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/GoodRunListsForAnalysisRun2#Naming_scheme_and_documentation
GRLList = []
if 'data15' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20190708/data15_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns.xml')
if 'data16' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20190708/data16_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_WITH_IGNORES.xml')
if 'data17' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/data17_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')
if 'data18' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190708/data18_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')

# Pileup Reweighting
# The sample you're running over must have the PRW file available.
# If you are getting errors such as "Unrecognised channelNumber 311660 for periodNumber 300000" this is the place to start.
# option 1. use local PRW files (these may be deleted in the future if the central cvmfs files work well)
# from DHNLAlgorithm.prw_files import prw_files_local as PRWList
# option 2. use centrally produced CVMFS files. This is still being tested as there are potentially issues.
# see https://indico.cern.ch/event/892901/contributions/3779966/attachments/2002909/3344068/sampleRequest.pdf
import DHNLAlgorithm.prw_files as prw_files
PRWList = []
if 'mc16a' in sample_periods: PRWList.extend(prw_files.prw_files_mc16a)
if 'mc16d' in sample_periods: PRWList.extend(prw_files.prw_files_mc16d)
if 'mc16e' in sample_periods: PRWList.extend(prw_files.prw_files_mc16e)

# Lumicalc Files
# Must be careful about which lines are commented and which are active.
# Note: if you want to use all lumicalc files without deactivating any,
# then the PRW files for ALL MC CAMPAIGNS must be loaded in the PRWList.
# If this isn't done the pileup reweighting tool will crash the algorithm.
# Note 2: These files are fairly large (~20 MB) so they will not be kept
# in the git repository for now. You may need to copy them from cvmfs
# and store them in $TestArea/DHNLAlgorithm/data/GRL/ to run on grid.
lumicalcList = []
if 'mc16a' in sample_periods: 
    lumicalcList.extend([
        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20190708/ilumicalc_histograms_None_276262-284484_OflLumi-13TeV-010.root',
        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20190708/ilumicalc_histograms_None_267638-271744_OflLumi-13TeV-010.root',
        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20190708/ilumicalc_histograms_None_297730-311481_OflLumi-13TeV-010.root',
    ])
if 'mc16d' in sample_periods: 
    lumicalcList.extend([
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/ilumicalc_histograms_None_325713-340453_OflLumi-13TeV-010.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/ilumicalc_histograms_None_341294-341649_OflLumi-13TeV-001.root',
    ])
if 'mc16e' in sample_periods: 
    lumicalcList.extend([
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_354396-355468_OflLumi-13TeV-001.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root',
    ])

GRL       = ",".join(GRLList)
PRW       = ",".join(PRWList)
LUMICALCS = ",".join(lumicalcList)


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%% BasicEventSelection %%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
basicEventSelectionDict = {
    "m_name"                      : "BasicEventSelect",
    "m_applyGRLCut"               : not args.is_MC,
    "m_GRLxml"                    : GRL,
    #"m_derivationName"            : "SUSY15Kernel_skim",
    "m_useMetaData"               : False, #fake AODs dont have complete metadata
    "m_storePassHLT"              : False, # no Trigger menu in fakeAOD
    "m_storeTrigDecisions"        : False, # no Trigger menu in fakeAOD
    "m_storePassL1"               : False, # no Trigger menu in fakeAOD
    "m_storeTrigKeys"             : False, # no Trigger menu in fakeAOD
    "m_applyTriggerCut"           : False, # no Trigger menu in fakeAOD
    "m_doPUreweighting"           : False if o.noPRW else args.is_MC,
    "m_PRWFileNames"              : PRW,
    "m_lumiCalcFileNames"         : LUMICALCS,
    "m_autoconfigPRW"             : False,
    "m_triggerSelection"          : "", # no trigger selection in fake AOD
    "m_checkDuplicatesData"       : False,
    "m_applyEventCleaningCut"     : False,
    "m_applyCoreFlagsCut"         : False,
    "m_vertexContainerName"       : "PrimaryVertices",
    "m_applyPrimaryVertexCut"     : True,
    "m_PVNTrack"                    : 2,
    "m_msgLevel"                  : "Info",
}

c.algorithm("BasicEventSelection", basicEventSelectionDict)


# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%% MuonCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# MuonCalibratorDict = {
#     "m_name"                      : "MuonCalibrate",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "Muons",
#     "m_outContainerName"          : "Muons_Calibrate",
#     #----------------------- Systematics ----------------------------#
#     "m_systName"                  : "",
#     "m_systVal"                   : 0,
#     #----------------------- Other ----------------------------#
#     "m_forceDataCalib"            : False,
#     "m_sort"                      : True,
#     "m_msgLevel"                  : "Info"
# }

# c.algorithm("MuonCalibrator", MuonCalibratorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%% MuonSelector %%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# fake AOD doesnt have the variables to determine the quality and this will crash the code
# MuonSelectorDict = {
#     "m_name"                      : "MuonSelect",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "Muons_Calibrate",
#     "m_outContainerName"          : "Muons_Signal",
#     "m_createSelectedContainer"   : True,
#     #----------------------- Systematics ----------------------------#
#     "m_systName"                  : "",        ## Data
#     "m_systVal"                   : 0,
#     #----------------------- configurable cuts ----------------------------#
#     "m_muonQualityStr"            : "VeryLoose",
#     "m_pass_max"                  : -1,
#     "m_pass_min"                  : -1,
#     "m_pT_max"                    : 1e8,
#     "m_pT_min"                    : 1,
#     "m_eta_max"                   : 1e8,
#     "m_d0_max"                    : 1e8,
#     "m_d0sig_max"                 : 1e8,
#     "m_z0sintheta_max"            : 1e8,
#     #----------------------- isolation stuff ----------------------------#
#     "m_MinIsoWPCut"               : "",
#     "m_IsoWPList"                 : "FCLoose,FCTight" if o.isDerivation else "FixedCutHighPtTrackOnly",
#     #----------------------- trigger matching stuff ----------------------------#
#     # "m_singleMuTrigChains"        : "HLT_mu20_iloose_L1MU15, HLT_mu24_iloose, HLT_mu24_ivarloose, HLT_mu24_ivarmedium, HLT_mu26_imedium, HLT_mu26_ivarmedium, HLT_mu40, HLT_mu50, HLT_mu60_0eta105_msonly",
#     "m_singleMuTrigChains"        : "", # no trigger matching in fake AOD

#     #"m_minDeltaR"                 : 0.1,
#     #----------------------- Other ----------------------------#
#     "m_msgLevel"                  : "Info",
#     "m_removeEventBadMuon"        : False,
# }

# # Annoyingly, we must run the MuonSelector algorithm in order to store quality parameters even in the input container.
# c.algorithm("MuonSelector", MuonSelectorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%% ElectronCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# ElectronCalibratorDict = {
#     "m_name"                      : "ElectronCalibrate",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "Electrons",
#     "m_outContainerName"          : "Electrons_Calibrate",
#     #----------------------- Systematics ----------------------------#
#     "m_systName"                  : "Nominal",            ## For data
#     "m_systVal"                   : 0,                    ## For data
#     "m_esModel"                   : "es2016PRE",
#     "m_decorrelationModel"        : "1NP_v1",
#     #----------------------- Other ----------------------------#
#     "m_sort"                      : True,
#     "m_msgLevel"                  : "Info"
# }

# c.algorithm("ElectronCalibrator", ElectronCalibratorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%% ElectronSelector %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# fake AOD does not have the decorations avaliable to decorate the quality
# ElectronSelectorDict = {
#     "m_name"                      : "ElectronSelect",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "Electrons_Calibrate",
#     "m_outContainerName"          : "Electrons_Signal",
#     "m_createSelectedContainer"   : True,
#     #----------------------- PID ------------- ----------------------------#
#     "m_doLHPIDcut"                : False,
#     "m_LHOperatingPoint"          : "Medium",
#     #----------------------- configurable cuts ----------------------------#
#     "m_pass_max"                  : -1,
#     "m_pass_min"                  : -1,
#     "m_pT_max"                    : 1e8,
#     "m_pT_min"                    : 1,
#     "m_eta_max"                   : 1e8,
#     "m_d0_max"                    : 1e8,
#     "m_d0sig_max"                 : 1e8,
#     "m_z0sintheta_max"            : 1e8,
#     #----------------------- isolation stuff ----------------------------#
#     "m_MinIsoWPCut"               : "",
#     "m_IsoWPList"                 : "FCLoose,FCTight" if o.isDerivation else "Gradient",
#     #----------------------- trigger matching stuff ----------------------------#
#     # "m_singleElTrigChains"        : "HLT_e24_lhmedium_L1EM20VH, HLT_e24_lhtight_nod0_ivarloose, HLT_e26_lhtight_nod0, HLT_e26_lhtight_nod0_ivarloose, HLT_e60_lhmedium_nod0, HLT_e60_lhmedium, LT_e60_medium, HLT_e120_lhloose, HLT_e140_lhloose_nod0, HLT_e300_etcut",
#     "m_singleElTrigChains"        : "", # no trigger matching for fake AOD
#     #----------------------- Other ----------------------------#
#     # "m_IsoWPList"                 : "Gradient",
#     "m_msgLevel"                  : "Info"
# }
# # Annoyingly, we must run the ElectronSelector algorithm in order to store quality parameters even in the input container.
# c.algorithm("ElectronSelector", ElectronSelectorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#


for augstr in AugmentationVersionStrings: 

    SecondaryVertexSelectorDict = {
    "m_name"                 : "SecVtxSel_VSI",
    "m_mapInFile"            : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3.2_Inner.root",
    "m_mapOutFile"           : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3_Outer.root",
    "m_inContainerName"      : "VrtSecInclusive_SecondaryVertices"+augstr,
    "m_outContainerName"      : "VrtSecInclusive_SecondaryVertices"+augstr+"_sel",
    #---------------------- Selections ---------------------------#
    "m_do_trackTrimming"     : False,
    "m_do_matMapVeto"        : True,
    "prop_chi2Cut"           : 5.0,
    "prop_d0_wrtSVCut"       : 0.8,
    "prop_z0_wrtSVCut"       : 1.2,
    "prop_errd0_wrtSVCut"    : 999999,
    "prop_errz0_wrtSVCut"    : 999999,
    "prop_d0signif_wrtSVCut" : 5.0,
    "prop_z0signif_wrtSVCut" : 5.0,
    "prop_chi2_toSVCut"      : 5.0,
    "prop_vtx_suffix"        : "",
    #------------------------ Other ------------------------------#
    "m_msgLevel"             : "Info",
    }

    c.algorithm ( "SecondaryVertexSelector", SecondaryVertexSelectorDict )

    Dict_VertexMatcher = {
        "m_name"                            : "VertexMatch"+ augstr,
        "m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices" + augstr,   # --> use selected vertices
        "m_doTruth"                         : True,
        #------------------------ Lepton Matching ------------------------------#
        "m_doLeptons"                       : True,
        "m_fakeAOD"                         : True,
        "m_inMuContainerName"               : "",
        "m_inElContainerName"               : "",
        "m_VSILepmatch"                    : True if "Leptons" in augstr else False,
        "m_inTrackParticleContainerName"   : "TracksForVSI",
        #------------------------ Other ------------------------------#
        "m_msgLevel"             : "Info",
    }
    c.algorithm ( "VertexMatcher",           Dict_VertexMatcher   )




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
    "m_pdgIdList"               : "9900012, 50, 24, 443",
    #------------------------ Other ------------------------------#
    "m_msgLevel"             : "Info",

}

if args.is_MC:
    c.algorithm("TruthVertexSelector", TruthVertexSelectorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
##%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLAlgo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLDict = {
    "m_name"                    : "DHNLAlgo",
    #----------------------- Container Flow ----------------------------#
    "m_inMuContainerName"       : "Muons_Calibrate",
    "m_inElContainerName"       : "Electrons_Calibrate",
    "m_muInputAlgo"             : "",
    "m_elInputAlgo"             : "",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_AugmentationVersionStringList"    : ','.join(AugmentationVersionStrings),
    "m_inDetTrackParticlesContainerName" : "InDetTrackParticles",
    "m_backgroundEstimationNoParticleData": True,
    #----------------------- Selections ----------------------------#
    "m_doInverseLeptonControlRegion"   : False,
    "m_fakeAOD"                 : True,
    #----------------------- Other ----------------------------#
    "m_msgLevel"                : "Info",
    "m_trackingCalibFile"       : "InDetTrackSystematicsTools/CalibData_21.2_2018-v15/TrackingRecommendations_final_rel21.root",
    "m_doSkipTracks"            : False,
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
    "m_inElContainerName"            : "",
    "m_eventInfoContainerName"       : "",
    "m_inputAlgos"                   : "",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_secondaryVertexBranchNameList" : ','.join(secondaryVertexBranchNames),
    "m_AugmentationVersionStringList" : ','.join(AugmentationVersionStrings),
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering
    "m_truthVertexContainerName"     : "",
    "m_truthVertexBranchName"        : "",
    "m_inTruthParticleContainerName" : "",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "", #shapeEM
    "m_muDetailStr"                  : "",
    "m_elDetailStr"                  : "",
    "m_trigDetailStr"                : "",#basic menuKeys passTriggers",
    "m_secondaryVertexDetailStr"     : "tracks truth leptons fakeAOD", # "tracks" linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_vertexDetailStr"              : "primary",
    "m_truthVertexDetailStr"         : "isMatched", # Uses pt-matching to match reconstructed vertices.
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

from xAODAnaHelpers import Config
import os
import shlex
import argparse


parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isSUSY15', dest='isSUSY15', action="store_true", default=False)
parser.add_argument('--runAllSyst', dest='runAllSyst', action="store_true", default=False)
parser.add_argument('--noPRW', dest='noPRW', action="store_true", default=False)
o = parser.parse_args(shlex.split(args.extra_options))

c = Config()


# vertex container information (by default run VSI & VSI Leptons)
if o.isSUSY15:
    secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices_LeptonsMod_LRTR3_1p0"]
    secondaryVertexBranchNames = ["secVtx_VSI_LeptonsMod"]
    AugmentationVersionStrings = ["_LeptonsMod_LRTR3_1p0"]
    # VrtSecInclusive_SecondaryVertices_FixedExtroplator in SUSY15
else:
    secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices_Leptons"]
    secondaryVertexBranchNames = ["secVtx_VSI_Leptons"]
    AugmentationVersionStrings = ["_Leptons"]

# Good Run Lists
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/GoodRunListsForAnalysisRun2#Naming_scheme_and_documentation
GRLList = [
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20190708/data15_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns.xml',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20190708/data16_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_WITH_IGNORES.xml',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/data17_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190708/data18_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
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
    # mc16a 
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20190708/ilumicalc_histograms_None_276262-284484_OflLumi-13TeV-010.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20190708/ilumicalc_histograms_None_267638-271744_OflLumi-13TeV-010.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20190708/ilumicalc_histograms_None_297730-311481_OflLumi-13TeV-010.root',
    # mc16d
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/ilumicalc_histograms_None_325713-340453_OflLumi-13TeV-010.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/ilumicalc_histograms_None_341294-341649_OflLumi-13TeV-001.root',
    # mc16e 
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_354396-355468_OflLumi-13TeV-001.root',
    '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190708/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root',
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
    #-------------------------- GRL --------------------------------------#
    "m_GRLxml"                    : GRL,
    #-------------------------- Derivation -------------------------------#
    # "m_derivationName"            : "SUSY15Kernel_skim",
    #-------------------------- PRW --------------------------------------#
    "m_doPUreweighting"           : False if o.noPRW else True,
    "m_PRWFileNames"              : PRW,
    "m_lumiCalcFileNames"         : lumicalcs,
    "m_autoconfigPRW"             : False,
    #-------------------------- Trigger ----------------------------------#
    "m_triggerSelection"          : "HLT_mu20_iloose_L1MU15 || HLT_mu24_iloose || HLT_mu24_ivarloose || HLT_mu24_imedium || HLT_mu24_ivarmedium || HLT_mu26_imedium || HLT_mu26_ivarmedium || HLT_mu60_0eta105_msonly || HLT_e24_lhmedium_L1EM20VH || HLT_e24_lhtight_nod0_ivarloose || HLT_e26_lhtight_nod0 || HLT_e26_lhtight_nod0_ivarloose || HLT_e60_lhmedium_nod0 || HLT_e140_lhloose_nod0",
    "m_storePassHLT"              : True,
    "m_storeTrigDecisions"        : True,
    "m_storePassL1"               : True,
    "m_storeTrigKeys"             : True,
    "m_applyTriggerCut"           : not args.is_MC,
    #---------------------------- Cuts ----------------------------------#
    "m_checkDuplicatesData"       : False,
    "m_applyEventCleaningCut"     : False,
    "m_applyCoreFlagsCut"         : False,
    "m_vertexContainerName"       : "PrimaryVertices",
    "m_applyPrimaryVertexCut"     : True,
    "m_PVNTrack"                    : 2,
    #---------------------------- Other ---------------------------------#
    "m_useMetaData"               : True,
    "m_msgLevel"                  : "Info",
}

c.algorithm("BasicEventSelection", basicEventSelectionDict)


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
##%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLFilter%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLFilterDict = {
    "m_name"                    : "DHNLFilter",
    #----------------------- Container Flow ----------------------------#

    "m_allJetContainerName"     : "AntiKt4EMTopoJets"if not o.isSUSY15 else "AntiKt4EMTopoJets_BTagging201810",
    "m_inMuContainerName"       : "Muons",
    "m_inElContainerName"       : "Electrons",
    "m_vertexContainerName"     : "PrimaryVertices",
    "m_applyFilterCut"          : False,
    # "m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices",

    #----------------------- Selections ----------------------------#

    # All selections are stored in default parameters in filter.
    # they can still be modified here. e.g.:
    # "m_AlphaMaxCut"             : 0.03,
    "m_electronLHWP"              : "Medium" if not o.isSUSY15 else "DFCommonElectronsLHMedium",
    "m_el1IDKey"                  :  "LHLoose", # if not o.isSUSY15 else "DFCommonElectronsLHLoose", # if you didnt add LHLoose to the SUSy15 config you need to update the electron quality
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
    "m_systName"                  : "All" if o.runAllSyst else "",
    "m_systVal"                   : 1.0,
    "m_outputAlgoSystNames"       : "MuonCalibrator_Syst",
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
    "m_inputAlgoSystNames"        : "MuonCalibrator_Syst",
    "m_outputAlgoSystNames"       : "MuonSelector_Syst",
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
    "m_IsoWPList"                 : "FCLoose,FCTight" if o.isSUSY15 else "FixedCutHighPtTrackOnly",
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
#%%%%%%%%%%%%%%%%%%%%%%%% MuonEfficiencyCorrector %%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#  
MuonEfficiencyCorrectorDict = {
  "m_name"                      : "MuonEfficiencyCorrector",
  #----------------------- Container Flow ----------------------------#
  "m_inContainerName"           : "PLACEHOLDER_CONTAINER", # should be Muons_Signal
  #----------------------- Systematics ----------------------------#
  "m_inputSystNamesMuons"       : "MuonSelector_Syst",
  "m_systNameReco"              : "All",
  "m_systValReco"               : 1.0,
  "m_systNameIso"               : "All",
  "m_systValIso"                : 1.0,
  "m_systNameTrig"              : "All",
  "m_systValTrig"               : 1.0,
  "m_MuTrigLegs"                : "2015:HLT_mu20_iloose_L1MU15_OR_HLT_mu50,2016:HLT_mu26_ivarmedium_OR_HLT_mu50,2017:HLT_mu26_ivarmedium_OR_HLT_mu50,2018:HLT_mu26_ivarmedium_OR_HLT_mu50",
#  "m_MuTrigLegs"                : "HLT_mu26_ivarmedium_OR_HLT_mu50",
#  "m_MuTrigLegs"                : "HLT_mu50",
  #----------------------- Working Points ----------------------------#
  "m_WorkingPointReco"          : "Medium",
  "m_WorkingPointIso"           : "FCLoose",
  #----------------------- Other ----------------------------#
  "m_writeSystToMetadata"       : True,
  "m_msgLevel"                  : "Info"
}

c.algorithm("MuonEfficiencyCorrector", MuonEfficiencyCorrectorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%% ElectronCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
ElectronCalibratorDict = {
    "m_name"                      : "ElectronCalibrate",
    #----------------------- Container Flow ----------------------------#
    "m_inContainerName"           : "Electrons",
    "m_outContainerName"          : "Electrons_Calibrate",
    #----------------------- Systematics ----------------------------#
    "m_systName"                  : "All" if o.runAllSyst else "",
    "m_systVal"                   : 1.0,
    "m_esModel"                   : "es2018_R21_v0", # recommendation as of May 11 2020
    "m_decorrelationModel"        : "1NP_v1",        # likely sufficient for our needs (NPs summed in quadrature)
    "m_outputAlgoSystNames"       : "ElectronCalibrator_Syst",
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
    #----------------------- Systematics ----------------------------#
    "m_inputAlgoSystNames"        : "ElectronCalibrator_Syst",
    "m_outputAlgoSystNames"       : "ElectronSelector_Syst",
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
    "m_IsoWPList"                 : "FCLoose,FCTight" if o.isSUSY15 else "Gradient",
    #----------------------- trigger matching stuff ----------------------------#
    "m_singleElTrigChains"        : "HLT_e24_lhmedium_L1EM20VH, HLT_e24_lhtight_nod0_ivarloose, HLT_e26_lhtight_nod0, HLT_e26_lhtight_nod0_ivarloose, HLT_e60_lhmedium_nod0, HLT_e140_lhloose_nod0",
    #----------------------- Other ----------------------------#
    "m_msgLevel"                  : "Info"
}
# Annoyingly, we must run the ElectronSelector algorithm in order to store quality parameters even in the input container.
c.algorithm("ElectronSelector", ElectronSelectorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
SecondaryVertexSelectorDict = {
    "m_name"                 : "SecVtxSel_VSI",
    "m_mapInFile"            : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3.2_Inner.root",
    "m_mapOutFile"           : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3_Outer.root",
    "m_inContainerName"      : secondaryVertexContainerNames[0],
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


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#


for augstr in AugmentationVersionStrings: 

    Dict_VertexMatcher = {
        "m_name"                            : "VertexMatch"+ augstr,
        "m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices" + augstr,   # --> use selected vertices
        "m_doTruth"                         : True,
        #------------------------ Lepton Matching ------------------------------#
        "m_doLeptons"                       : True,
        "m_inMuContainerName"               : "Muons",
        "m_inElContainerName"               : "Electrons",
         "m_VSILepmatch"                    : True if "Leptons" in augstr else False,
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
    "m_inJetContainerName"      : "SignalJets",
    "m_inputAlgo"               : "SignalJets_Algo",
    "m_allJetContainerName"     : "AntiKt4EMTopoJets_Calib",
    "m_allJetInputAlgo"         : "AntiKt4EMTopoJets_Calib_Algo",
    "m_inMuContainerName"       : "Muons_Calibrate",
    "m_inElContainerName"       : "Electrons_Calibrate",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    # "m_inMETContainerName"      : "MET",
    # "m_inMETTrkContainerName"   : "METTrk",
    #----------------------- Selections ----------------------------#
    "m_leadingJetPtCut"         : 20,
    "m_subleadingJetPtCut"      : 20,
    "m_jetMultiplicity"         : 2,
    "m_useMCPileupCheck"        : False,
    "m_metCut"                  : 20000,
    "m_doInverseLeptonControlRegion"   : False,
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
    "m_inMuContainerName"            : "Muons_Calibrate",
    "m_inElContainerName"            : "Electrons_Calibrate",
    "m_inputAlgos"                    : ",".join(["MuonSelector_Syst", "ElectronSelector_Syst"]) if o.runAllSyst else "",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_secondaryVertexBranchNameList" : ','.join(secondaryVertexBranchNames),
    "m_AugmentationVersionStringList" : ','.join(AugmentationVersionStrings),
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering 
    "m_truthVertexContainerName"     : "SelectedTruthVertices",
    "m_truthVertexBranchName"        : "truthVtx",
    "m_inTruthParticleContainerName" : "MuonTruthParticles",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth pileup", #shapeEM
    "m_elDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation trackparams PID PID_Loose PID_Medium PID_Tight PID_LHLoose PID_LHMedium PID_LHTight PID_MultiLepton effSF PIDSF_Medium TRIG_SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0",
    "m_muDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation trackparams quality RECO_Tight RECO_Medium RECO_Loose energyLoss effSF TRIG_HLT_mu20_iloose_L1MU15_OR_HLT_mu50 TRIG_HLT_mu26_ivarmedium_OR_HLT_mu50",
    "m_trigDetailStr"                : "basic passTriggers",#basic menuKeys passTriggers",
    "m_secondaryVertexDetailStr"     : "tracks truth leptons", # "tracks" linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_vertexDetailStr"              : "primary",
    "m_truthVertexDetailStr"         : "isMatched", # Uses pt-matching to match reconstructed vertices.
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

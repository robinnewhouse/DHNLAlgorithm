from xAODAnaHelpers import Config
import os

c = Config()

# Good Run Lists
GRLList = [
    '$TestArea/DHNLAlgorithm/data/GRL/data15_13TeV/20170619/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml',
    '$TestArea/DHNLAlgorithm/data/GRL/data16_13TeV/20180129/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml',
    '$TestArea/DHNLAlgorithm/data/GRL/data17_13TeV/20180619/data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
    '$TestArea/DHNLAlgorithm/data/GRL/data18_13TeV/20190318/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml',
]

# Pileup Reweighting
# The sample you're running over must have the PRW file available.
# If you are getting errors such as "Unrecognised channelNumber 311660 for periodNumber 300000" this is the place to start.
# Load the PRW File locations from an external file. Comment any lines you don't want with '#'.
PRWFiles = os.environ['TestArea']+'/DHNLAlgorithm/data/PRW/PRWFiles.txt'
with open(PRWFiles) as f:
    PRWList = [line.rstrip() for line in f] # read lines
    PRWList = [line for line in PRWList if line]  # remove empty lines
    PRWList = [line for line in PRWList if not line.startswith("#")]  # remove comments

# Lumicalc Files
# Must be careful about which lines are commented and which are active.
# Note: if you want to use all lumicalc files without deactivating any,
# then the PRW files for ALL MC CAMPAIGNS must be loaded in the PRWList.
# If this isn't done the pileup reweighting tool will crash the algorithm.
# Note 2: These files are fairly large (~20 MB) so they will not be kept
# in the git repository for now . You may need to copy them from cvmfs
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
    "m_doPUreweighting"           : False,
    "m_PRWFileNames"              : PRW,
    "m_lumiCalcFileNames"         : lumicalcs,
    "m_autoconfigPRW"             : False,
    "m_triggerSelection"          : "HLT_mu26_ivarmedium || HLT_3mu6_msonly || HLT_j30_jes_cleanLLP_PS_llp_L1TAU60 || HLT_j30_jes_cleanLLP_PS_llp_L1TAU100 || HLT_j30_jes_cleanLLP_PS_llp_L1LLP-NOMATCH || HLT_j30_jes_cleanLLP_PS_llp_L1LLP-RO || HLT_j30_muvtx || HLT_j30_muvtx_noiso ",
    "m_checkDuplicatesData"       : False,
    "m_applyEventCleaningCut"     : False,
    "m_applyCoreFlagsCut"	        : False,
    "m_vertexContainerName"       : "PrimaryVertices",
    "m_applyPrimaryVertexCut"     : False,
    "m_PVNTrack"		            : 2,
    "m_msgLevel"                  : "Info",
}

c.algorithm("BasicEventSelection", basicEventSelectionDict)


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
##%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLFilter%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLFilterDict = {
    "m_name"                    : "DHNLFilter",
    #----------------------- Container Flow ----------------------------#

    "m_allJetContainerName"     : "AntiKt4EMTopoJets",
    "m_inMuContainerName"       : "Muons",
    "m_inElContainerName"       : "Electrons",
    "m_vertexContainerName"     : "PrimaryVertices",
    # "m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices",

    #----------------------- Selections ----------------------------#

    # All selections are stored in default parameters in filter.
    # they can still be modified here. e.g.:
    # "m_AlphaMaxCut"             : 0.03,

    #----------------------- Other ----------------------------#
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLFilter", DHNLFilterDict )



# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%% JetCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# JetCalibratorDict = {
#     "m_name"                      : "JetCalibrate",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "AntiKt4EMTopoJets",
#     "m_jetAlgo"                   : "AntiKt4EMTopo",
#     "m_outContainerName"          : "AntiKt4EMTopoJets_Calib",
#     "m_outputAlgo"                : "AntiKt4EMTopoJets_Calib_Algo",
#     "m_sort"                      : True,
#     "m_redoJVT"                   : False,
#     #----------------------- Systematics ----------------------------#
#     "m_systName"                  : "Nominal",            ## For data
#     "m_systVal"                   : 0,                    ## For data
#     #----------------------- Calibration ----------------------------#
#     "m_calibConfigAFII"           : "JES_MC16Recommendation_AFII_EMTopo_April2018_rel21.config",
#     "m_calibConfigFullSim"        : "JES_MC16Recommendation_28Nov2017.config",
#     "m_calibConfigData"           : "JES_data2017_2016_2015_Recommendation_Feb2018_rel21.config",
#     "m_calibSequence"             : "JetArea_Residual_EtaJES_GSC",
#     "m_forceInsitu"               : False, # Insitu calibration file not found in some cases
#     #----------------------- JES Uncertainty ----------------------------#
#     #"m_uncertConfig"               : "rel21/Moriond2018/R4_StrongReduction_Scenario1.config",
#     #"m_uncertMCType"               : "MC16",
#     "m_uncertConfig"           : "rel21/Moriond2018/R4_StrongReduction_Scenario1.config",
#     "m_uncertMCType"           : "MC16",
#     #----------------------- JER Uncertainty ----------------------------#
#     #"m_JERUncertConfig"           : "JetResolution/Prerec2015_xCalib_2012JER_ReducedTo9NP_Plots_v2.root",
#     #"m_JERFullSys"                : False,
#     #"m_JERApplyNominal"           : False,
#     #----------------------- Cleaning ----------------------------#
#     "m_jetCleanCutLevel"          : "LooseBad",
#     "m_jetCleanUgly"              : False,
#     "m_saveAllCleanDecisions"     : True,
#     "m_cleanParent"               : False,
#     #----------------------- Other ----------------------------#
#     "m_msgLevel"                  : "Info",
# }
#
# c.algorithm("JetCalibrator",  JetCalibratorDict )
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%% JetSelector %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# JetSelectorDict = {
#     "m_name"                      : "JetSelect",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "AntiKt4EMTopoJets_Calib",
#     "m_outContainerName"          : "SignalJets",
#     #"m_truthJetContainer"         : "",
#     "m_inputAlgo"                 : "AntiKt4EMTopoJets_Calib_Algo",
#     "m_outputAlgo"                : "SignalJets_Algo",
#     "m_decorateSelectedObjects"   : True,
#     "m_createSelectedContainer"   : True,
#     #----------------------- Selections ----------------------------#
#     "m_cleanJets"                 : False,
#     "m_pass_min"                  : -1,
#     "m_pT_min"                    : 20,
#     "m_eta_max"                   : 1e8,
#     #----------------------- JVT ----------------------------#
#     "m_doJVT"                     : False,
#     "m_pt_max_JVT"                : 60e3,
#     "m_eta_max_JVT"               : 2.4,
#     "m_JVTCut"                    : 0.59,
#     #----------------------- B-tagging ----------------------------#
#     "m_doBTagCut"                 : False,
#     #----------------------- Other ----------------------------#
#     "m_msgLevel"                  : "Info",
# }
#
# c.algorithm("JetSelector", JetSelectorDict )

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
#
# c.algorithm("MuonCalibrator", MuonCalibratorDict )
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%% MuonSelector %%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
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
#     #----------------------- trigger matching stuff ----------------------------#
#     "m_singleMuTrigChains"        : "",
#     #"m_minDeltaR"                 : 0.1,
#     #----------------------- Other ----------------------------#
#     "m_msgLevel"                  : "Info",
#     "m_removeEventBadMuon"        : False,
#     "m_IsoWPList"                 : "FixedCutHighPtTrackOnly",
# }
#
# c.algorithm("MuonSelector", MuonSelectorDict )

# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%% ElectronCalibrator %%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
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
#
# c.algorithm("ElectronCalibrator", ElectronCalibratorDict )

# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%% ElectronSelector %%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# ElectronSelectorDict = {
#     "m_name"                      : "ElectronSelect",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "Electrons_Calibrate",
#     "m_outContainerName"          : "Electrons_Signal",
#     "m_createSelectedContainer"   : True,
#     #----------------------- PID ------------- ----------------------------#
#     "m_doLHPIDcut"                : True,
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
#     #----------------------- trigger matching stuff ----------------------------#
#     "m_singleElTrigChains"        : "",
#     #----------------------- Other ----------------------------#
#     "m_IsoWPList"                 : "Gradient",
#     "m_msgLevel"                  : "Info"
# }
#
# c.algorithm("ElectronSelector", ElectronSelectorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%% METTrk Constructor %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
METTrkConstructorDict = {
    "m_name"                      : "MetTrkConstruct",
    "m_referenceMETContainer"     : "MET_Reference_AntiKt4EMTopo",
    "m_mapName"                   : "METAssoc_AntiKt4EMTopo",
    "m_coreName"                  : "MET_Core_AntiKt4EMTopo",
    "m_outputContainer"           : "METTrk",
    "m_outputAlgoSystNames"       : "METTrk_Syst",
    "m_writeSystToMetadata"       : False,
    "m_setAFII"                   : True,
    "m_calculateSignificance"     : True,
    "m_doPhotonCuts"              : True,
    "m_doElectronCuts"            : True,
    "m_addSoftClusterTerms"       : False,
    "m_rebuildUsingTracksInJets"  : True,
    "m_inputElectrons"            : "Electrons",
    "m_inputMuons"                : "Muons",
    #"m_inputTaus"                 : "TauJets",
    "m_inputJets"                 : "AntiKt4EMTopoJets",
    "m_runNominal"                : True,
    #"m_eleSystematics"            : "ElectronSelector_Syst",
    #"m_muonSystematics"           : "MuonSelector_Syst",
    #"m_tauSystematics"            : "TauSelector_Syst",
    #"m_jetSystematics"            : "JetSelector_Syst",
    "m_doJVTCut"                  : True,
    "m_dofJVTCut"                 : False,
    "m_calculateSignificance"     : False,
    "m_msgLevel"                  : "Info"
}

c.algorithm("METConstructor", METTrkConstructorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%% MET Constructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
MetConstructorDict = {
    "m_name"                      : "MetConstruct",
    "m_referenceMETContainer"     : "MET_Reference_AntiKt4EMTopo",
    "m_mapName"                   : "METAssoc_AntiKt4EMTopo",
    "m_coreName"                  : "MET_Core_AntiKt4EMTopo",
    "m_outputContainer"           : "MET",
    "m_outputAlgoSystNames"       : "MET_Syst",
    "m_writeSystToMetadata"       : False,
    "m_setAFII"                   : True,
    "m_calculateSignificance"     : True,
    "m_doPhotonCuts"              : True,
    "m_doElectronCuts"            : True,
    "m_addSoftClusterTerms"       : False,
    "m_rebuildUsingTracksInJets"  : False,
    "m_inputElectrons"            : "Electrons",
    "m_inputMuons"                : "Muons",
    #"m_inputTaus"                 : "TauJets",
    "m_inputJets"                 : "AntiKt4EMTopoJets",
    "m_runNominal"                : True,
    #"m_eleSystematics"            : "ElectronSelector_Syst",
    #"m_muonSystematics"           : "MuonSelector_Syst",
    #"m_tauSystematics"            : "TauSelector_Syst",
    #"m_jetSystematics"            : "JetSelector_Syst",
    "m_doJVTCut"                  : True,
    "m_dofJVTCut"                 : False,
    "m_calculateSignificance"     : False,
    "m_msgLevel"                  : "Info"
}

c.algorithm("METConstructor", MetConstructorDict )

# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# SecondaryVertexSelectorDict = {
#     "m_name"                 : "SecVtxSel",
#     "m_mapInFile"            : "$TestArea/DHNLAlgorithm/deps/DVAnalysisBase/deps/FactoryTools/data/DV/MaterialMap_v3.2_Inner.root",
#     "m_mapOutFile"           : "$TestArea/DHNLAlgorithm/deps/DVAnalysisBase/deps/FactoryTools/data/DV/MaterialMap_v3_Outer.root",
#     #---------------------- Selections ---------------------------#
#     "m_do_trackTrimming"     : True,
#     "m_do_matMapVeto"        : True,
#     "prop_chi2Cut"           : 5.0,
#     "prop_d0_wrtSVCut"       : 0.8,
#     "prop_z0_wrtSVCut"       : 1.2,
#     "prop_errd0_wrtSVCut"    : 999999,
#     "prop_errz0_wrtSVCut"    : 999999,
#     "prop_d0signif_wrtSVCut" : 5.0,
#     "prop_z0signif_wrtSVCut" : 5.0,
#     "prop_chi2_toSVCut"      : 5.0,
#     #------------------------ Other ------------------------------#
#     "m_msgLevel"             : "Info",
# }
#
# c.algorithm ( "SecondaryVertexSelector", SecondaryVertexSelectorDict )

# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# Dict_VertexMatcher = {
#     "m_name"                           : "VertexMatch",
#     "m_msgLevel"                       : "Info",
#     "m_inSecondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices",   # --> use selected vertices
#     "m_inMuContainerName"            : "Mons_Calibrate",
#     "m_inElContainerName"            : "Electrons_Calibrate",
#     #------------------------ Other ------------------------------#
#     "m_msgLevel"             : "Info",
# }
# # Vertex Matching
# if args.is_MC:
#     c.algorithm ( "VertexMatcher",           Dict_VertexMatcher           )



# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%% TruthSelector %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# TruthSelectorDict = {
#     "m_name"                      : "TruthSelect",
#     #----------------------- Container Flow ----------------------------#
#     "m_inContainerName"           : "Muons_Signal",
#     "m_outContainerName"          : "Muons_Truth",
#     #------------------------ Other ------------------------------#
#     "m_msgLevel"             : "Debug",
# }
#
# if args.is_MC:
#     c.algorithm("TruthSelector", TruthSelectorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
##%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLAlgo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLDict = {
    "m_name"                    : "DHNLAlgo",
    #----------------------- Container Flow ----------------------------#
    "m_inJetContainerName"      : "AntiKt4EMTopoJets",
    "m_inputAlgo"               : "AntiKt4EMTopoJets_Algo",
    "m_allJetContainerName"     : "AntiKt4EMTopoJets",
    "m_allJetInputAlgo"         : "AntiKt4EMTopoJets_Algo",
    "m_inMuContainerName"       : "Muons",
    "m_inElContainerName"       : "Electrons",
    "m_inMETContainerName"      : "MET",
    "m_inMETTrkContainerName"   : "METTrk",
    #----------------------- Selections ----------------------------#
    "m_leadingJetPtCut"         : 20,
    "m_subleadingJetPtCut"      : 20,
    "m_jetMultiplicity"         : 2,
    "m_useMCPileupCheck"        : False,
    "m_metCut"                  : 20000,
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
    "m_inJetContainerName"           : "AntiKt4EMTopoJets",
    "m_inputAlgo"                    : "",#"SignalJets_Algo",
    "m_allJetContainerName"          : "AntiKt4EMTopoJets",
    "m_allJetInputAlgo"              : "",#"AntiKt4EMTopoJets_Calib_Algo",
    "m_inMuContainerName"            : "Muons",
    "m_inElContainerName"            : "Electrons",
    "m_inMETContainerName"           : "MET",
    "m_inMETTrkContainerName"        : "METTrk",
    "m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices", # --> use selected DVs
    "m_AugumentationVersionString"   : "", # no augumentation for standard VSI
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering     "m_secondaryVertexBranchName"    : "secVtx",
    "m_secondaryVertexBranchName"    : "secVtx",
    "m_truthVertexContainerName"     : "TruthVertices",
    "m_truthVertexBranchName"        : "truthVtx",
    "m_inTruthParticleContainerName" : "TruthParticles",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth pileup", #shapeEM
    "m_jetDetailStr"                 : "",
    "m_jetDetailStrSyst"             : "",
    "m_elDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation isolationKinematics trackparams trackhitcont effSF PID PID_Loose PID_Medium PID_Tight PID_LHLoose PID_LHMedium PID_LHTight PID_MultiLepton ",
    "m_muDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation isolationKinematics trackparams trackhitcont effSF quality RECO_Tight RECO_Medium RECO_Loose energyLoss ",
    "m_trigDetailStr"                : "basic passTriggers",#basic menuKeys passTriggers",
    "m_metDetailStr"                 : "",
    "m_metTrkDetailStr"              : "",
    # "m_trackDetailStr"               : "fitpars",
    "m_secondaryVertexDetailStr"     : "", # "linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_truthVertexDetailStr"         : "", # Uses pt-matching to match reconstructed vertices.
    "m_truthParticleDetailString"    : "kinematic", # type parents children bVtx
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

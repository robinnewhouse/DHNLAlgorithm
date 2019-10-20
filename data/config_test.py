from xAODAnaHelpers import Config

c = Config()

GRLList      = ['$TestArea/DHNLAlgorithm/data/GRL/physics_25ns_Triggerno17e33prim.xml' ]
PRWList      = [
    '$TestArea/DHNLAlgorithm/data/PRW/mc16_13TeV.311660.Pythia8EvtGen_A14NNPDF23LO_WmuHNL50_20G_lt10dd_el.merge.NTUP_PILEUP.e7422_e5984_a875_r10740_r10706_p3384_p3385/NTUP_PILEUP.18061252._000001.pool.root.1',
]
lumicalcList = ['$TestArea/DHNLAlgorithm/data/lumi/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root',
                '$TestArea/DHNLAlgorithm/data/lumi/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root']
#lumicalcList = ['$TestArea/DHNLAlgorithm/data/lumi/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-001.root',
#                '$TestArea/DHNLAlgorithm/data/lumi/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root']

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
    "m_doPUreweighting"           : args.is_MC,
    "m_PRWFileNames"              : PRW,
    "m_lumiCalcFileNames"         : lumicalcs,
    "m_autoconfigPRW"             : False,
    "m_triggerSelection"          : "HLT_mu26_ivarmedium || HLT_e140_lhloose_nod0 || HLT_e60_lhmedium_nod0 || HLT_e26_lhtight_nod0_ivarloose",
    "m_checkDuplicatesData"       : False,
    "m_applyEventCleaningCut"     : False,
    "m_applyCoreFlagsCut"	        : False,
    "m_vertexContainerName"       : "PrimaryVertices",
    "m_applyPrimaryVertexCut"     : False,
    "m_PVNTrack"		            : 2,
    "m_msgLevel"                  : "Info",
}

c.algorithm("BasicEventSelection", basicEventSelectionDict)
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# ##%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Hto4bFilter%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# Hto4bLLPFilterDict = {
#     "m_name"                    : "Hto4bLLPFilter",
#     #----------------------- Container Flow ----------------------------#
#
#     "m_allJetContainerName"     : "AntiKt4EMTopoJets",
#     "m_inMuContainerName"       : "Muons",
#     "m_inElContainerName"       : "Electrons",
#     #----------------------- Selections ----------------------------#
#
#     "m_TrackMinPt"              : 400,
#     "m_TrackZ0Max"              : 0.3,
#     "m_TrackD0Max"              : 0.5,
#     "m_jetPtCut"                : 20,
#     "m_AlphaMaxCut"             : 0.03,
#     "m_CHFCut"                  : 0.3,
#     "m_electronPtCut"           : 27000,
#     "m_muonPtCut"               : 25000,
#     #----------------------- Other ----------------------------#
#
#     "m_msgLevel"                : "Info",
# }

# c.algorithm("Hto4bLLPFilter", Hto4bLLPFilterDict )
#
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
#
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
#     "m_IsoWPList"                 : "LooseTrackOnly,Loose,Gradient,GradientLoose",
# }
#
# c.algorithm("MuonSelector", MuonSelectorDict )
#
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
#
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
#     "m_IsoWPList"                 : "Loose",
#     "m_msgLevel"                  : "Info"
# }
#
# c.algorithm("ElectronSelector", ElectronSelectorDict )
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%% METTrk Constructor %%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# METTrkConstructorDict = {
#     "m_name"                      : "MetTrkConstruct",
#     "m_referenceMETContainer"     : "MET_Reference_AntiKt4EMTopo",
#     "m_mapName"                   : "METAssoc_AntiKt4EMTopo",
#     "m_coreName"                  : "MET_Core_AntiKt4EMTopo",
#     "m_outputContainer"           : "METTrk",
#     "m_outputAlgoSystNames"       : "METTrk_Syst",
#     "m_writeSystToMetadata"       : False,
#     "m_setAFII"                   : True,
#     "m_calculateSignificance"     : True,
#     "m_doPhotonCuts"              : True,
#     "m_doElectronCuts"            : True,
#     "m_addSoftClusterTerms"       : False,
#     "m_rebuildUsingTracksInJets"  : True,
#     "m_inputElectrons"            : "Electrons",
#     "m_inputMuons"                : "Muons",
#     #"m_inputTaus"                 : "TauJets",
#     "m_inputJets"                 : "AntiKt4EMTopoJets",
#     "m_runNominal"                : True,
#     #"m_eleSystematics"            : "ElectronSelector_Syst",
#     #"m_muonSystematics"           : "MuonSelector_Syst",
#     #"m_tauSystematics"            : "TauSelector_Syst",
#     #"m_jetSystematics"            : "JetSelector_Syst",
#     "m_doJVTCut"                  : True,
#     "m_dofJVTCut"                 : False,
#     "m_calculateSignificance"     : False,
#     "m_msgLevel"                  : "Info"
# }
#
# c.algorithm("METConstructor", METTrkConstructorDict )
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%% MET Constructor %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# MetConstructorDict = {
#     "m_name"                      : "MetConstruct",
#     "m_referenceMETContainer"     : "MET_Reference_AntiKt4EMTopo",
#     "m_mapName"                   : "METAssoc_AntiKt4EMTopo",
#     "m_coreName"                  : "MET_Core_AntiKt4EMTopo",
#     "m_outputContainer"           : "MET",
#     "m_outputAlgoSystNames"       : "MET_Syst",
#     "m_writeSystToMetadata"       : False,
#     "m_setAFII"                   : True,
#     "m_calculateSignificance"     : True,
#     "m_doPhotonCuts"              : True,
#     "m_doElectronCuts"            : True,
#     "m_addSoftClusterTerms"       : False,
#     "m_rebuildUsingTracksInJets"  : False,
#     "m_inputElectrons"            : "Electrons",
#     "m_inputMuons"                : "Muons",
#     #"m_inputTaus"                 : "TauJets",
#     "m_inputJets"                 : "AntiKt4EMTopoJets",
#     "m_runNominal"                : True,
#     #"m_eleSystematics"            : "ElectronSelector_Syst",
#     #"m_muonSystematics"           : "MuonSelector_Syst",
#     #"m_tauSystematics"            : "TauSelector_Syst",
#     #"m_jetSystematics"            : "JetSelector_Syst",
#     "m_doJVTCut"                  : True,
#     "m_dofJVTCut"                 : False,
#     "m_calculateSignificance"     : False,
#     "m_msgLevel"                  : "Info"
# }
#
# c.algorithm("METConstructor", MetConstructorDict )
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# SecondaryVertexSelectorDict = {
#     "m_name"                 : "SecVtxSel",
#     "m_mapInFile"            : "../deps/DVAnalysisBase/deps/FactoryTools/data/DV/MaterialMap_v3.2_Inner.root",
#     "m_mapOutFile"           : "../deps/DVAnalysisBase/deps/FactoryTools/data/DV/MaterialMap_v3_Outer.root",
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
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# Dict_VertexMatcher = {
#     "m_name"                           : "VertexMatch",
#     "m_msgLevel"                       : "Info",
#     "m_inSecondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices",   # --> use selected vertices
# }
# # Vertex Matching
# if args.is_MC:
#     c.algorithm ( "VertexMatcher",           Dict_VertexMatcher           )
#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# ##%%%%%%%%%%%%%%%%%%%%%%%%%% Hto4bLLPAlgo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# DHNLDict = {
#     "m_name"                    : "Hto4bLLPAlgo",
#     #----------------------- Container Flow ----------------------------#
#     "m_inJetContainerName"      : "SignalJets",
#     "m_inputAlgo"               : "SignalJets_Algo",
#     "m_allJetContainerName"     : "AntiKt4EMTopoJets_Calib",
#     "m_allJetInputAlgo"         : "AntiKt4EMTopoJets_Calib_Algo",
#     "m_inMuContainerName"       : "Muons_Signal",
#     "m_inElContainerName"       : "Electrons_Signal",
#     "m_inMETContainerName"      : "MET",
#     "m_inMETTrkContainerName"   : "METTrk",
#     #----------------------- Selections ----------------------------#
#     "m_leadingJetPtCut"         : 20,
#     "m_subleadingJetPtCut"      : 20,
#     "m_jetMultiplicity"         : 2,
#     "m_useMCPileupCheck"        : False,
#     "m_metCut"                  : 20000,
#     #----------------------- Other ----------------------------#
#     "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
#     "m_msgLevel"                : "Info",
# }
#
# c.algorithm("DHNL", DHNLDict )
#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLNtuple %%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLNtupleDict = {
    "m_name"                         : "DHNLNtuple",
    #----------------------- Container Flow ----------------------------#
    # "m_inJetContainerName"           : "AntiKt4EMTopoJets",
    # "m_inputAlgo"                    : "",#"SignalJets_Algo",
    # "m_allJetContainerName"          : "AntiKt4EMTopoJets",
    # "m_allJetInputAlgo"              : "",#"AntiKt4EMTopoJets_Calib_Algo",
    "m_inMuContainerName"            : "Muons",
    # "m_inElContainerName"            : "Electrons",
    # "m_inMETContainerName"           : "MET",
    # "m_inMETTrkContainerName"        : "METTrk",
    # "m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices", # --> use selected DVs
    # "m_secondaryVertexBranchName"    : "secVtx",
    # "m_truthVertexContainerName"     : "TruthVertices",
    # "m_truthVertexBranchName"        : "truthVtx",
    #----------------------- Output ----------------------------#
    # "m_eventDetailStr"               : "truth pileup", #shapeEM
    # "m_jetDetailStr"                 : "kinematic rapidity clean energy truth flavorTag trackAll trackPV allTrackPVSel allTrackDetail allTrackDetailPVSel btag_jettrk",
    # "m_jetDetailStrSyst"             : "kinematic rapidity energy clean flavorTag",
    # "m_elDetailStr"                  : "kinematic clean energy truth flavorTag isolation trackparams trackhitcont effSF PID", #trigger
    "m_muDetailStr"                  : "kinematic clean energy truth flavorTag isolation trackparams trackhitcont effSF quality energyLoss", #trigger
    # "m_trigDetailStr"                : "basic passTriggers",#basic menuKeys passTriggers",
    # "m_metDetailStr"                 : "metClus",
    # "m_metTrkDetailStr"              : "metTrk",
    # # "m_trackDetailStr"               : "fitpars",
    # "m_secondaryVertexDetailStr"     : "tracks truth jetMatched",
    # "m_truthVertexDetailStr"         : "isMatched",
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Debug",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

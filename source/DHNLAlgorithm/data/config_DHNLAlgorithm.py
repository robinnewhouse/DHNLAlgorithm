from xAODAnaHelpers import Config
import shlex
import argparse


def get_comma_separated_args(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(','))


parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isDerivation', dest='isDerivation', action="store_true", default=False)
parser.add_argument('--runAllSyst', dest='runAllSyst', action="store_true", default=False)
parser.add_argument('--noPRW', dest='noPRW', action="store_true", default=False)
parser.add_argument('--samplePeriod', dest='samplePeriod', default='',)
parser.add_argument('--DSID', dest='DSID', default='',)
o = parser.parse_args(shlex.split(args.extra_options))

sample_periods = o.samplePeriod.split(',')
if sample_periods == []:
    print("You didn't specify a data or MC period. Setting to 'all'. This will give incorrect results.")
    sample_periods = ['mc16a', 'mc16d', 'mc16e', 'data15', 'data16', 'data17', 'data18',]

c = Config()

# vertex container information (by default run VSI Leptons [VSI LeptonsMod for derivations])
if o.isDerivation:
    secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices_LeptonsMod_LRTR3_1p0"]
    secondaryVertexBranchNames = ["secVtx_VSI_LeptonsMod"]
    AugmentationVersionStrings = ["_LeptonsMod_LRTR3_1p0"]
    # VrtSecInclusive_SecondaryVertices_FixedExtroplator in SUSY15
else:
    secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices"]
    secondaryVertexBranchNames = ["secVtx_VSI"]
    AugmentationVersionStrings = [""]

# Good Run Lists
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/GoodRunListsForAnalysisRun2#Naming_scheme_and_documentation
GRLList = []
if 'data15' in sample_periods : GRLList.append('GoodRunsLists/data15_13TeV/20170619/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml')
if 'data16' in sample_periods : GRLList.append('GoodRunsLists/data16_13TeV/20180129/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml')
if 'data17' in sample_periods : GRLList.append('GoodRunsLists/data17_13TeV/20180619/data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')
if 'data18' in sample_periods : GRLList.append('GoodRunsLists/data18_13TeV/20190318/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')

# Pileup Reweighting
# The sample you're running over must have the PRW file available.
# If you are getting errors such as "Unrecognised channelNumber 311660 for periodNumber 300000" this is the place to start.
# option 1. use centrally produced CVMFS files located in dev/PileupReweighting/share/
# option 2. use local PRW files. You can download the NTUP_PILEUP version of your sample locally and load it into the PRW tool.
import DHNLAlgorithm.prw_files as prw_files
PRWList = []
if 'mc16a' in sample_periods: PRWList.extend(prw_files.prw_files_mc16a)
if 'mc16d' in sample_periods: PRWList.extend(prw_files.prw_files_mc16d)
if 'mc16e' in sample_periods: PRWList.extend(prw_files.prw_files_mc16e)
if 'mc20e' in sample_periods: PRWList.extend(prw_files.prw_files_mc20e)
# Filter PRW list by DSID if provided
try:
    tmp_prw_list = [prw_file for prw_file in PRWList if str(int(o.DSID)) in prw_file or 'actualMu' in prw_file]
    PRWList = tmp_prw_list
except:
    pass

# Lumicalc Files
# Must be careful about which MC campaign is specified in the run command.
# Note: if you want to use all lumicalc files without deactivating any,
# then the PRW files for ALL MC CAMPAIGNS must be loaded in the PRWList.
# If this isn't done the pileup reweighting tool will crash the algorithm.
lumicalcList = []
if 'mc16a' in sample_periods: 
    lumicalcList.extend(['GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root'])
    lumicalcList.extend(['GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root']) 
if 'mc16d' in sample_periods: lumicalcList.extend(['GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root',])
if 'mc16e' or 'mc20e' in sample_periods: lumicalcList.extend(['GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root',]) 

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
    "m_useMetaData"               : True,
    "m_storePassHLT"              : True,
    "m_storeTrigDecisions"        : True,
    "m_storePassL1"               : True,
    "m_storeTrigKeys"             : True,
    "m_applyTriggerCut"           : False, #not args.is_MC,
    "m_doPUreweighting"           : False if o.noPRW else True,
    "m_doPUreweightingSys"        : False if o.noPRW else True,
    "m_PRWFileNames"              : PRW,
    "m_lumiCalcFileNames"         : LUMICALCS,
    "m_autoconfigPRW"             : False,
    "m_triggerSelection"          : "HLT_e24_lhmedium_L1EM20VH || HLT_e26_lhtight_nod0_ivarloose || HLT_mu26_ivarmedium || HLT_mu20_iloose_L1MU15",
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
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLFilter %%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
# DHNLFilterDict = {
#     "m_name"                    : "DHNLFilter",
#     #----------------------- Container Flow ----------------------------#
#     "m_allJetContainerName"     : "AntiKt4EMTopoJets"if not o.isDerivation else "AntiKt4EMTopoJets_BTagging201810", # not used (vh4b only)
#     "m_inMuContainerName"       : "Muons",
#     "m_inElContainerName"       : "Electrons",
#     "m_vertexContainerName"     : "PrimaryVertices",
#     "m_applyFilterCut"          : False,
#     #----------------------- Selections ----------------------------#
#     # All selections are stored in default parameters in filter.
#     # they can still be modified here. e.g.:
#     # "m_AlphaMaxCut"             : 0.03,
#     "m_electronLHWP"              : "Medium" if not o.isDerivation else "DFCommonElectronsLHMedium", # not used (vh4b only)
#     "m_el1IDKey"                  :  "LHLoose", # if not o.isDerivation else "DFCommonElectronsLHLoose", # if you didnt add LHLoose to the SUSy15 config you need to update the electron quality
#     #----------------------- Other ----------------------------#
#     "m_msgLevel"                : "Info",
# }

# c.algorithm("DHNLFilter", DHNLFilterDict )


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
    "m_systVal"                   : 1.0,
    #----------------------- configurable cuts ----------------------------#
    "m_muonQualityStr"            : "VeryLoose",
    "m_pass_max"                  : -1,
    "m_pass_min"                  : -1,
    "m_pT_max"                    : 1e8,
    "m_pT_min"                    : 3.0,
    "m_eta_max"                   : 1e8,
    "m_d0_max"                    : 1e8,
    "m_d0sig_max"                 : 1e8,
    "m_z0sintheta_max"            : 1e8,
    #----------------------- isolation stuff ----------------------------#
    "m_MinIsoWPCut"               : "",
    "m_IsoWPList"                 : "FCLoose,FCTight",
    #----------------------- trigger matching stuff ----------------------------#
    "m_singleMuTrigChains"        : "HLT_mu20_iloose_L1MU15,HLT_mu26_ivarmedium",
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
  "m_inContainerName"           : "Muons_Calibrate",
  #----------------------- Systematics ----------------------------#
  "m_inputSystNamesMuons"       : "MuonSelector_Syst",
  "m_writeSystToMetadata"       : True,
  "m_systNameReco"              : "All",
  "m_systValReco"               : 1.0,
  "m_systNameIso"               : "All",
  "m_systValIso"                : 1.0,
  "m_systNameTrig"              : "All",
  "m_systValTrig"               : 1.0,
  "m_AllowZeroSF"               : True, # the code says to use with caution... why?
  "m_MuTrigLegs"                : "2015:HLT_mu20_iloose_L1MU15,2016:HLT_mu26_ivarmedium,2017:HLT_mu26_ivarmedium,2018:HLT_mu26_ivarmedium",
  #----------------------- Working Points ----------------------------#
  "m_overrideCalibRelease"      : "210222_Precision_r21",
  "m_WorkingPointReco"          : "Medium",
  "m_WorkingPointIso"           : "FCLoose",
  #----------------------- Other ----------------------------#
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
    #----------------------- PID -----------------------------------------#
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
    "m_IsoWPList"                 : "Loose,Tight",
    #----------------------- trigger matching stuff ----------------------------#
    # "m_singleElTrigChains"        : "HLT_e24_lhmedium_L1EM20VH,HLT_e26_lhtight_nod0_ivarloose",
    # "m_singleElTrigChains"        : "HLT_e24_lhmedium_L1EM20VH, HLT_e24_lhtight_nod0_ivarloose, HLT_e26_lhtight_nod0, HLT_e26_lhtight_nod0_ivarloose, HLT_e60_lhmedium_nod0, HLT_e140_lhloose_nod0",
    #----------------------- Other ----------------------------#
    # "m_IsoWPList"                 : "Gradient",
    "m_msgLevel"                  : "Info"
}
# Annoyingly, we must run the ElectronSelector algorithm in order to store quality parameters even in the input container.
c.algorithm("ElectronSelector", ElectronSelectorDict )


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#  
#%%%%%%%%%%%%%%%%%%%%%%% ElectronEfficiencyCorrector %%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#  
ElectronEfficiencyCorrectorDict = {
  "m_name"                      : "ElectronEfficiencyCorrector",
  #----------------------- Container Flow ----------------------------#
  "m_inContainerName"           : "Electrons_Calibrate",
  #----------------------- Systematics ----------------------------#
  "m_inputSystNamesElectrons"   : "ElectronSelector_Syst",
  "m_writeSystToMetadata"       : True,
  "m_systNamePID"               : "All",
  "m_systValPID"                : 1.0,
  "m_systNameTrig"              : "All",
  "m_systValTrig"               : 1.0,
  "m_systNameReco"              : "All",
  "m_systValReco"               : 1.0,
#  "m_systNameIso"               : "All",
#  "m_systValIso"                : 1.0,
  "m_correlationModel"          : "TOTAL",
  #----------------------- Working Points ----------------------------#
  "m_overrideMapFilePath"       : "$WorkDir_DIR/data/DHNLAlgorithm/electron_sf/electron_efficiency_map.txt",
  "m_WorkingPointReco"          : "Reconstruction",
  "m_WorkingPointPID"           : "LooseBLayer",
  "m_WorkingPointIso"           : "Gradient",
  "m_WorkingPointTrig"          : "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0",
  #----------------------- Other ----------------------------#
  "m_msgLevel"                  : "Info"
}

c.algorithm("ElectronEfficiencyCorrector", ElectronEfficiencyCorrectorDict ) 

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#  
#%%%%%%%%%%%%%%%%%%%%%%% ElectronEfficiencyCorrector %%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#  
ElectronEfficiencyCorrectorDict = {
  "m_name"                      : "ElectronEfficiencyCorrector",
  #----------------------- Container Flow ----------------------------#
  "m_inContainerName"           : "Electrons_Calibrate",
  #----------------------- Systematics ----------------------------#
  "m_inputSystNamesElectrons"   : "ElectronSelector_Syst",
  "m_writeSystToMetadata"       : True,
  "m_systNamePID"               : "All",
  "m_systValPID"                : 1.0,
  "m_systNameTrig"              : "All",
  "m_systValTrig"               : 1.0,
  "m_systNameReco"              : "All",
  "m_systValReco"               : 1.0,
#  "m_systNameIso"               : "All",
#  "m_systValIso"                : 1.0,
  "m_correlationModel"          : "TOTAL",
  #----------------------- Working Points ----------------------------#
  "m_overrideMapFilePath"       : "$WorkDir_DIR/data/DHNLAlgorithm/electron_sf/electron_efficiency_map.txt",
  "m_WorkingPointReco"          : "Reconstruction",
  "m_WorkingPointPID"           : "Medium",
  "m_WorkingPointIso"           : "Gradient",
  "m_WorkingPointTrig"          : "SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0",
  #----------------------- Other ----------------------------#
  "m_msgLevel"                  : "Info"
}

c.algorithm("ElectronEfficiencyCorrector", ElectronEfficiencyCorrectorDict ) 

for augstr in AugmentationVersionStrings: 

    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
    #%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
    SecondaryVertexSelectorDict = {
        "m_name"                 : "SecVtxSel_VSI"+ augstr,
        "m_mapInFile"            : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3.2_Inner.root",
        "m_mapOutFile"           : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3_Outer.root",
        "m_inContainerName"      : "VrtSecInclusive_SecondaryVertices" + augstr,
        "m_outContainerName"      : "VrtSecInclusive_SecondaryVertices" + augstr +"_sel",
        #---------------------- Selections ---------------------------#
        # "m_do_trackTrimming"     : False,
        "m_do_matMapVeto"        : True,
        # "prop_chi2Cut"           : 5.0,
        # "prop_d0_wrtSVCut"       : 0.8,
        # "prop_z0_wrtSVCut"       : 1.2,
        # "prop_errd0_wrtSVCut"    : 999999,
        # "prop_errz0_wrtSVCut"    : 999999,
        # "prop_d0signif_wrtSVCut" : 5.0,
        # "prop_z0signif_wrtSVCut" : 5.0,
        # "prop_chi2_toSVCut"      : 5.0,
        # "prop_vtx_suffix"        : "",
        #------------------------ Other ------------------------------#
        "m_msgLevel"             : "Info",
    }
    c.algorithm ( "SecondaryVertexSelector", SecondaryVertexSelectorDict )
    # Annoyingly, we must run the SecondaryVertex Selector algorithm in order get the material map decorations on the vertices

    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
    #%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
    #%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
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
    "m_inMuContainerName"       : "Muons_Calibrate",
    "m_inElContainerName"       : "Electrons_Calibrate",
    "m_muInputAlgo"             : "MuonSelector_Syst" if o.runAllSyst else "",
    "m_elInputAlgo"             : "ElectronSelector_Syst" if o.runAllSyst else "",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_AugmentationVersionStringList"    : ','.join(AugmentationVersionStrings),
    "m_inDetTrackParticlesContainerName" : "InDetTrackParticles",
    #----------------------- Selections ----------------------------#
    "m_doInverseLeptonControlRegion"     : False,
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
    "m_inMuContainerName"            : "Muons_Calibrate",
    "m_inElContainerName"            : "Electrons_Calibrate",
    "m_inputAlgos"                   : "MuonSelector_Syst,ElectronSelector_Syst," if o.runAllSyst else "",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_secondaryVertexBranchNameList" : ','.join(secondaryVertexBranchNames),
    "m_AugmentationVersionStringList" : ','.join(AugmentationVersionStrings),
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering 
    "m_truthVertexContainerName"     : "SelectedTruthVertices",
    "m_truthVertexBranchName"        : "truthVtx",
    "m_inTruthParticleContainerName" : "MuonTruthParticles",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth pileup pileupsys", #shapeEM
    "m_muDetailStr"                  : "kinematic clean energy truth flavorTag trackparams energyLoss \
                                        effSF \
                                        quality RECO_Tight RECO_Medium RECO_Loose \
                                        isolation ISOL_FCLoose \
                                        trigger TRIG_HLT_mu26_ivarmedium TRIG_HLT_mu20_iloose_L1MU15 \
                                        ",
    "m_elDetailStr"                  : "kinematic clean energy truth flavorTag trackparams \
                                        PID PID_LHLooseBL PID_LHLoose PID_LHMedium PID_LHTight \
                                        effSF \
                                        PIDSF_LooseBLayer PIDSF_Medium \
                                        isolation ISOL_Gradient \
                                        trigger TRIG_SINGLE_E_2015_e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose_2016_2018_e26_lhtight_nod0_ivarloose_OR_e60_lhmedium_nod0_OR_e140_lhloose_nod0 \
                                        ",
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

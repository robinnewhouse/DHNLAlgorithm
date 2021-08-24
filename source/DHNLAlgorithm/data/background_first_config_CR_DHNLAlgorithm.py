from xAODAnaHelpers import Config
import os
import shlex
import argparse



def get_comma_separated_args(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(','))


parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isDerivation', dest='isDerivation', action="store_true", default=False)
parser.add_argument('--runAllSyst', dest='runAllSyst', action="store_true", default=False)
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
GRLList = []
if 'data15' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/data15_13TeV.periodAllYear_DetStatus-v89-pro21-02_Unknown_PHYS_StandardGRL_All_Good_25ns.xml')
if 'data16' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml')
if 'data17' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/data17_13TeV.periodAllYear_DetStatus-v99-pro22-01_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')
if 'data18' in sample_periods : GRLList.append('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml')

# Pileup Reweighting
# The sample you're running over must have the PRW file available.
# If you are getting errors such as "Unrecognised channelNumber 311660 for periodNumber 300000" this is the place to start.
# option 1. use centrally produced CVMFS files located in /cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/share/
# option 2. use local PRW files. You can download the NTUP_PILEUP version of your sample locally and load it into the PRW tool.
import DHNLAlgorithm.prw_files as prw_files
PRWList = []
if 'mc16a' in sample_periods: PRWList.extend(prw_files.prw_files_mc16a)
if 'mc16d' in sample_periods: PRWList.extend(prw_files.prw_files_mc16d)
if 'mc16e' in sample_periods: PRWList.extend(prw_files.prw_files_mc16e)

# Lumicalc Files
# Must be careful about which MC campaign is specified in the run command.
# Note: if you want to use all lumicalc files without deactivating any,
# then the PRW files for ALL MC CAMPAIGNS must be loaded in the PRWList.
# If this isn't done the pileup reweighting tool will crash the algorithm.
lumicalcList = []
if 'mc16a' in sample_periods:
    lumicalcList.extend(['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root'])
    lumicalcList.extend(['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root'])
if 'mc16d' in sample_periods: lumicalcList.extend(['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root',])
if 'mc16e' in sample_periods: lumicalcList.extend(['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root',])

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


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DHNLFilter %%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
DHNLFilterDict = {
    "m_name"                    : "DHNLFilter",
    #----------------------- Container Flow ----------------------------#
    "m_allJetContainerName"     : "AntiKt4EMTopoJets" if not o.isDerivation else "AntiKt4EMTopoJets_BTagging201810", # not used (vh4b only)
    "m_inMuContainerName"       : "Muons",
    "m_inElContainerName"       : "Electrons",
    "m_vertexContainerName"     : "PrimaryVertices",
    "m_applyFilterCut"          : False,
    #----------------------- Selections ----------------------------#
    # All selections are stored in default parameters in filter.
    # they can still be modified here. e.g.:
    # "m_AlphaMaxCut"             : 0.03,
    "m_electronLHWP"              : "Medium" if not o.isDerivation else "DFCommonElectronsLHMedium", # not used (vh4b only)
    "m_el1IDKey"                  :  "LHLoose", # if not o.isDerivation else "DFCommonElectronsLHLoose", # if you didnt add LHLoose to the SUSy15 config you need to update the electron quality
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
    "m_IsoWPList"                 : "FCLoose,FCTight" if o.isDerivation else "FixedCutHighPtTrackOnly",
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
    "m_IsoWPList"                 : "FCLoose,FCTight" if o.isDerivation else "Gradient",
    #----------------------- trigger matching stuff ----------------------------#
    "m_singleElTrigChains"        : "HLT_e24_lhmedium_L1EM20VH,HLT_e26_lhtight_nod0_ivarloose",
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
    "m_inMuContainerName"       : "Muons_Calibrate",
    "m_inElContainerName"       : "Electrons_Calibrate",
    "m_muInputAlgo"             : "MuonSelector_Syst" if o.runAllSyst else "",
    "m_elInputAlgo"             : "ElectronSelector_Syst" if o.runAllSyst else "",
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_AugmentationVersionStringList"    : ','.join(AugmentationVersionStrings),
    "m_inDetTrackParticlesContainerName" : "InDetTrackParticles_Selected",
    #----------------------- Selections ----------------------------#
    "m_doInverseLeptonControlRegion"   : True, # apply inverse prompt lepton cut!
    "m_backgroundEstimationBranches"   : True,
    "m_doPromptLeptonCut"   : False,
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
    "m_trackParticleContainerName"   : "InDetTrackParticles_Selected",
    "m_inputAlgos"                   : "",
    "m_secondaryVertexContainerNameList" : "",
    "m_secondaryVertexBranchNameList" : "",
    "m_AugmentationVersionStringList" : "",
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering
    "m_truthVertexContainerName"     : "",
    "m_truthVertexBranchName"        : "",
    "m_inTruthParticleContainerName" : "",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth pileup pileupsys", #shapeEM
    "m_muDetailStr"                  : "",
    "m_elDetailStr"                  : "",
    "m_trigDetailStr"                : "basic passTriggers",#basic menuKeys passTriggers",
    "m_secondaryVertexDetailStr"     : "", # "tracks" linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_vertexDetailStr"              : "",
    "m_truthVertexDetailStr"         : "", # Uses pt-matching to match reconstructed vertices.
    "m_trackDetailStr"               : "numbers fitpars vertex",
    #----------------------- Other ----------------------------#
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

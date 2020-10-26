from xAODAnaHelpers import Config
import os
import shlex
import argparse



parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isSUSY15', dest='isSUSY15', action="store_true", default=False)
parser.add_argument('--altVSIstr', dest='altVSIstr', type=str, default="_Leptons")

o = parser.parse_args(shlex.split(args.extra_options))

c = Config()

# vertex container information (by default run VSI & VSI Leptons)
secondaryVertexContainerNames = ["VrtSecInclusive_SecondaryVertices_2","VrtSecInclusive_SecondaryVertices_Leptons2"]
secondaryVertexBranchNames = ["secVtx_VSI", "secVtx_VSI_Leptons"]
AugmentationVersionStrings = ["_2","_Leptons2"]



#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
SecondaryVertexSelectorDict = {
    "m_name"                 : "SecVtxSel",
    "m_mapInFile"            : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3.2_Inner.root",
    "m_mapOutFile"           : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3_Outer.root",
    "m_inContainerName"      : "VrtSecInclusive_SecondaryVertices" + o.altVSIstr+ "2",
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
    "prop_vtx_suffix"        : o.altVSIstr+ "2",
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
        #------------------------ Lepton Matching ------------------------------#
        "m_doLeptons"                       : True,
        "m_inMuContainerName"               : "Muons",
        "m_inElContainerName"               : "Electrons",
        "m_VSILepmatch"                    : True if "Leptons" in augstr else False,
        #------------------------ Other ------------------------------#
        "m_msgLevel"             : "Info",
    }
    c.algorithm ( "VertexMatcher",           Dict_VertexMatcher   )

if o.altVSIstr != "None":
    Dict_VertexMatcher_Alt = {
        "m_name"                            : "VertexMatch"+o.altVSIstr ,
        "m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices" + o.altVSIstr ,
        #------------------------ Lepton Matching ------------------------------#
        "m_doLeptons"                       : True,
        "m_inMuContainerName"               : "Muons",
        "m_inElContainerName"               : "Electrons",
        "m_VSILepmatch"                     : True if "Leptons" in o.altVSIstr else False, # careful here since if altVSIstr doesnt include Leptons but VSI algorithm was run with selectMuons or selectElectrons this wont run properly
        #------------------------ Other ------------------------------#
        "m_msgLevel"             : "Info",
    }
    c.algorithm ( "VertexMatcher",           Dict_VertexMatcher_Alt           )


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
    "m_inJetContainerName"      : "",
    "m_inputAlgo"               : "",
    "m_allJetContainerName"     : "",
    "m_allJetInputAlgo"         : "",
    "m_inDetTrackParticlesContainerName": "TracksForVSI",
    "m_inMuContainerName"       : "",
    "m_inElContainerName"       : "",
    "m_inMETContainerName"      : "",
    "m_inMETTrkContainerName"   : "",
    #----------------------- Selections ----------------------------#
    "m_leadingJetPtCut"         : 20,
    "m_subleadingJetPtCut"      : 20,
    "m_jetMultiplicity"         : 2,
    "m_useMCPileupCheck"        : False,
    "m_metCut"                  : 20000,
    #----------------------- Other ----------------------------#
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
    "m_backgroundEstimationNoParticleData": True,
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
    "m_secondaryVertexContainerNameList" : ','.join(secondaryVertexContainerNames),
    "m_secondaryVertexBranchNameList" : ','.join(secondaryVertexBranchNames),
    "m_AugmentationVersionStringList" : ','.join(AugmentationVersionStrings),
    "m_secondaryVertexContainerNameAlt" : "VrtSecInclusive_SecondaryVertices" + o.altVSIstr,
    "m_secondaryVertexBranchNameAlt" : "secVtx_VSI" + o.altVSIstr,
    "m_AltAugmentationVersionString" : o.altVSIstr, # augumentation for alternate vertex container
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering
    "m_truthVertexContainerName"     : "TruthVertices",
    "m_truthVertexBranchName"        : "truthVtx",
    "m_inTruthParticleContainerName" : "MuonTruthParticles",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "truth pileup", #shapeEM
    "m_elDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation trackparams PID PID_Loose PID_Medium PID_Tight PID_LHLoose PID_LHMedium PID_LHTight PID_MultiLepton",
    "m_muDetailStr"                  : "kinematic clean energy truth flavorTag trigger isolation trackparams quality RECO_Tight RECO_Medium RECO_Loose energyLoss",
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

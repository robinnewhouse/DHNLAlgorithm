from xAODAnaHelpers import Config
import os
import shlex
import argparse



parser = argparse.ArgumentParser(description='Test for extra options')
parser.add_argument('--isSUSY15', dest='isSUSY15', action="store_true", default=False)
parser.add_argument('--altVSIstr', dest='altVSIstr', type=str, default="_Leptons")

o = parser.parse_args(shlex.split(args.extra_options))

c = Config()


#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%% Secondary Vertex Selection %%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
SecondaryVertexSelectorDict = {
    "m_name"                 : "SecVtxSel",
    "m_mapInFile"            : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3.2_Inner.root",
    "m_mapOutFile"           : "$WorkDir_DIR/data/FactoryTools/DV/MaterialMap_v3_Outer.root",
    "m_inContainerName"      : "VrtSecInclusive_SecondaryVertices" + o.altVSIstr+ "_2",
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
    "prop_vtx_suffix"        : o.altVSIstr+ "_2",
    #------------------------ Other ------------------------------#
    "m_msgLevel"             : "Info",
}

c.algorithm ( "SecondaryVertexSelector", SecondaryVertexSelectorDict )

#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%% Vertex Matching %%%%%%%%%%%%%%%%%%%%%%%%%%%%#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#
Dict_VertexMatcher = {
    "m_name"                            : "VertexMatch"+ "_2",
    "m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices"+ "_2",   # --> use selected vertices
    #------------------------ Lepton Matching ------------------------------#
    "m_doLeptons"                       : True,
    "m_inMuContainerName"               : "Muons",
    "m_inElContainerName"               : "Electrons",
     "m_VSILepmatch"                    : False,
    #------------------------ Other ------------------------------#
    "m_msgLevel"             : "Info",
}
c.algorithm ( "VertexMatcher",           Dict_VertexMatcher           )

Dict_VertexMatcher_Leptons = {
"m_name"                            : "VertexMatch"+o.altVSIstr+ "_2",
"m_inSecondaryVertexContainerName"  : "VrtSecInclusive_SecondaryVertices" + o.altVSIstr+ "_2",
#------------------------ Lepton Matching ------------------------------#
"m_doLeptons"                       : True,
"m_inMuContainerName"               : "Muons",
"m_inElContainerName"               : "Electrons",
"m_VSILepmatch"                     : True if "Leptons" in o.altVSIstr else False,
#------------------------ Other ------------------------------#
"m_msgLevel"             : "Info",
}
c.algorithm ( "VertexMatcher",           Dict_VertexMatcher_Leptons           )

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
    "m_inJetContainerName"      : "SignalJets",
    "m_inputAlgo"               : "SignalJets_Algo",
    "m_allJetContainerName"     : "AntiKt4EMTopoJets_Calib",
    "m_allJetInputAlgo"         : "AntiKt4EMTopoJets_Calib_Algo",
    "m_inMuContainerName"       : "Muons_Calibrate",
    "m_inElContainerName"       : "Electrons_Calibrate",
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
    "m_backgroundEstimationBranches": True,
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
    # "m_inJetContainerName"           : "SignalJets"if not o.isSUSY15 else "AntiKt4EMTopoJets_BTagging201810",
    # "m_allJetContainerName"          : "SignalJets" if not o.isSUSY15 else "AntiKt4EMTopoJets_BTagging201810" ,
    "m_inJetContainerName"           :  "",
    "m_allJetContainerName"          :  "",
    "m_jetInputAlgo"                 : "",
    "m_allJetInputAlgo"              : "",
    "m_inMuContainerName"            : "",
    "m_inElContainerName"            : "",
    "m_trackParticleContainerName"   : "",
    "m_inMETContainerName"           : "",
    "m_inMETTrkContainerName"        : "",
    "m_secondaryVertexContainerName" : "VrtSecInclusive_SecondaryVertices" + "_2", # --> use selected DVs
    "m_secondaryVertexContainerNameAlt" : "VrtSecInclusive_SecondaryVertices" + o.altVSIstr + "_2", # --> use selected DVs
    "m_secondaryVertexBranchName"    : "secVtx_VSI" + "_2",
    "m_secondaryVertexBranchNameAlt" : "secVtx_VSI" + o.altVSIstr + "_2",
    "m_AltAugmentationVersionString" : o.altVSIstr + "_2", # no augumentation for standard VSI
    "m_suppressTrackFilter"          : True, # supress VSI bonsi track filtering 
    "m_truthVertexContainerName"     : "",
    "m_truthVertexBranchName"        : "",
    "m_inTruthParticleContainerName" : "",
    #----------------------- Output ----------------------------#
    "m_eventDetailStr"               : "", #shapeEM
    "m_jetDetailStr"                 : "",
    "m_jetDetailStrSyst"             : "",
    "m_elDetailStr"                  : "",
    "m_muDetailStr"                  : "",
    "m_trigDetailStr"                : "",#basic menuKeys passTriggers",
    "m_metDetailStr"                 : "",
    "m_metTrkDetailStr"              : "",
    "m_trackDetailStr"               : "",
    "m_secondaryVertexDetailStr"     : "tracks truth leptons", # "linked": pt-matched truth vertices. "close": distance matched truth vertices.
    "m_vertexDetailStr"              : "",
    "m_truthVertexDetailStr"         : "", # Uses pt-matching to match reconstructed vertices.
    #----------------------- Other ----------------------------#
    "m_useCutFlow"              : False,
    "m_useMCPileupCheck"        : False,
    "m_MCPileupCheckContainer"  : "AntiKt4TruthJets",
    "m_msgLevel"                : "Info",
}

c.algorithm("DHNLNtuple", DHNLNtupleDict )

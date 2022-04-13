# The name of the package:
atlas_subdir (DHNLAlgorithm)

# Declare the package's dependencies:
#atlas_depends_on_subdirs( PUBLIC
#        Control/AthToolSupport/AsgTools
#        Control/xAODRootAccess
#        Event/xAOD/xAODEventInfo
#        Event/xAOD/xAODJet
#        Event/xAOD/xAODMissingET
#        PhysicsAnalysis/D3PDTools/EventLoop
#        PhysicsAnalysis/D3PDTools/RootCoreUtils
#        xAODAnaHelpers
#        DVAnalysisBase
#        PRIVATE
#        Event/xAOD/xAODEgamma
#        Event/xAOD/xAODMuon )


# external dependencies
find_package( ROOT COMPONENTS Core RIO Hist Tree )

# build a dictionary for the library
atlas_add_root_dictionary ( DHNLAlgorithmLib DHNLAlgorithmDictSource
        ROOT_HEADERS DHNLAlgorithm/*.h Root/LinkDef.h
        EXTERNAL_PACKAGES ROOT )

# Component(s) in the package:
atlas_add_library( DHNLAlgorithmLib DHNLAlgorithm/*.h Root/*.h Root/*.cxx ${DHNLAlgorithmDictSource}
        PUBLIC_HEADERS DHNLAlgorithm
        INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
        LINK_LIBRARIES ${ROOT_LIBRARIES}
        AsgTools
        EventLoop
        xAODAnaHelpersLib
        DVAnalysisBaseLib
        xAODEgamma
        xAODEventInfo
        xAODJet
        xAODMissingET
        xAODMuon
        xAODRootAccess 
        PMGAnalysisInterfacesLib
        PMGToolsLib
        xAODBTagging)

# Install files from the package:
atlas_install_scripts( scripts/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_data( data/* scripts/sampleLists/*)

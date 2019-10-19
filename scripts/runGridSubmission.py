#!/usr/bin/env python

##################################################
# Grid submission script updated from 
# previous version written by Jeff.Dandoy@cern.ch.
# For questions on the current version, contact:
#  bingxuan.liu@cern.ch
#################################################

import os, math, sys
import Samples as sampleMaster
from gridConfigOptions import *

if not test:
  if not os.path.exists(sudDir):
    print "Directory " + subDir + " does not exist, please check! Aborting!"
    sys.exit(0)

for configName in configFiles:
  configFile = configFiles[configName]
  outputTag  = '%in:name[1]%.%in:name[2]%.%in:name[3]%.' + configName
  for runSample in runSamples:
    if not sampleMaster.Samples.has_key(runSample):
      continue
    typeString = ""
    if "MC" in sampleMaster.Types[runSample]:
      typeString = " --isMC "
      for sampleNickName in sampleMaster.Samples[runSample]:
        datasetName = sampleMaster.DatasetNames[sampleNickName]
        submitDirectory = sudDir + "/" + sampleNickName + "_" +configName  
        driverCommand = ''
        if runType == 'grid':
          driverCommand = 'prun --optGridDestSE=BNL-OSG2_USERDISK --optBatchShellInit \'source $WorkDir_DIR/setup.sh\' --optGridOutputSampleName='
          if production_name != "":
            driverCommand = ' prun --optSubmitFlags="--official" --optGridOutputSampleName='
            driverCommand += 'group.'+production_name
          else:
            driverCommand += 'user.' + userName
            driverCommand += "." + sampleNickName + "." + outputTag
        elif runType == 'local':
            driverCommand = ' direct'
  
        ## create and submit command ##
        command = 'xAH_run.py'
        if runType == 'grid':
          command += ' --inputRucio'
        command += ' --files ' + datasetName + typeString
        command += ' --config '+ configFile
        command += ' --force --submitDir '+ submitDirectory
        command += ' '+driverCommand
  
        print command
        if not test:
          os.system(command)
    else:
      for sampleNickName in sampleMaster.Samples[runSample]:
        command = 'xAH_run.py'
        if runType == 'grid':
          command += ' --inputRucio '
        datasetNames = sampleMaster.DatasetNames[sampleNickName] 
        if len(datasetNames) > 1:
          os.system("touch " + "inputList_" + sampleNickName + ".txt")
          inputList = open("inputList_" + sampleNickName + ".txt", "r+w")    
          for datasetName in datasetNames:
            inputList.write(datasetName + '\n')
          command += '--inputList --files ' +  "inputList_" + sampleNickName + ".txt" + typeString
        else:
          datasetName = datasetNames[0]
          command += ' --files ' + datasetName + typeString
        submitDirectory = sudDir + "/" + sampleNickName + "_" +configName
        driverCommand = ''
        if runType == 'grid':
          driverCommand = 'prun --optGridDestSE=BNL-OSG2_USERDISK --optBatchShellInit \'source $WorkDir_DIR/setup.sh\' --optGridOutputSampleName='
          if production_name != "":
            driverCommand = ' prun --optSubmitFlags="--official" --optGridOutputSampleName='
            driverCommand += 'group.'+production_name
          else:
            driverCommand += 'user.' + userName
            driverCommand += "." + sampleNickName + "." + outputTag
        elif runType == 'local':
            driverCommand = ' direct'
        command += ' --config '+ configFile
        command += ' --force --submitDir '+ submitDirectory
        command += ' '+driverCommand
        ## create and submit command ##
        print command
        if not test:
          os.system(command)

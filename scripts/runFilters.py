#!/usr/bin/python

#############################################
# A tool to group files into periods
# It prints out blocks for each periods that
# can be directly copied to the sample 
# dictionaries in Samples.py
# It also checks local data integrity
# command:
#    python runFilters.py filelist 
# bingxuan.liu@cern.ch
#############################################

import os 
import sys

runDic = {
  "A" : [324320, 325558],
  "B" : [325713, 328393],
  "C" : [329385, 330470],
  "D" : [330857, 332304],
  "E" : [332720, 334779],
  "F" : [334842, 335290],
  "H" : [336497, 336782],
  "I" : [336832, 337833],
  "K" : [338183, 340453]
}

runLists = {
  "A" : [],
  "B" : [],
  "C" : [],
  "D" : [],
  "E" : [],
  "F" : [],
  "H" : [],
  "I" : [],
  "K" : [],
}  

inputDataList = open(str(sys.argv[1]),"r")
for line in inputDataList:
  runNumber = int(line.split('.')[1])
  for runPeriod in runDic:
    if runNumber <= runDic[runPeriod][1] and runNumber >= runDic[runPeriod][0]:
      runLists[runPeriod].append(line.rstrip('\n'))
for runPeriod in runDic:
  print '"' + runPeriod + '" : ' 
  print runLists[runPeriod]

localDataDir = "/atlasfs/atlas/local/bingxuan.liu/FourJetNtuple/2017Data/"

periodToCheck = ["B", "C", "D", "E", "F", "H", "I"] 

print "Checking data integrity in local disk: " + localDataDir + "\n"

if os.path.exists("missingFiles.txt"):
  os.system("rm missingFiles.txt")
os.system("touch missingFiles.txt")
missingFile = open("missingFiles.txt","r+w")
for period in periodToCheck:
  print "Checking data integrity in period " + period + ": "
  for run in runLists[period]:
    runNumber = int(run.split('.')[1]) 
    files = os.popen("ls " + localDataDir + period + "/" + "*" + str(runNumber) + "*").readlines()
    if not len(files):
      print "Run " + str(runNumber) + " is missing!!!!"
      print "The dataset is " + run + "!"   
      missingFile.write(run + "\n")

runListLine = os.popen("grep 'RunList' ../data/data17_13TeV.periodAllYear_DetStatus-v97-pro21-13_Unknown_PHYS_StandardGRL_All_Good_25ns_JetHLT_Normal2017.xml").readlines()
runLists = runListLine[0].split(">")[1].split("<")[0].split(",")
for run in runLists:
  files = os.popen("ls " + localDataDir + "*/*" + str(run) + "*").readlines()
  if not len(files):
    print "Run " + str(run) + " is missing!!!"
    os.system("rucio ls data17_13TeV.00" + str(run) + ".physics_Main.deriv.DAOD_EXOT2.**")

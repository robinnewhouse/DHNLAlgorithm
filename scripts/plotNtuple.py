#!/usr/bin/env python

import os, sys
from array import array
import time
import argparse
import copy
from math import sqrt, log, isnan, isinf

#put argparse before ROOT call.  This allows for argparse help options to be printed properly (otherwise pyroot hijacks --help) and allows -b option to be forwarded to pyroot
parser = argparse.ArgumentParser(description="%prog [options]", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("-b", dest='batchMode', action='store_true', default=False, help="Batch mode for PyRoot")
parser.add_argument("--f_plotAll", dest='f_plotAll', action='store_true', default=False, help="Plot all tree entries.  This will not plot vector branches")
parser.add_argument("--file", dest='file', default="../../submitDir/data-tree/AOD.01598030._000086.pool.root.1.root",
         help="Input file name for fitting functions")
parser.add_argument("--outDir", dest='outDir', default="../../submitDir/data-tree/plots/",
         help="Name of output directory for plots")
args = parser.parse_args()

from ROOT import *
import AtlasStyle


def main():
#  outDir = args.file[:-5]+'/'
  if not os.path.exists(args.outDir):
    os.mkdir(args.outDir)
  AtlasStyle.SetAtlasStyle()

  global inTree, histList, plotList, userHist1D, userHist2D
  histList = []
  plotList = []
  userHist1D = []
  userHist2D = []

  ####### Define user histograms here########
  userHist2D.append( ["mjj_yStar", 100, 0., 6000000., 100, 0., 4., "GeV", "#eta" ] )


  inFile = TFile.Open(args.file, "READ")
  inTree = inFile.Get("outTree")

  args.f_plotAll = True

  #To loop through tree
  #for event in dataTree:
  #  charge.append(event.charge)

  defineHists()
  fillHists()
  saveHists()
  raw_input("wait...")

#########Define histograms######################
def defineHists( ):
  global inTree, histList, plotList, userHist1D, userHist2D


  if args.f_plotAll:
    plotList = [key.GetName() for key in inTree.GetListOfBranches()]

    print "Will print the following branches: ", plotList

    plotList = [thisPlot for thisPlot in plotList if not "vector" in inTree.GetLeaf(thisPlot).GetTypeName() ]
    print plotList

    for iPlot, plotName in enumerate(plotList):
      histList.append(TH1F( "h_"+plotName, "h_"+plotName, 50, inTree.GetMinimum(plotName), inTree.GetMaximum(plotName) ))
      print inTree.GetMinimum(plotList[iPlot]), inTree.GetMaximum(plotList[iPlot])


  #Add user defined histograms
  for iPlot, plotInfo in enumerate(userHist1D):
    plotList.append(plotInfo[0])
    histList.append(TH1F( "h_"+plotInfo[0], "h_"+plotInfo[0], plotInfo[1], plotInfo[2], plotInfo[3] ))
    if len(plotInfo) > 4:
      histList[-1].GetXaxis().SetTitle(plotList[-1]+" ("+plotInfo[4]+")" )

  for iPlot, plotInfo in enumerate(userHist2D):
    plotList.append(plotInfo[0])
    histList.append(TH2F( "h_"+plotInfo[0], "h_"+plotInfo[0], plotInfo[1], plotInfo[2], plotInfo[3], plotInfo[4], plotInfo[5], plotInfo[6] ))
    splitName = plotList[-1].split('_')
    if len(plotInfo) > 7:
      histList[-1].GetXaxis().SetTitle(splitName[0]+" ("+plotInfo[7]+")" )
    if len(plotInfo) > 8:
      histList[-1].GetYaxis().SetTitle(splitName[1]+" ("+plotInfo[8]+")" )

  return


###########Direct Filling of histograms################
def fillHists( ):
  global inTree, histList, plotList, userHist1D, userHist2D

  #if args.f_plotAll:
  #  print "doing this", len(plotList)
  for iPlot, plotName in enumerate(plotList):
    if( plotName.count('_') == 0):
      inTree.Draw(plotName+" >> h_"+plotName)
    elif( plotName.count('_') == 1):
      splitName = plotName.split('_')
      print plotName, splitName
      print "draw string is ", splitName[0]+";"+splitName[1]+" >> h_"+plotName
      inTree.Draw(splitName[1]+":"+splitName[0]+" >> h_"+plotName)


  return

#############Saving of Histograms########################
def saveHists( ):
  c1 = TCanvas()

  global inTree, histList, plotList, userHist1D, userHist2D

  for iPlot, plotName in enumerate(plotList):
    if(plotName.count('_') == 0):
      histList[iPlot].Draw()
    elif(plotName.count('_') == 1):
      histList[iPlot].Draw("COLZ")

    c1.Print(args.outDir+histList[iPlot].GetName()+".png" )

  return


if __name__ == "__main__":
    main()

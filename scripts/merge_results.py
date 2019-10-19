import argparse
import os
import subprocess
import fnmatch

from MyGenUtil import *

parser = argparse.ArgumentParser(description='This script merges the results of a CxAOD Reader submission for VHqqbb' , add_help = True)
parser.add_argument('-i' , '--input-dir' , type=str                     , help='A directory containing mc16a/mc16d/mc16e/data submission directories')
parser.add_argument('--hists-only'     , help='only merge histograms' , action='store_true')

args = parser.parse_args()

# fetch directory contains duplicates of everything (quirk of CxAOD framework)
# WARNING: No longer needed with current setup (sept 2019)
# for fetch_dir in find_directories(args.input_dir, "*fetch*"):
#     print("deleting fetch directory:", fetch_dir)
#     subprocess.call(["rm", "-rf", fetch_dir])

# ntuples
if not args.hists_only:
    my_hadd(args.input_dir + "/merged_output.root"    , args.input_dir , "*/data-tree/input.root"        , match_full_path = True)


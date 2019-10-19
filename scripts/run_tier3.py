import os
import sys
import random

from MyGenUtil import find_files, chunks

BASE_OUTPUT_DIR = os.getcwd()

NFILES_PER_MC = 1
NFILES_PER_DATA = 1

base_path = "/gpfs3/umass/jburzyns/VH4b/xAODs/"
DAOD_FILE_DIRS = {}
#DAOD_FILE_DIRS["dijet"] = "/gpfs3/umass/jburzyns/VH4b/xAODs/mc16_13TeV.361024.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ4W.merge.DAOD_RPVLL.e3668_s3126_r10847_r10706"


DAOD_FILE_DIRS["ttbar"] = base_path + "mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.merge.DAOD_RPVLL.e6337_e5984_s3126_r10847_r10706"
#DAOD_FILE_DIRS["WpH_H125_a15a15_4b_ctau1"] = base_path + "mc16_13TeV.309851.PowhegPythia8EvtGen_WpH_H125_a15a15_4b_ctau1.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WpH_H125_a55a55_4b_ctau1"] = base_path + "mc16_13TeV.309852.PowhegPythia8EvtGen_WpH_H125_a55a55_4b_ctau1.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WpH_H125_a15a15_4b_ctau10"] = base_path + "mc16_13TeV.309853.PowhegPythia8EvtGen_WpH_H125_a15a15_4b_ctau10.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WpH_H125_a55a55_4b_ctau10"] = base_path + "mc16_13TeV.309854.PowhegPythia8EvtGen_WpH_H125_a55a55_4b_ctau10.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WpH_H125_a15a15_4b_ctau100"] = base_path + "mc16_13TeV.309855.PowhegPythia8EvtGen_WpH_H125_a15a15_4b_ctau100.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WpH_H125_a55a55_4b_ctau100"] = base_path + "mc16_13TeV.309856.PowhegPythia8EvtGen_WpH_H125_a55a55_4b_ctau100.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WmH_H125_a15a15_4b_ctau1"] = base_path + "mc16_13TeV.309857.PowhegPythia8EvtGen_WmH_H125_a15a15_4b_ctau1.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WmH_H125_a55a55_4b_ctau1"] = base_path + "mc16_13TeV.309858.PowhegPythia8EvtGen_WmH_H125_a55a55_4b_ctau1.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WmH_H125_a15a15_4b_ctau10"] = base_path + "mc16_13TeV.309859.PowhegPythia8EvtGen_WmH_H125_a15a15_4b_ctau10.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WmH_H125_a55a55_4b_ctau10"] = base_path + "mc16_13TeV.309860.PowhegPythia8EvtGen_WmH_H125_a55a55_4b_ctau10.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WmH_H125_a15a15_4b_ctau100"] = base_path + "mc16_13TeV.309861.PowhegPythia8EvtGen_WmH_H125_a15a15_4b_ctau100.recon.DAOD_RPVLL.e6753_a875_r10739"
#DAOD_FILE_DIRS["WmH_H125_a55a55_4b_ctau100"] = base_path + "mc16_13TeV.309862.PowhegPythia8EvtGen_WmH_H125_a55a55_4b_ctau100.recon.DAOD_RPVLL.e6753_a875_r10739"


NODE_LIST = [
        "abc-at11",
        "abc-at12",
        "abc-at13",
        "abc-at14",
        "abc-um1",
        "abc-um2",
        "abc-um3",
        ]

for category, SAMPLE_DIR in DAOD_FILE_DIRS.iteritems():
    CATEGORY_OUTPUT_FOLDER_PATH = os.path.join(BASE_OUTPUT_DIR, category)
    os.mkdir(CATEGORY_OUTPUT_FOLDER_PATH)
    JOB_SCRIPT_DIR = os.path.join(CATEGORY_OUTPUT_FOLDER_PATH, "job_scripts")
    os.mkdir(JOB_SCRIPT_DIR)

    job_iter_num = 0
    SAMPLE_DIR = SAMPLE_DIR.strip()
    IS_MC = "mc16_13TeV" in SAMPLE_DIR
    ALL_SAMPLE_FILES = find_files(SAMPLE_DIR, "*.root*",True)

    sample_iter_num = 0
    for input_file_subset in chunks(ALL_SAMPLE_FILES, NFILES_PER_MC if IS_MC else NFILES_PER_DATA):
        OUTPUT_FOLDER_NAME = os.path.basename(SAMPLE_DIR)
        while ".root" in OUTPUT_FOLDER_NAME:
            OUTPUT_FOLDER_NAME = os.path.splitext(OUTPUT_FOLDER_NAME)[0]
        OUTPUT_FOLDER_NAME += "_" + str(sample_iter_num).zfill(5)

        WORK_DIR = os.path.join("/scratch/jburzyns/", OUTPUT_FOLDER_NAME)

        NODE = NODE_LIST[job_iter_num % len(NODE_LIST)]

        local_cmd = ""
        def add_cmd(cmd_str):
            global local_cmd
            local_cmd += cmd_str
            local_cmd += "\n"

        add_cmd("#!/bin/sh\n")

        add_cmd("echoerr() { cat <<< \"$@\" 1>&2; }")

        add_cmd("failsafe_download () {")
        add_cmd("    local RC=1")
        add_cmd("    local TRYCOUNT=1")
        add_cmd("    while [[ $RC -ne 0 && $TRYCOUNT -lt 100 ]]")
        add_cmd("    do")
        add_cmd("       scp -r $1 .")
        add_cmd("       RC=$?")
        add_cmd("       if ((RC != 0)); then")
        add_cmd("         TRYCOUNT=$TRYCOUNT+1")
        add_cmd("         sleep 30s")
        add_cmd("       fi")
        add_cmd("    done")
        add_cmd("}")

        add_cmd("failsafe_upload () {")
        add_cmd("    local RC=1")
        add_cmd("    local TRYCOUNT=1")
        add_cmd("    while [[ $RC -ne 0 && $TRYCOUNT -lt 100 ]]")
        add_cmd("    do")
        add_cmd("       scp -r $1 $2")
        add_cmd("       RC=$?")
        add_cmd("       if ((RC != 0)); then")
        add_cmd("         TRYCOUNT=$TRYCOUNT+1")
        add_cmd("         sleep 30s")
        add_cmd("       fi")
        add_cmd("    done")
        add_cmd("}")

        add_cmd("mkdir -p %s" % WORK_DIR)
        add_cmd("cd %s\n" % WORK_DIR)

        add_cmd("echo $(hostname)")
        add_cmd("echoerr $(hostname)")

        # set up build env
        add_cmd("failsafe_download \"%s:/gpfs3/umass/jburzyns/VH4b/src\"" % NODE)
        add_cmd("[ ! -d \"./src/\" ] && exit 1") # if download failed, just exit
        add_cmd("failsafe_download \"%s:/gpfs3/umass/jburzyns/VH4b/build\"" % NODE)
        add_cmd("[ ! -d \"./build/\" ] && exit 1") # if download failed, just exit
        add_cmd("cd $HOME")
        add_cmd("export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase")
        add_cmd("source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh")
        add_cmd("cd %s/src\n" % WORK_DIR)
        add_cmd("asetup AnalysisBase,21.2.71,here")
        add_cmd("cd %s\n" % WORK_DIR)

        add_cmd("mkdir -p input")
        add_cmd("cd input")

        FILE_LIST_REMOTE = []
        for input_file in input_file_subset:
            add_cmd("failsafe_download \"%s:%s\"" % (NODE, input_file))
            FILE_LIST_REMOTE.append( WORK_DIR + "/input/" + os.path.basename(input_file))

        add_cmd("cd %s" % WORK_DIR)

        CFG_FILE = WORK_DIR + "/src/Hto4bLLPAlgorithm/data/config_Hto4bLLPAlgorithm.py"


        add_cmd("\n")
        add_cmd("source %s/build/x86_64-centos6-gcc62-opt/setup.sh" % WORK_DIR)
        add_cmd("xAH_run.py --config %s --files %s --isMC --submitDir run direct > run.log\n" % (CFG_FILE, " ".join(FILE_LIST_REMOTE)))
        add_cmd("rm -rf %s/input" % WORK_DIR)
        add_cmd("rm -rf %s/build" % WORK_DIR)
        add_cmd("rm -rf %s/src" % WORK_DIR)
        add_cmd("failsafe_upload \"%s\" \"%s:%s\"" % (WORK_DIR, NODE, CATEGORY_OUTPUT_FOLDER_PATH))
        add_cmd("rm -rf %s\n" % WORK_DIR)

        job_filepath = os.path.join(JOB_SCRIPT_DIR, "jobscript_" + OUTPUT_FOLDER_NAME + ".sh")
        job_file = open(job_filepath, "w")
        job_file.write(local_cmd)
        job_file.close()

        #qsub_cmd = "qsub -q tier3 -o %s -e %s -N %s < %s" % (job_output_filepath, job_error_filepath, OUTPUT_FOLDER_NAME, job_filepath)
        qsub_cmd = "qsub -l 'h=!*abc-m25*' -q tier3 -o /dev/null -e /dev/null -N %s < %s" % (OUTPUT_FOLDER_NAME, job_filepath)
        os.system(qsub_cmd)
        #print qsub_cmd
        job_iter_num += 1

        sample_iter_num += 1


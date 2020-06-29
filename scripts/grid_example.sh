# These are the test samples from reprocessing request https://its.cern.ch/jira/browse/DATREP-183

SAMPLE=data18_13TeV.00358031.physics_Main.merge.DAOD_RPVLL.r11760_r11764_p4054
xAH_run.py --config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py --files $SAMPLE --inputRucio prun --optGridMergeOutput 1 --optGridOutputSampleName user.${CERN_USER}.${SAMPLE}_HNLNtuple_01 --optGridNGBPerJob 4

SAMPLE=data17_13TeV.00340368.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054
xAH_run.py --config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py --files $SAMPLE --inputRucio prun --optGridMergeOutput 1 --optGridOutputSampleName user.${CERN_USER}.${SAMPLE}_HNLNtuple_01 --optGridNGBPerJob 4

SAMPLE=data16_13TeV.00304178.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054
xAH_run.py --config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py --files $SAMPLE --inputRucio prun --optGridMergeOutput 1 --optGridOutputSampleName user.${CERN_USER}.${SAMPLE}_HNLNtuple_02 --optGridNGBPerJob 4

SAMPLE=data15_13TeV.00284427.physics_Main.merge.DAOD_RPVLL.r11761_r11764_p4054
xAH_run.py --config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py --files $SAMPLE --inputRucio prun --optGridMergeOutput 1 --optGridOutputSampleName user.${CERN_USER}.${SAMPLE}_HNLNtuple_01 --optGridNGBPerJob 4

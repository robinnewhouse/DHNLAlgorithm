# run data samples on grid

# data15
# xAH_run.py --force \
# --config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
# --inputRucio --files user.rnewhous:data15_13TeV.physics_Main.deriv.DAOD_SUSY15.r11969_r11784_p4072_p4362 \
# --extraOptions="--isSUSY15" \
# prun --optGridOutputSampleName user.$CERN_USER.data15_ntuple_v1 \

# data16
xAH_run.py --force \
--config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
--inputRucio --files user.rnewhous:data16_13TeV.physics_Main.deriv.DAOD_SUSY15.r11969_r11784_p4072_p4362 \
--extraOptions="--isSUSY15" \
prun --optGridOutputSampleName user.$CERN_USER.data16_ntuple_v1 \

# data17
xAH_run.py --force \
--config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
--inputRucio --files user.rnewhous:data17_13TeV.physics_Main.deriv.DAOD_SUSY15.r11969_r11784_p4072_p4362 \
--extraOptions="--isSUSY15" \
prun --optGridOutputSampleName user.$CERN_USER.data17_ntuple_v1 \

# data18
xAH_run.py --force \
--config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
--inputRucio --files user.rnewhous:data18_13TeV.physics_Main.deriv.DAOD_SUSY15.r11969_r11784_p4072_p4362 \
--extraOptions="--isSUSY15" \
prun --optGridOutputSampleName user.$CERN_USER.data18_ntuple_v1 \

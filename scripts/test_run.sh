echo "Testing DHNLAlgorithm"
pwd
ls
# source build/*/setup.sh
xAH_run.py \
--config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
--inputList \
--files files.txt \
--submitDir testRun \
--force \
--isMC \
--extraOptions="--isDerivation --samplePeriod mc16e" \
--nevents=100 \
direct

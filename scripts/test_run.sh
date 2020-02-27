echo "Testing DHNLAlgorithm"
pwd
ls
source build/*/setup.sh
xAH_run.py \
--config ../source/DHNLAlgorithm/data/config_test.py \
--inputList \
--files files.txt \
--submitDir testRun \
--force \
--isMC \
--nevents=10 \
direct


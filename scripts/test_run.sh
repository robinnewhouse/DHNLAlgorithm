echo "Testing DHNLAlgorithm"
export TestArea=/code/build/
pwd
ls
# source build/*/setup.sh
xAH_run.py \
--config /code/src/DHNLAlgorithm/data/config_DHNLAlgorithm.py \
--inputList \
--files files.txt \
--submitDir testRun \
--force \
--isMC \
--nevents=10 \
direct


cd source/DHNLAlgorithm/deps/DVAnalysisBase/
source util/dependencyHack.sh
cd ../../../
cd $TestArea
cmake ../source/
make -j 
source */setup.sh
cd ../run/


echo "Uploading test run of current commit to eos"
last_commit=$(git rev-parse --verify --short HEAD)
uploaded_dir="tests_$last_commit"
#eos root://eosuser.cern.ch
#rsync -av -e ssh \
#--exclude "*.sh" --exclude "*.py" --exclude "*.txt" --exclude "*.swp" --exclude "*.pyc"  \
#../tests/ /eos/user/d/dhnl/ci-test-runs/tests_$last_commit
xrdcp -r ../tests/ root://eosuser.cern.ch//eos/user/d/dhnl/ci-test-runs/
xrdcp ../tests/ root://eosuser.cern.ch//eos/user/d/dhnl/ci-test-runs/tests_$last_commit

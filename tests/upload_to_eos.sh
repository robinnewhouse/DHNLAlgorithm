echo "Uploading test run of current commit to eos"
last_commit=$(git rev-parse --verify --short HEAD)
uploaded_dir="tests_$last_commit"
pwd
ls
cp -r ../tests/  /eos/user/d/dhnl/ci-test-runs/tests_$last_commit
scp -r ../tests/  /eos/user/d/dhnl/ci-test-runs/tests_$last_commit
rsync -av -e ssh \
--exclude "*.sh" --exclude "*.py" --exclude "*.txt" --exclude "*.swp" --exclude "*.pyc"  \
../tests/ /eos/user/d/dhnl/ci-test-runs/tests_$last_commit



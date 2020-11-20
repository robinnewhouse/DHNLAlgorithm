#imports checkpoint test run from eos and prepares groundwork to compare it to current run
echo "Importing checkpoint test run from eos.."
xrdcp root://eosuser.cern.ch//eos/user/d/dhnl/ci-test-runs/tests_checkpoint/testRun/data-tree/files.root .
mv files.root checkpoint_files.root

#this script uploads results and files from ci tests and uploads them to eos
echo "Uploading test run of current commit to eos"
#removing unnecessary files 
rm *.py
rm *.sh
rm *.pyc
rm *.pw?
#make a copy to upload to eos, labelled by git commit hash
last_commit=$(git rev-parse --verify --short HEAD)
upload_dir="tests_$last_commit"
cp ../tests ../$upload_dir
#upload to eos
xrdcp -r ../$upload_dir root://eosuser.cern.ch//eos/user/d/dhnl/ci-test-runs/

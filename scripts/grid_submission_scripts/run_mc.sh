# run mc samples on grid



for infile in \
DAOD_SUSY15_mc16a_promptmuon.txt \
DAOD_SUSY15_mc16a_promptelectron.txt \
DAOD_SUSY15_mc16d_promptmuon.txt \
DAOD_SUSY15_mc16d_promptelectron.txt \
DAOD_SUSY15_mc16e_promptmuon.txt \
DAOD_SUSY15_mc16e_promptelectron.txt \
; do
    while read line; do
        if [[ $line == \#* ]]; then # skip commented lines
            continue 
        fi
        xAH_run.py --force \
        --config $WorkDir_DIR/data/DHNLAlgorithm/config_DHNLAlgorithm.py \
        --inputRucio --files $line \
        --isMC \
        --extraOptions="--isSUSY15" \
        prun --optGridOutputSampleName user.$CERN_USER.$(echo $line | cut -d'.' -f 1,2,5,6).ntuple_v3p0.v1
    done < $infile
done



import argparse
import ROOT
ROOT.gROOT.SetBatch(True)

default_nbins = 50
# Define the ranges and binning for each variable
ranges = {
        "secVtx_VSI_mass": (default_nbins, 0, 30)
}

# Declare a human-readable label for each variable
labels = {
        "secVtx_VSI_mass": "Secondary Vertex Mass;GeV"
}

# Book a histogram for a specific variable
def bookHistogram(df, variable, label, range_):
    return df.Histo1D(ROOT.ROOT.RDF.TH1DModel(variable, label, range_[0], range_[1], range_[2]), variable)

# Write a histogram with a given name to the output ROOT file
def writeHistogram(h, name):
    h.SetName(name)
    h.Write()

def main(sample, output):
    #create output file (and overwrite it if filename already exists)
    tfile = ROOT.TFile(output, "RECREATE")
    variables = ranges.keys()
    
    #load data (where outTree is name of df object and is arbirtary)
    df = ROOT.ROOT.RDataFrame("outTree", sample)
    
    hists = {}
    for variable in variables:
        #book histograms
        hists[variable] = bookHistogram(df, variable, labels[variable], ranges[variable])
        #write histograms
        writeHistogram(hists[variable], variable)
    
    tfile.Close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("sample", type=str, help="Full path to the inputed DHNL sample files")
    parser.add_argument("output", type=str, help="Name of outputed file with histograms")
    args = parser.parse_args()
    main(args.sample, args.output)

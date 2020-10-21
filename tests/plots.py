import argparse
import ROOT
ROOT.gROOT.SetBatch(True)
from histograms import ranges, labels

# Main function for plotting
def main(input_hists, output_dir):
    #obtain the list of variable names from ranges dict defined in histograms.py
    variables = ranges.keys()
    
    #loop through variables and obtain their hists and then plot them
    for variable in variables:    
        tfile = ROOT.TFile(input_hists, "READ")
        h = tfile.Get(variable)
        c = ROOT.TCanvas("", "", 600, 600)
        h.Draw()
        c.Draw()
        c.SaveAs("{}/{}.pdf".format(output_dir, variable))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_hists", type=str, help="Full path to the inputed root file with all the hists")
    parser.add_argument("output_dir", type=str, help="Directory to place all the outputed plots")
    args = parser.parse_args()
    main(args.input_hists, args.output_dir)



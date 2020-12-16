#ci test script to check if any of the histograms made were empty
import argparse
from ROOT import TFile

def test_emptiness(hists):
    #read in the hists
    f = TFile(hists, "READ")
    #obtain list of variables that were histogrammed
    variables = [key.GetName() for key in f.GetListOfKeys()]
    #find (if any) the histograms that are empty
    variables = ranges.keys()
    empty_variables = [variable for variable in variables if sum(f.Get(variable))<=0]
    if len(empty_variables)!=0:
        raise ValueError("The following histograms were found to be empty:\n{0}".format(empty_variables)) #fail
    else:
        return 0 #sucess

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_hists", type=str, help="Directory to histogram root file outputed from histogram stage")
    args = parser.parse_args()
    test_emptiness(args.input_hists)
        

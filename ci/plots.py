import argparse
import ROOT
ROOT.gROOT.SetBatch(True)
from histograms import ranges, labels



#main function for plotting
def main(input_hists, output_dir):
    #obtain the list of variable names from ranges dict defined in histograms.py
    variables = ranges.keys()

    #loop through variables and obtain their hists and then plot them
    for variable in variables:
        tfile = ROOT.TFile(input_hists, "READ")
        h1 = tfile.Get(variable)
        h2 = tfile.Get(variable + "_reference")
        
        c = ROOT.TCanvas("c", "canvas", 600, 600)
        
        #upper plot in pad1
        pad1 = ROOT.TPad("pad1", "pad1", 0, 0.3, 1, 1.0)
        pad1.SetBottomMargin(0.04); # Upper and lower plot are not joined
        pad1.SetGridx();            # Vertical grid
        pad1.Draw();                # Draw the upper pad: pad1
        pad1.cd();                  # pad1 becomes the current pad
        h1.Add(h2)
        h1.Draw()                   # draw h1
        h2.Draw("same")             # draw h2 on top of h1
        
        #not drawing x axis label on upper plot
        h1.GetXaxis().SetLabelSize(0)
        
        #lower plot will be in plot 2
        c.cd() #go back to main canvas before defining pad2
        pad2 = ROOT.TPad("pad2", "pad2", 0, 0.05, 1, 0.3)
        pad2.SetTopMargin(0);
        pad2.SetBottomMargin(0.3);
        pad2.SetGridy(); #horizontal grid
        pad2.Draw();
        pad2.cd();       #pad2 becomes the current pad
        
        #define the ratio plot (h3)
        h3 = h1.Clone()
        h3.SetLineColor(ROOT.kRed);
        h3.SetStats(0);        # No statistics on lower plot
        h3.Divide(h2);
        h3.SetMarkerStyle(47)  #
        h3.Draw("P");          # Draw the ratio plot
        
        #h1 settings
        h1.SetLineColor(ROOT.kBlue+1);
        h1.SetLineWidth(2);
     
        #Y axis h1 plot settings
        h1.GetYaxis().SetTitleSize(20);
        h1.GetYaxis().SetTitleFont(43);
        h1.GetYaxis().SetTitleOffset(1.55);
     
        #h2 settings
        h2.SetLineColor(ROOT.kRed);
        h2.SetLineWidth(2);
     
        #Ratio plot (h3) settings
        h3.SetTitle(""); # Remove the ratio title
     
        #Y axis ratio plot settings
        h3.GetYaxis().SetTitle("ratio       ");
        h3.GetYaxis().SetNdivisions(505);
        h3.GetYaxis().SetTitleSize(20);
        h3.GetYaxis().SetTitleFont(43);
        h3.GetYaxis().SetTitleOffset(1);
        h3.GetYaxis().SetLabelFont(43); # Absolute font size in pixel (precision 3)
        h3.GetYaxis().SetLabelSize(15);
     
        #X axis ratio plot settings
        h3.GetXaxis().SetTitleSize(20);
        h3.GetXaxis().SetTitleFont(43);
        h3.GetXaxis().SetTitleOffset(4.);
        h3.GetXaxis().SetLabelFont(43); #Absolute font size in pixel (precision 3)
        h3.GetXaxis().SetLabelSize(15);

        c.SaveAs("{}/{}.pdf".format(output_dir, variable))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_hists", type=str, help="Full path to the inputed root file with all the hists")
    parser.add_argument("output_dir", type=str, help="Directory to place all the outputed plots")
    args = parser.parse_args()
    main(args.input_hists, args.output_dir)



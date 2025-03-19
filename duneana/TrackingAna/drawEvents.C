#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLeaf.h"
#include "TF1.h"
#include "TMinuit.h"
#include <typeinfo>

using namespace std;

std::vector<int> channelNum, tpcNum, planeNum, wireNum;
std::vector<float> xVal, yVal, zVal, aVal, bVal, cVal;

void readChannelMap() {
    // Get the file
    TString fileName = "./icebergwiredump_v2.txt";
    std::ifstream inputFile(fileName.Data());
    
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << fileName << std::endl;
        return;
    }

    // Define the variables
    int channel;
    int tpc;
    int plane;
    int wire;
    float x, y, z, a, b, c;

//    std::vector<int> tpcNum, planeNum, wireNum;
//    std::vector<float> xVal, yVal, zVal, aVal, bVal, cVal;

    // Read the text file line by line
    std::string line;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        
        // Values from the text file: channel tpc plane wire x y z a b c
        ss >> channel >> tpc >> plane >> wire >> x >> y >> z >> a >> b >> c;

        // Save them to a vector
        channelNum.push_back(channel);
        tpcNum.push_back(tpc);
        planeNum.push_back(plane);
        wireNum.push_back(wire);
        xVal.push_back(x);
        yVal.push_back(y);
        zVal.push_back(z);
        aVal.push_back(a);
        bVal.push_back(b);
        cVal.push_back(c);
    }

    cout << "Have a great day!!" << endl;

}

void plotHits(){

    // Read in the channel map
    readChannelMap();

    // Make vectors of 2D histograms - will set up the histograms in the loop
    TH2F* hist_u_tpc0;
    TH2F* hist_v_tpc0;
    TH2F* hist_z_tpc0;

    TH2F* hist_u_tpc1;
    TH2F* hist_v_tpc1;
    TH2F* hist_z_tpc1;

    // Open the ROOT file
    TFile *file = TFile::Open("/exp/dune/app/users/odalager/area_v10/hit_hist_stopped_muons_1.root", "READ"); //CHANGE FILE PATH/NAME HERE!!!!
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file!!" << std::endl;
        return;
    }

    // Access the TTree
    TTree* tree = (TTree*)file->Get("hitTree");
    if (!tree) {
        std::cerr << "Error: TTree 'hitTree' not found in file!" << std::endl;
        return;
    }

    // Declare variables for branches
    int event_number;
    int channel;
    float peak_amplitude;
    float start_tick;
    float end_tick;

    // Set up branches
    tree->SetBranchAddress("eventNumber", &event_number);
    tree->SetBranchAddress("channel", &channel);
    tree->SetBranchAddress("peakAmplitude", &peak_amplitude);
    tree->SetBranchAddress("startTick", &start_tick);
    tree->SetBranchAddress("endTick", &end_tick);

    // Set a threshold for the hits
    float threshold = 0.0;

    // Determine the number of entry in the TTree and variables for the loop
    int n_entries = tree->GetEntries();
    int prev_eventNum=-1;
    int this_eventNum=-1;
    int next_eventNum=-1;
    int drawPlot=-1;

    // Iterate through the events in the TTree
    for (int i = 0; i < n_entries; ++i) {
        tree->GetEntry(i);  // Load the data for the current entry
        this_eventNum = event_number;
        if(prev_eventNum==-1 || this_eventNum!=prev_eventNum){ //if it's a new event number, make a new set of histograms
            hist_u_tpc0 = new TH2F(Form("Event %d TPC 0 U Plane", this_eventNum), Form("Event %d -- TPC 0 -- U Plane Peak Amplitude vs. Time Tick vs. Channel", this_eventNum),
                                    6000, 0, 6000,    // Time ticks (X-axis)
                                    200, 0, 199);     // Channel numbers (Y-axis) for U plane

            hist_v_tpc0 = new TH2F(Form("Event %d TPC 0 V Plane", this_eventNum), Form("Event %d -- TPC 0 -- V Plane Peak Amplitude vs. Time Tick vs. Channel", this_eventNum),
                                    6000, 0, 6000,    // Time ticks (X-axis)
                                    400, 400, 599);   // Channel numbers (Y-axis) for V plane

            hist_z_tpc0 = new TH2F(Form("Event %d TPC 0 Z Plane", this_eventNum), Form("Event %d  -- TPC 0-- Z Plane Peak Amplitude vs. Time Tick vs. Channel", this_eventNum),
                                    6000, 0, 6000,    // Time ticks (X-axis)
                                    480, 800, 1039);  // Channel numbers (Y-axis) for Z plane


            hist_u_tpc1 = new TH2F(Form("Event %d TPC 1 U Plane", this_eventNum), Form("Event %d -- TPC 1 -- U Plane Peak Amplitude vs. Time Tick vs. Channel", this_eventNum),
                                    6000, 0, 6000,    // Time ticks (X-axis)
                                    400, 200, 399);     // Channel numbers (Y-axis) for U plane

            hist_v_tpc1 = new TH2F(Form("Event %d TPC 1 V Plane", this_eventNum), Form("Event %d -- TPC 1 -- V Plane Peak Amplitude vs. Time Tick vs. Channel", this_eventNum),
                                    6000, 0, 6000,    // Time ticks (X-axis)
                                    400, 600, 799);   // Channel numbers (Y-axis) for V plane

            hist_z_tpc1 = new TH2F(Form("Event %d TPC 1 Z Plane", this_eventNum), Form("Event %d -- TPC 1 -- Z Plane Peak Amplitude vs. Time Tick vs. Channel", this_eventNum),
                                    6000, 0, 6000,    // Time ticks (X-axis)
                                    480, 1040, 1279);  // Channel numbers (Y-axis) for Z plane
        }



        // Fill the Histograms
        //technically filling all the histograms with the values from both TPCs but the binning established in making the histograms prevents it from being an issue
            for (int tick = (int)start_tick; tick <= (int)end_tick; ++tick) {
                if (peak_amplitude >= threshold){
                    if (channel >= 0 && channel <= 399) {  // U plane
                        hist_u_tpc0->Fill(tick, channel, peak_amplitude);
                        hist_u_tpc1->Fill(tick, channel, peak_amplitude);
                    } else if (channel >= 400 && channel <= 799) {  // V plane
                        hist_v_tpc0->Fill(tick, channel, peak_amplitude);
                        hist_v_tpc1->Fill(tick, channel, peak_amplitude);
                    } else if (channel >= 800 && channel <= 1279) {  // Z plane
                        hist_z_tpc0->Fill(tick, channel, peak_amplitude);
                        hist_z_tpc1->Fill(tick, channel, peak_amplitude);
                    }
                }
            }




        prev_eventNum = event_number;

        if(i == n_entries-1) drawPlot=1; // Check if its the last entry in the TTree -- if so, want to draw the histograms
        else{// Check if the next entry is the same event as this one -- if not, want to draw the histograms
            tree->GetEntry(i+1);  // Load the data for the current entry
            next_eventNum = event_number;
            if(this_eventNum != next_eventNum) drawPlot=1;
        }

        if(drawPlot==1){ // Draw the plot
//            if(this_eventNum!=185) continue; //IF YOU ONLY WANT TO PLOT ONE OF THEM
            // make the TCanvas to put the plots on
            TCanvas *canvas = new TCanvas("canvas", Form("Event %d Plane Histograms",this_eventNum), 1200, 800);
            canvas->Divide(2, 3);  // Divide the canvas into 2x2 for multiple histograms
            canvas->cd(1);
            hist_u_tpc0->SetStats(0);
            hist_u_tpc0->Draw("COLZ"); // draw U plane
            canvas->cd(2);
            hist_u_tpc1->SetStats(0);
            hist_u_tpc1->Draw("COLZ"); // draw U plane
            canvas->cd(3);
            hist_v_tpc0->SetStats(0);
            hist_v_tpc0->Draw("COLZ"); // draw V plane
            canvas->cd(4);
            hist_v_tpc1->SetStats(0);
            hist_v_tpc1->Draw("COLZ"); // draw V plane
            canvas->cd(5);
            hist_z_tpc0->SetStats(0);
            hist_z_tpc0->Draw("COLZ"); // draw Z plane
            canvas->cd(6);
            hist_z_tpc1->SetStats(0);
            hist_z_tpc1->Draw("COLZ"); // draw Z plane
            canvas->Update();
            canvas->Print(Form("/exp/dune/app/users/odalager/area_v10/plots/hits/run1_event%d_planeHists.png",this_eventNum)); //CHANGE OUTPUT FILE NAMES HERE
        }

        drawPlot = -1;

    }


    file->Close();

}
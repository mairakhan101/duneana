#include <numeric>
#include <string>
#include <vector>
#include <iostream>

#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "nusimdata/SimulationBase/MCParticle.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TPolyLine3D.h"


using namespace std;

int main(int argc, char** argv){

//How to run this code:
//  With CMakeLists.txt: update information to apply to this file
//  Any time CMakeLists.txt is updated, need to: cmake .
//  Any time this code gets changed, need to update the executable: make
//  Then to run the executable: ./drawTruth <filename_example:/exp/dune/app/users/odalager/area_v10/stopped_muon_gentps_1.root>


  if(argc != 2) //checks that you gave it only one artROOT input file
  {
    std::cerr << "Please provide exactly one artROOT input file." << std::endl;
    return 1;
  }


  //make the output file that we will write to
  //TFile * outfile = new TFile("output_truth_stoppedMuonSim.root", "RECREATE");

  //int thisEvent=8;

  std::vector<std::string> const input_files(argv + 1, argv + argc); //defines the input file
  art::InputTag const tag("largeant"); //MODULE LABEL
  int ievent=0;
  for(gallery::Event evt(input_files); !evt.atEnd(); evt.next())
  {
  /*  if(ievent<thisEvent){
      ievent++;
       continue;
    }
    if(ievent>thisEvent) break;*/

    double momentum[2][3];

      for(int i=0; i<2; i++){
        for(int i_dimension=0; i_dimension<3; i_dimension++){
          momentum[i][i_dimension]=0;
        }
      }

    TCanvas* c1 = new TCanvas("c1","multigraph L3",200,10,700,500);

    TH3F *h = new TH3F("h", "MCParticle Truth", 10, -30, 30, 10, -50, 50, 10, -50, 50);
    h->SetStats(0);  // Hide the stats box
    h->GetXaxis()->SetTitle("x");
    h->GetYaxis()->SetTitle("y");
    h->GetZaxis()->SetTitle("z");
    h->Draw();     // Draw the axes

    auto const& particle = *evt.getValidHandle<std::vector<simb::MCParticle>>(tag); //from DATA PRODUCT TYPE
    for(const simb::MCParticle & p : particle)
    {
      // do stuff.
      //temp: cout values
      std::cout << "Particle " << p.Mother() << "\t" << p.PdgCode() << "\tMomentum: " << p.Px() << ", " << p.Py() << ", " << p.Pz() << std::endl; //get other variables like StartTick, Channel, PeakTime, RMS, PeakAmplitude

      if(p.Mother() > 1) continue;
      momentum[p.Mother()][0] += p.Px();
      momentum[p.Mother()][1] += p.Py();
      momentum[p.Mother()][2] += p.Pz();


      //Make a line for each of the particles
        // Create a TPolyLine3D object
        TPolyLine3D *line = new TPolyLine3D();

        // Set the points for the line (x, y, z coordinates)
        line->SetPoint(0, p.Vx(), p.Vy(), p.Vz()); // Start point
        line->SetPoint(1, p.EndX(), p.EndY(), p.EndZ()); // End point
        
        if(p.PdgCode() == 11){ //electron
            line->SetLineColor(kRed);
        }
        if(p.PdgCode() == -11){ //positron
            line->SetLineColor(kRed);
            line->SetLineStyle(9);
        }
        if(p.PdgCode() == 12){ //electron neutrino
            line->SetLineColor(kBlue);
        }
        if(p.PdgCode() == -12){ //electron antineutrino
            line->SetLineColor(kBlue);
            line->SetLineStyle(9);
        }
        if(p.PdgCode() == 13){ //muon
            line->SetLineColor(kBlack);
        }
        if(p.PdgCode() == -13){ //antimuon
            line->SetLineColor(kBlack);
            line->SetLineStyle(9);
        }
        if(p.PdgCode() == 14){ //muon neutrino
            line->SetLineColor(kGreen);
        }
        if(p.PdgCode() == -14){ //muon antineutrino
            line->SetLineColor(kGreen);
            line->SetLineStyle(9);
        }

        // Draw the line
        c1->cd();
        line->Draw("same");

    }
    
    cout << "Check Momentum Conservation!! x: " << momentum[1][0]-momentum[0][0] << "\t y: " << momentum[1][1]-momentum[0][1] << "\t z: " <<  momentum[1][2]-momentum[0][2] << endl;

    c1->Print(Form("/exp/dune/app/users/odalager/area_v10/plots/truth/truth_event%d.png",ievent));
    ievent++;
  }

//  outfile->Close();

  std::cout << "Have a wonderful day!!!" << std::endl;

  return 0;
}
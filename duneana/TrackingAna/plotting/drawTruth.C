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
#include "TPolyMarker3D.h"


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

  int numContained=0;
  int numTotal=0;
  int numAbove=0;
  int numBelow=0;

  //make the output file that we will write to
  //TFile * outfile = new TFile("output_truth_stoppedMuonSim.root", "RECREATE");

  //int thisEvent=8;

  TCanvas* c_all = new TCanvas("c_all","all events",200,10,700,500);
    TH3F *h_allevents = new TH3F("h", "MCParticle Truth", 10, -300, 300, 10, -500, 500, 10, -500, 500);
    h_allevents->SetStats(0);  // Hide the stats box
    h_allevents->GetXaxis()->SetTitle("x");
    h_allevents->GetYaxis()->SetTitle("y");
    h_allevents->GetZaxis()->SetTitle("z");
    h_allevents->Draw();     // Draw the axes

    //Draw the detector in the space
        double x_min = -30, x_max = 30;
        double y_min = -50, y_max = 50;
        double z_min = -50, z_max = 50;

        // 8 corners of the box
        double x[8] = {x_min, x_max, x_max, x_min, x_min, x_max, x_max, x_min};
        double y[8] = {y_min, y_min, y_max, y_max, y_min, y_min, y_max, y_max};
        double z[8] = {z_min, z_min, z_min, z_min, z_max, z_max, z_max, z_max};

        // Define 12 edges (each with 2 points) using a TPolyLine3D
        int edge_indices[24] = {
            0,1, 1,2, 2,3, 3,0,  // bottom face
            4,5, 5,6, 6,7, 7,4,  // top face
            0,4, 1,5, 2,6, 3,7   // vertical edges
        };

        TPolyLine3D* iceberg_box = new TPolyLine3D(24);
        for (int i = 0; i < 24; i++) {
            int idx = edge_indices[i];
            iceberg_box->SetPoint(i, x[idx], y[idx], z[idx]);
        }

        //For plotting the interaction points
        const int n_points = 30;
        TPolyMarker3D* interaction_vertices = new TPolyMarker3D(n_points, kFullDotMedium);

        //Set up a vector to only save those which are inside the detector -- for color coding purposes
        std::vector<double> interactions_inside_x, interactions_inside_y, interactions_inside_z;        

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

    double muonEnd_x, muonEnd_y, muonEnd_z=0;
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
//      std::cout << "Particle " << p.Mother() << "\t" << p.PdgCode() << "\tMomentum: " << p.Px() << ", " << p.Py() << ", " << p.Pz() << std::endl; //get other variables like StartTick, Channel, PeakTime, RMS, PeakAmplitude
      if(p.Mother()==0){
        muonEnd_x=p.EndX();
        muonEnd_y=p.EndY();
        muonEnd_z=p.EndZ();
      }


      if(p.Mother() > 1) continue;
      momentum[p.Mother()][0] += p.Px();
      momentum[p.Mother()][1] += p.Py();
      momentum[p.Mother()][2] += p.Pz();

      //Plot the interaction vertices
      interaction_vertices->SetPoint(ievent, muonEnd_x, muonEnd_y, muonEnd_z);


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
//        c_all->cd();
//        line->Draw("same");

    }
    
//    cout << "Check Momentum Conservation!! x: " << momentum[1][0]-momentum[0][0] << "\t y: " << momentum[1][1]-momentum[0][1] << "\t z: " <<  momentum[1][2]-momentum[0][2] << endl;

    cout << endl << "MUON ENDS @ " << muonEnd_x << ", " << muonEnd_y << ", " << muonEnd_z << endl;

    numTotal++;

    if(muonEnd_x>=-30 && muonEnd_x<=30 && muonEnd_y>=-50 && muonEnd_y<=50 && muonEnd_z>=-50 && muonEnd_z<=50){
      cout << "muon ends INSIDE the detector" << endl;
      interactions_inside_x.push_back(muonEnd_x);
      interactions_inside_y.push_back(muonEnd_y);
      interactions_inside_z.push_back(muonEnd_z);
      numContained++;
    }
    else{
      cout << "!!! OUTSIDE the detector !!!" << endl;
      if(muonEnd_z<-50){
          cout << "Below the TPC" << endl;
          numBelow++;
      }
      if(muonEnd_z>50){
        cout << "Above the TPC" << endl;
        numAbove++;
      }

    }

//    c1->Print(Form("/exp/dune/app/users/odalager/area_v10/plots/truth/mod/truth_originPLUSzeroMomentum_event%d.png",ievent));
    cout << "----------------" << endl << endl;

    ievent++;
  }

  //Mark the points inside the detector with a different color
  TPolyMarker3D* interaction_vertices_inside = new TPolyMarker3D(interactions_inside_x.size(), kFullDotMedium);
  for(int i=0; i<interactions_inside_x.size(); i++){
    interaction_vertices_inside->SetPoint(i, interactions_inside_x.at(i), interactions_inside_y.at(i), interactions_inside_z.at(i));
  }



  c_all->cd();
  iceberg_box->Draw("same");
  interaction_vertices->SetMarkerColor(kRed);
  interaction_vertices->Draw("same");
  interaction_vertices_inside->SetMarkerColor(kBlue);
  interaction_vertices_inside->Draw("same");
  c_all->Print("/exp/dune/app/users/odalager/area_v10/plots/truth/mod/truth_AllInteractions_pxyz15_50.png");


//  outfile->Close();

  cout << endl << "Number of events contained: " << numContained << " / " << numTotal << "   =   " << double(numContained)/double(numTotal)*100 << " %" << endl;
  cout << "\tNumber of events Above: " << numAbove << " / " << numTotal << "   =   " << double(numAbove)/double(numTotal)*100 << " %" << endl;
  cout << "\tNumber of events Below: " << numBelow << " / " << numTotal << "   =   " << double(numBelow)/double(numTotal)*100 << " %" << endl;

  std::cout << "Have a wonderful day!!!" << std::endl;

  return 0;
}
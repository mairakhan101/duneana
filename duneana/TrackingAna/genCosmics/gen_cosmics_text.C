#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TPolyLine.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TTree.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

void gen_cosmics_text(const int num_events = 100) {
  std::cout << "Writing events to: textfile!" << std::endl;

  std::ofstream outputFile("test_modZY_debug.txt");

  TF2 *fdsigdT = new TF2("xsecform","18*TMath::Power(x + 1/y,-2.7)/(x*y+145) + (x + 5)/(x + 5/y)",0,1,0.1,1);

  TH1F *eHist = new TH1F("ehist","Energy",25,0,1);
  TH1F *tHist = new TH1F("thist","cos(theta)",25,0.1,1);

  double p = 0;
  double costheta = 1;

  double px = 0;
  double py = 0;
  double pz = 0;
  double E = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  double t = 0;
  double phi = 0;

  TRandom3 *fRand = new TRandom3(0);

  for (int i=0; i<num_events; i++){

    fdsigdT->GetRandom2(p,costheta);
    phi = fRand->Uniform(0,2*TMath::Pi());

    E = p*p + 0.105;

/*    px = p*sqrt(1-costheta*costheta)*sin(phi);
    py = -1*p*costheta;
    pz = p*sqrt(1-costheta*costheta)*sin(phi);*/

    double sintheta = sqrt(1 - costheta * costheta);
    px = p * sintheta * cos(phi);
    py = p * sintheta * sin(phi);
    pz = p * costheta;

    x = fRand->Uniform(-30,30);
    y = fRand->Uniform(-50, 50);
    z = fRand->Uniform(-50, 50);

    t = fRand->Uniform(-3.325e-3,0)*1e9;

    outputFile << i << " " << 1 << std::endl;
    outputFile << "1 -13 0 0 0 0 " << px << " " << py << " " << pz << " " << E << " 0.105 " << x << " " << y << " " << z << " " << t << std::endl;

    eHist->Fill(E);
    tHist->Fill(costheta);
  }

  /**
  // This block for verifying produced distributions
  TCanvas *canvas1 = new TCanvas("canvas1","Canvas for Histograms", 800, 600);
  eHist->Draw();
  canvas1->SaveAs("eHist.png");

  delete canvas1;

  TCanvas *canvas2 = new TCanvas("canvas2","Canvas for Histograms", 800, 600);
  tHist->Draw();
  canvas2->SaveAs("tHist.png");

  delete canvas2;
  **/

  outputFile.close();
}


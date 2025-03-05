////////////////////////////////////////////////////////////////////////
// Class:       TpAnalyzer
// Module Type: analyzer
// File:        TpAnalyzer_module.cc
////////////////////////////////////////////////////////////////////////

// C++ includes

// ROOT includes
#include "TTree.h"
#include "TH1F.h"
//#include "TFile.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"


// LArSoft includes 
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "larsim/Utils/TruthMatchUtils.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "larcore/Geometry/Geometry.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"

// NuSim includes
#include "nusimdata/SimulationBase/MCParticle.h"
#include "nusimdata/SimulationBase/MCTruth.h"

class TpAnalyzer;

class TpAnalyzer : public art::EDAnalyzer {

public:

  explicit TpAnalyzer(fhicl::ParameterSet const & pset);

  // The main guts...
  void analyze(art::Event const & evt) override;

  void beginJob();
  void endJob();

  // custom functions
  void InitHistograms(art::ServiceHandle<art::TFileService> tfs);
  void FillHitHistograms(const recob::Hit* hit);

private:

  // fcl params
  bool        fPrintDebug; // Consider printing debug info
  std::string fHitLabel; // Label for recob::Hit data product

  // hit level histograms
  TH1F* fChannel;
  TH1F* fStartTick; // initial tdc tick for hit
  TH1F* fEndTick; // final tdc tick
  TH1F* fPeakTime; // time of signal peak, in tick units
  TH1F* fSigmaPeakTime; // uncertainty for the signal peak, in tick units
  TH1F* fRMS; // RMS of the hit shape, in tick units
  TH1F* fPeakAmplitude; // the estimated amplitude of the hit at its peak, in ADC units
  TH1F* fSigmaPeakAmplitude; // uncertainty on estimated amplitude of the hit at its peak (ADC units)
  TH1F* fROISummedADC; // the sum of calibrated ADC counts of the ROI
  TH1F* fHitSummedADC; // the sum of calibrated ADC counts of the ROI
  TH1F* fIntegral; // the integral under the calibrated signal waveform of the hit (tick x ADC units)
  TH1F* fSigmaIntegral; // the uncertainty of integral under the calibrated signal waveform of the hit, in ADC units
  TH1F* fMultiplicity; // how many hits could this one be shared with
  TH1F* fLocalIndex; // index of this hit among the Multiplicity() hits in the signal window
  TH1F* fGoodnessOfFit; // how well do we believe we know this hit?
  TH1F* fNDF; // degrees of freedom in the determination of the hit shape
  TH1F* fView; // view for the plane of the hit
  TH1F* fSignalType; // signal type for the plane of the hit
  TH1F* fWireID; // WireID for the hit (Cryostat, TPC, Plane, Wire)

};

//......................................................
TpAnalyzer::TpAnalyzer(fhicl::ParameterSet const& pset)
  : EDAnalyzer(pset),
    fPrintDebug        (pset.get<bool>("PrintDebug")               ),
    fHitLabel          (pset.get<std::string>("HitLabel")          )
{
}


//......................................................
void TpAnalyzer::beginJob()
{
  art::ServiceHandle<art::TFileService> tfs;
  //art::ServiceHandle<geo::Geometry> geometry;
  InitHistograms(tfs);

  //fGeometry = &*geometry;
}

//......................................................
void TpAnalyzer::endJob()
{
}

//......................................................
void TpAnalyzer::analyze(art::Event const & evt)
{
 
    // read in recob hits
    std::vector<art::Ptr<recob::Hit>> hitList;
    auto hitListHandle = evt.getHandle<std::vector<recob::Hit>>(fHitLabel);
    if (hitListHandle) { art::fill_ptr_vector(hitList, hitListHandle); }
    size_t nHits = hitList.size();

    ///////////////////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////////////////

    // fill hit histograms
    for (size_t i =0; i<nHits; i++) {
      const recob::Hit* hit = hitList.at(i).get();
      FillHitHistograms(hit);
      if (fPrintDebug == true) {
	int cryoNum = hit->WireID().Cryostat;
	int tpcNum  = hit->WireID().TPC;
	int planeNum = hit->WireID().Plane;
	int wireNum = hit->WireID().Wire;
	int channelNum = hit->Channel();
	std::cout << "Cryo, TPC, Plane, Wire, Channel  = " << cryoNum << ", " << tpcNum << ", " << planeNum << ", " << wireNum << ", " << channelNum << std::endl;
      }
    }

 }   
      

void TpAnalyzer::InitHistograms(art::ServiceHandle<art::TFileService> tfs) {

    // hit histograms
    fChannel = tfs->make<TH1F>("fChannel", "Channel number", 1280, 0, 1279);
    fStartTick = tfs->make<TH1F>("fStartTick", "Start Tick", 3050, 0.0, 6100.0);
    fEndTick = tfs->make<TH1F>("fEndTick", "End Tick", 3050, 0.0, 6100.0);
    fPeakTime = tfs->make<TH1F>("fPeakTime", "Peak Time", 3050, 0.0, 6100.0);
    fSigmaPeakTime = tfs->make<TH1F>("fSigmaPeakTime", "Sigma of Peak Time", 100, 0.0, 1.0);
    fRMS = tfs->make<TH1F>("fRMS", "RMS of Hit Shape", 150, 0.0, 150);
    fPeakAmplitude = tfs->make<TH1F>("fPeakAmplitude", "Peak Amplitude (ADC units)", 2500, 0.0, 2500);
    fSigmaPeakAmplitude = tfs->make<TH1F>("fSigmaPeakAmplitude", "Uncertainty in Peak Amplitude (ADC units)", 100, 0.0, 1.0);
    fROISummedADC = tfs->make<TH1F>("fROISummedADC", "Sum of Calibrated ADC Counts (ROI)", 60000, 0.0, 60000.0);
    fHitSummedADC = tfs->make<TH1F>("fHitSummedADC", "Sum of Calibrated ADC Counts (Hit)", 60000, 0.0, 60000.0);
    fIntegral = tfs->make<TH1F>("fIntegral", "Integral under Signal Waveform", 60000, 0.0, 60000.0);
    fSigmaIntegral = tfs->make<TH1F>("fSigmaIntegral", "Uncertainty of Integral (ADC units)", 1000, 0.0, 1.0);
    fMultiplicity = tfs->make<TH1F>("fMultiplicity", "Hit Multiplicity", 100, 0.0, 2.0);
    fLocalIndex = tfs->make<TH1F>("fLocalIndex", "Local Index of Hit", 100, 0.0, 2.0);
    fGoodnessOfFit = tfs->make<TH1F>("fGoodnessOfFit", "Goodness of Fit", 10, 0.0, 2.0);
    fNDF = tfs->make<TH1F>("fNDF", "Degrees of Freedom (NDF)", 100, 0.0, 2.0);
    fView = tfs->make<TH1F>("fView", "View for Plane of Hit", 3, 0.0, 3.0);
    fSignalType = tfs->make<TH1F>("fSignalType", "Signal Type for Plane of Hit", 10, 0.0, 10.0);
    //fWireID = tfs->make<TH1F>("fWireID", "Wire ID (Cryostat, TPC, Plane, Wire)", 1000, 0.0, 10000.0);
}


void TpAnalyzer::FillHitHistograms(const recob::Hit* hit) {
  
  // fill histograms
  fChannel->Fill(hit->Channel());
  fStartTick->Fill(hit->StartTick());
  fEndTick->Fill(hit->EndTick());
  

}

DEFINE_ART_MODULE(TpAnalyzer)

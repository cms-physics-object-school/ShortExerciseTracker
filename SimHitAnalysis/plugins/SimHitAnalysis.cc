// -*- C++ -*-
//
// Package:    TrackerDPG/SimHitAnalysis
// Class:      SimHitAnalysis
//
/**\class SimHitAnalysis SimHitAnalysis.cc TrackerDPG/SimHitAnalysis/plugins/SimHitAnalysis.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Suvankar Roy Chowdhury
//         Created:  Wed, 17 Jul 2019 15:58:07 GMT
//
//


// system include files
#include <memory>
#include "TH2D.h"
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

//#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class SimHitAnalysis : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit SimHitAnalysis(const edm::ParameterSet&);
      ~SimHitAnalysis();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
      void fillSimHitHistos(const edm::ESHandle<TrackerGeometry>& trackerGeometry, 
                            const edm::Handle<edm::PSimHitContainer>& psHitHandle);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      TH2D* hSimHit_Barrel_XY;
      TH2D* hSimHit_Endcap_plus_XY;
      TH2D* hSimHit_Endcap_minus_XY;
      TH2D* hSimHit_Barrel_RZ;
      TH2D* hSimHit_Endcap_plus_RZ;
      TH2D* hSimHit_Endcap_minus_RZ;

      edm::EDGetTokenT<edm::PSimHitContainer> psimBarrelPixelToken_;
      edm::EDGetTokenT<edm::PSimHitContainer> psimEndcapPixelToken_;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
SimHitAnalysis::SimHitAnalysis(const edm::ParameterSet& iConfig)
 :
  psimBarrelPixelToken_(consumes<edm::PSimHitContainer>(edm::InputTag("g4SimHits", "TrackerHitsPixelBarrelHighTof"))),
  psimEndcapPixelToken_(consumes<edm::PSimHitContainer>(edm::InputTag("g4SimHits", "TrackerHitsPixelEndcapHighTof")))
{
  edm::Service<TFileService> fs;
  hSimHit_Barrel_XY       = fs->make<TH2D>( "hSimHit_Barrel_XY", "PSimHit Barrel y vs. x", 960, -120, 120, 960, -120, 120 );
  hSimHit_Endcap_plus_XY  = fs->make<TH2D>( "hSimHit_Endcap_plus_XY", "PSimHit Forward Endcap y vs. x", 960, -120, 120, 960, -120, 120 );
  hSimHit_Endcap_minus_XY = fs->make<TH2D>( "hSimHit_Endcap_minus_XY", "PSimHit Forward Endcap y vs. x", 960, -120, 120, 960, -120, 120 );
  hSimHit_Barrel_RZ       = fs->make<TH2D>( "hSimHit_Barrel_RZ", "PSimHit #rho vs. z", 900, -300, 300, 480, 0, 120 );
  hSimHit_Endcap_plus_RZ  = fs->make<TH2D>( "hSimHit_Endcap_plus_RZ", "PSimHit #rho vs. z", 900, -300, 300, 480, 0, 120 );
  hSimHit_Endcap_minus_RZ = fs->make<TH2D>( "hSimHit_Endcap_minus_RZ", "PSimHit #rho vs. z", 900, -300, 300, 480, 0, 120 );
}

SimHitAnalysis::~SimHitAnalysis()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
SimHitAnalysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::ESHandle<TrackerGeometry> trackerGeometry;
  iSetup.get<TrackerDigiGeometryRecord>().get(trackerGeometry);

  //A PSimHit container is a vector<PSimHit>
  edm::Handle<edm::PSimHitContainer> simHitHandleBPix;
  edm::Handle<edm::PSimHitContainer> simHitHandleFPix;
  iEvent.getByToken(psimBarrelPixelToken_, simHitHandleBPix);
  iEvent.getByToken(psimEndcapPixelToken_, simHitHandleFPix);

  fillSimHitHistos(trackerGeometry, simHitHandleBPix);
  fillSimHitHistos(trackerGeometry, simHitHandleFPix);

}

void SimHitAnalysis::fillSimHitHistos(const edm::ESHandle<TrackerGeometry>& trackerGeometry, 
                                      const edm::Handle<edm::PSimHitContainer>& psHitHandle) {
  for(const auto& iSimHit : *psHitHandle) {
    const GeomDetUnit* gDetUnit = trackerGeometry->idToDetUnit( iSimHit.detUnitId() );
    GlobalPoint simhitPos = gDetUnit->surface().toGlobal( iSimHit.localPosition() ) ;
    if (gDetUnit->type().isBarrel()) {
      hSimHit_Barrel_XY->Fill( simhitPos.x(), simhitPos.y() );
      hSimHit_Barrel_RZ->Fill( simhitPos.z(), simhitPos.perp());
    } else if (gDetUnit->type().isEndcap()) {
      if(simhitPos.z() > 0) {
        hSimHit_Endcap_plus_XY->Fill( simhitPos.x(), simhitPos.y() );
        hSimHit_Endcap_plus_RZ->Fill( simhitPos.z(), simhitPos.perp());
      } else {
        hSimHit_Endcap_minus_XY->Fill( simhitPos.x(), simhitPos.y() );
        hSimHit_Endcap_minus_RZ->Fill( simhitPos.z(), simhitPos.perp());
      }
    }

  }
}


// ------------ method called once each job just before starting event loop  ------------
void
SimHitAnalysis::beginJob()
{

}

// ------------ method called once each job just after ending the event loop  ------------
void
SimHitAnalysis::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SimHitAnalysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(SimHitAnalysis);
/*
Available simHit tokens
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsPixelBarrelHighTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsPixelBarrelLowTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsPixelEndcapHighTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsPixelEndcapLowTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTECHighTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTECLowTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTIBHighTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTIBLowTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTIDHighTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTIDLowTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTOBHighTof"   "SIM"     
vector<PSimHit>                       "g4SimHits"                 "TrackerHitsTOBLowTof"   "SIM"  
*/

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EEDataFrame.h"
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimCalorimetry/CaloSimAlgos/interface/CaloHitResponse.h"
#include "SimCalorimetry/CaloSimAlgos/interface/CaloTDigitizer.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalSimParameterMap.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalShape.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalDigitizerTraits.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalCoder.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalElectronicsSim.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include <vector>
#include<iostream>
#include<iterator>
using namespace std;
using namespace cms;

int main() {
  // make a silly little hit in each subdetector, which should
  // correspond to a 100 GeV particle
  EBDetId barrelDetId(1, 1);
  PCaloHit barrelHit(barrelDetId.rawId(), 100., 0.);

  EEDetId endcapDetId(1, 1, 1);
  PCaloHit endcapHit(endcapDetId.rawId(), 1000., 0.);

  vector<DetId> barrelDetIds, endcapDetIds;
  barrelDetIds.push_back(barrelDetId);
  endcapDetIds.push_back(endcapDetId);

  edm::PCaloHitContainer barrelHits, endcapHits;
  barrelHits.push_back(barrelHit);
  endcapHits.push_back(endcapHit);

  string barrelName = "EcalHitsEB";
  string endcapName = "EcalHitsEE";
  vector<string> caloDets, trackingDets;
  caloDets.push_back(barrelName);
  caloDets.push_back(endcapName);

  CrossingFrame crossingFrame(-5, 5, 25, trackingDets, caloDets);
  crossingFrame.addSignalCaloHits(barrelName, &barrelHits);
  crossingFrame.addSignalCaloHits(endcapName, &endcapHits);

  EcalSimParameterMap parameterMap;
  //EBSimShape ecalShape(1.9181,4.52546,1.92921);
  EcalShape shape2(47.6683);

//  for(int i = 0; i < 100; ++i) {
//    std::cout << i << " " << ecalShape(i) << "  " << shape2(i) << std::endl;
//  }


  CaloHitResponse ecalResponse(&parameterMap, &shape2);


  // make pedestals for each of these
  EcalPedestals::Item barrelItem;
  // pedestals = 220 counts * 32 MeV/ADC = 7 GeV
  // noise = 40 MeV in barrel
  barrelItem.mean_x1 = 7.05;
  barrelItem.rms_x1 = 0.003;
  barrelItem.mean_x6 = 7.05;
  barrelItem.rms_x6 = 0.020;
  barrelItem.mean_x12 = 7.05;
  barrelItem.rms_x12 = 0.040;

  // noise is 150 MeV in endcaps
  EcalPedestals::Item endcapItem;
  endcapItem.mean_x1 = 7.05;
  endcapItem.rms_x1 = 0.038;
  endcapItem.mean_x6 = 7.05;
  endcapItem.rms_x6 = 0.075;
  endcapItem.mean_x12 = 7.05;
  endcapItem.rms_x12 = 0.150;

  EcalPedestals thePedestals;
  thePedestals.m_pedestals.insert(pair<int, EcalPedestals::Item>(barrelDetId.rawId(), barrelItem));
  thePedestals.m_pedestals.insert(pair<int, EcalPedestals::Item>(endcapDetId.rawId(), endcapItem));

  bool addNoise = true;
  EcalCoder coder(addNoise);
  EcalElectronicsSim electronicsSim(&parameterMap, &coder);
  coder.setPedestals(&thePedestals);

  CaloTDigitizer<EBDigitizerTraits> barrelDigitizer(&ecalResponse, &electronicsSim, addNoise);
  CaloTDigitizer<EEDigitizerTraits> endcapDigitizer(&ecalResponse, &electronicsSim, addNoise);
  barrelDigitizer.setDetIds(barrelDetIds);
  endcapDigitizer.setDetIds(endcapDetIds);

  auto_ptr<EBDigiCollection> barrelResult(new EBDigiCollection);
  auto_ptr<EEDigiCollection> endcapResult(new EEDigiCollection);

  MixCollection<PCaloHit> barrelHitCollection(&crossingFrame, barrelName);
  MixCollection<PCaloHit> endcapHitCollection(&crossingFrame, endcapName);

  barrelDigitizer.run(barrelHitCollection, *barrelResult);
  endcapDigitizer.run(endcapHitCollection, *endcapResult);

  // print out all the digis
  cout << "EB Frames" << endl;
  copy(barrelResult->begin(), barrelResult->end(), std::ostream_iterator<EBDataFrame>(std::cout, "\n"));

  cout << "EE Frames" << endl;
  copy(endcapResult->begin(), endcapResult->end(), std::ostream_iterator<EEDataFrame>(std::cout, "\n"));

  return 0;
}



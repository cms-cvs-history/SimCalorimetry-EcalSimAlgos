#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EEDataFrame.h"
#include "SimCalorimetry/CaloSimAlgos/interface/CaloHit.h"
#include "SimCalorimetry/CaloSimAlgos/interface/CaloHitResponse.h"
#include "SimCalorimetry/CaloSimAlgos/interface/CaloTDigitizer.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalSimParameterMap.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EcalShape.h"
#include "SimCalorimetry/EcalSimAlgos/interface/EBSimShape.h"
#include "CalibFormats/EcalObjects/interface/EcalCoder.h"
#include "CalibCalorimetry/EcalAlgos/interface/EcalDbServiceHardcode.h"
#include <vector>
#include<iostream>
#include<iterator>
using namespace std;
using namespace cms;

int main() {
  // make a silly little hit in each subdetector, which should
  // correspond to a 100 GeV particle
  EBDetId barrelDetId(1, 1);
  CaloHit barrelHit(barrelDetId, 100., 0., 0);

  EEDetId endcapDetId(1, 1, 1);
  CaloHit endcapHit(endcapDetId, 1000., 0., 0);

  vector<DetId> barrelDetIds, endcapDetIds;
  barrelDetIds.push_back(barrelDetId);
  endcapDetIds.push_back(endcapDetId);

  vector<CaloHit> barrelHits, endcapHits;
  barrelHits.push_back(barrelHit);
  endcapHits.push_back(endcapHit);

  EcalSimParameterMap parameterMap;
  EBSimShape ecalShape(1.9181,4.52546,1.92921);
  EcalShape shape2;

//  for(int i = 0; i < 100; ++i) {
//    std::cout << i << " " << ecalShape(i) << "  " << shape2(i) << std::endl;
//  }


  CaloHitResponse ecalResponse(&parameterMap, &shape2);

  EcalDbServiceHardcode calibrator;
  EcalCoder coder(&calibrator);

  CaloTDigitizer<EBDataFrame, EcalCoder> barrelDigitizer(&ecalResponse, &coder, barrelDetIds);
  CaloTDigitizer<EEDataFrame, EcalCoder> endcapDigitizer(&ecalResponse, &coder, endcapDetIds);

  auto_ptr<vector<EBDataFrame> > barrelResult(new vector<EBDataFrame>);
  auto_ptr<vector<EEDataFrame> > endcapResult(new vector<EEDataFrame>);

  barrelDigitizer.run(barrelHits, barrelResult);
  endcapDigitizer.run(endcapHits, endcapResult);

  // print out all the digis
  cout << "EB Frames" << endl;
  copy(barrelResult->begin(), barrelResult->end(), std::ostream_iterator<EBDataFrame>(std::cout, "\n"));

  cout << "EE Frames" << endl;
  copy(endcapResult->begin(), endcapResult->end(), std::ostream_iterator<EEDataFrame>(std::cout, "\n"));

return 0;
}



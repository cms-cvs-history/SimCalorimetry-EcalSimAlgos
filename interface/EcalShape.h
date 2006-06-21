#ifndef EcalSimAlgos_EcalShape_h
#define EcalSimAlgos_EcalShape_h

#include<vector>
#include<stdexcept>
#include "SimCalorimetry/CaloSimAlgos/interface/CaloVShape.h"
  
/**
   \class EcalShape
   \brief  shaper for Ecal
*/
class EcalShape : public CaloVShape
{
public:
  
  /// ctor
  EcalShape(double timePhase);
  /// dtor
  ~EcalShape(){}
  
  double operator () (double time_) const;
  void display () const;
  double derivative (double time_) const;
  
  double computeTimeOfMaximum() const;
  double computeT0() const;
  double computeRisingTime() const;

 private:

  int nsamp;
  int tconv;
  int nbin;
  std::vector<double> nt;
  std::vector<double> ntd;

  double threshold;
  int binstart;

};
  


#endif
  

#ifndef TDATA_H
#define TDATA_H

class tData{
 public:
  static const Int_t nMaxTrk = 100000;

  int nTrk_;
  Float_t trkPt_[nMaxTrk];
  Float_t trkEta_[nMaxTrk];
  Float_t trkPhi_[nMaxTrk];
};

#endif

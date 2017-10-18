#ifndef JDATA_H
#define JDATA_H

class jData{
 public:
  static const Int_t nMaxJet = 500;

  int nref_;
  Float_t jtpt_[nMaxJet];
  Float_t rawpt_[nMaxJet];
  Float_t jteta_[nMaxJet];
  Float_t jtphi_[nMaxJet];
};

#endif

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TDatime.h"
#include "TMath.h"

#include "include/checkExtension.h"
#include "include/jData.h"
#include "include/tData.h"
#include "include/applyXeJSON.h"

int jetSkim(const std::string inFileName, const std::string outFileName = "")
{
  std::vector<std::string> fileList;
  if(isExtRoot(inFileName)) fileList.push_back(inFileName);
  else if(isExtTxt(inFileName)){
    std::ifstream file(inFileName.c_str());
    std::string tempStr;
    while(std::getline(file, tempStr)){
      if(tempStr.size() == 0) continue;
      else if(!isExtRoot(tempStr)) continue;
      fileList.push_back(tempStr);
    }
    file.close();
  }

  std::string tempOutFileName = outFileName;
  if(tempOutFileName.size() == 0){
    tempOutFileName = inFileName;
    while(tempOutFileName.find("/") != std::string::npos){tempOutFileName.replace(0,tempOutFileName.find("/")+1,"");}
    if(tempOutFileName.find(".") != std::string::npos) tempOutFileName.replace(tempOutFileName.find("."), tempOutFileName.size()-tempOutFileName.find("."),"");
    TDatime* date = new TDatime();
    tempOutFileName = tempOutFileName + "_" + std::to_string(date->GetDate()) + ".root";
    delete date;
  }
  
  std::cout << tempOutFileName << std::endl;

  TFile* outFile_p = new TFile(tempOutFileName.c_str(), "RECREATE");
  TTree* jetSkimTree_p = new TTree("jetSkimTree", "jetSkimTree");

  unsigned int run_, lumi_;
  Int_t hiBin_;
  Float_t vz_;

  jData jetsIn;
  jData jetsOut;

  tData trksIn;
  tData trksOut;

  jetSkimTree_p->Branch("hiBin", &hiBin_, "hiBin/I");
  jetSkimTree_p->Branch("vz", &vz_, "vz/F");

  jetSkimTree_p->Branch("nref", &jetsOut.nref_, "nref/I");
  jetSkimTree_p->Branch("jtpt", jetsOut.jtpt_, "jtpt[nref]/F");
  jetSkimTree_p->Branch("rawpt", jetsOut.rawpt_, "rawpt[nref]/F");
  jetSkimTree_p->Branch("jtphi", jetsOut.jtphi_, "jtphi[nref]/F");
  jetSkimTree_p->Branch("jteta", jetsOut.jteta_, "jteta[nref]/F");

  jetSkimTree_p->Branch("nTrk", &trksOut.nTrk_, "nTrk/I");
  jetSkimTree_p->Branch("trkPt", trksOut.trkPt_, "trkPt[nref]/F");
  jetSkimTree_p->Branch("trkPhi", trksOut.trkPhi_, "trkPhi[nref]/F");
  jetSkimTree_p->Branch("trkEta", trksOut.trkEta_, "trkEta[nref]/F");


  const unsigned int nFiles = TMath::Min(10, (int)fileList.size());

  for(unsigned int fileIter = 0; fileIter < nFiles; ++fileIter){
    std::cout << "Processing file " << fileIter << "/" << fileList.size() << "..." << std::endl;

    TFile* inFile_p = new TFile(fileList.at(fileIter).c_str(), "READ");
    TTree* jetTree_p = (TTree*)inFile_p->Get("akPu4PFJetAnalyzer/t");
    TTree* trkTree_p = (TTree*)inFile_p->Get("ppTrack/trackTree");
    TTree* hiTree_p = (TTree*)inFile_p->Get("hiEvtAnalyzer/HiTree");

    jetTree_p->SetBranchStatus("*", 0);
    jetTree_p->SetBranchStatus("nref", 1);
    jetTree_p->SetBranchStatus("jtpt", 1);
    jetTree_p->SetBranchStatus("rawpt", 1);
    jetTree_p->SetBranchStatus("jtphi", 1);
    jetTree_p->SetBranchStatus("jteta", 1);

    jetTree_p->SetBranchAddress("nref", &jetsIn.nref_);
    jetTree_p->SetBranchAddress("jtpt", jetsIn.jtpt_);
    jetTree_p->SetBranchAddress("rawpt", jetsIn.rawpt_);
    jetTree_p->SetBranchAddress("jtphi", jetsIn.jtphi_);
    jetTree_p->SetBranchAddress("jteta", jetsIn.jteta_);

    trkTree_p->SetBranchStatus("*", 0);
    trkTree_p->SetBranchStatus("nTrk", 1);
    trkTree_p->SetBranchStatus("trkPt", 1);
    trkTree_p->SetBranchStatus("trkPhi", 1);
    trkTree_p->SetBranchStatus("trkEta", 1);

    trkTree_p->SetBranchAddress("nTrk", &trksIn.nTrk_);
    trkTree_p->SetBranchAddress("trkPt", trksIn.trkPt_);
    trkTree_p->SetBranchAddress("trkPhi", trksIn.trkPhi_);
    trkTree_p->SetBranchAddress("trkEta", trksIn.trkEta_);

    hiTree_p->SetBranchStatus("*", 0);
    hiTree_p->SetBranchStatus("run", 1);
    hiTree_p->SetBranchStatus("lumi", 1);
    hiTree_p->SetBranchStatus("hiBin", 1);
    hiTree_p->SetBranchStatus("vz", 1);

    hiTree_p->SetBranchAddress("run", &run_);
    hiTree_p->SetBranchAddress("lumi", &lumi_);
    hiTree_p->SetBranchAddress("hiBin", &hiBin_);
    hiTree_p->SetBranchAddress("vz", &vz_);

    for(Int_t entry = 0; entry < jetTree_p->GetEntries(); ++entry){
      jetTree_p->GetEntry(entry);
      trkTree_p->GetEntry(entry);
      hiTree_p->GetEntry(entry);

      if(!isGoodEvent(run_, lumi_)) continue;

      bool goodEvent = false;
      for(Int_t jIter = 0; jIter < jetsIn.nref_; ++jIter){
	if(TMath::Abs(jetsIn.jteta_[jIter]) > 2.) continue;
	if(jetsIn.rawpt_[jIter] < 70.) continue;

	goodEvent = true;
	break;
      }

      if(goodEvent){
	jetsOut.nref_ = 0;

	for(Int_t jIter = 0; jIter < jetsIn.nref_; ++jIter){
	  if(TMath::Abs(jetsIn.jteta_[jIter]) > 2.) continue;
	  if(jetsIn.rawpt_[jIter] < 30.) continue;
	  
	  jetsOut.jtpt_[jetsOut.nref_] = jetsIn.jtpt_[jIter];
	  jetsOut.rawpt_[jetsOut.nref_] = jetsIn.rawpt_[jIter];
	  jetsOut.jtphi_[jetsOut.nref_] = jetsIn.jtphi_[jIter];
	  jetsOut.jteta_[jetsOut.nref_] = jetsIn.jteta_[jIter];
	  jetsOut.nref_++;
	}

	trksOut.nTrk_ = 0;

	for(Int_t tIter = 0; tIter < trksIn.nTrk_; ++tIter){
	  if(TMath::Abs(trksIn.trkEta_[tIter]) > 2.4) continue;
	  if(trksIn.trkPt_[tIter] < 5.) continue;
	  
	  trksOut.trkPt_[trksOut.nTrk_] = trksIn.trkPt_[tIter];
	  trksOut.trkPhi_[trksOut.nTrk_] = trksIn.trkPhi_[tIter];
	  trksOut.trkEta_[trksOut.nTrk_] = trksIn.trkEta_[tIter];
	  trksOut.nTrk_++;
	}

	jetSkimTree_p->Fill();
      }
    }

    inFile_p->Close();
    delete inFile_p;
  }

  outFile_p->cd();
  jetSkimTree_p->Write("", TObject::kOverwrite);
  delete jetSkimTree_p;
  outFile_p->Close();
  delete outFile_p;

  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2 && argc != 3){
    std::cout << "Usage: ./jetSkim.exe <inFileName> <optOutFileName>" << std::endl;
    return 1;
  }

  int retVal = 0;
  if(argc == 2) retVal += jetSkim(argv[1]);
  else if(argc == 3) retVal += jetSkim(argv[1], argv[2]);
  return retVal;
}

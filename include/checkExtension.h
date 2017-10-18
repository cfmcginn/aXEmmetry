#ifndef CHECKEXTENSIOeN_H
#define CHECKEXTENSION_H

#include <string>

bool isExtGeneric(const std::string inFileName, const std::string ext)
{
  if(inFileName.size() < ext.size()) return false;
  if(inFileName.substr(inFileName.size()-ext.size(), ext.size()).find(ext) == std::string::npos) return false;
  return true;
}

bool isExtTxt(const std::string inFileName){return isExtGeneric(inFileName, ".txt");}
bool isExtRoot(const std::string inFileName){return isExtGeneric(inFileName, ".root");}

#endif

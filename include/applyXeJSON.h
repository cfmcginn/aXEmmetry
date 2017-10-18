#ifndef APPLYXEJSON_H
#define APPLYXEJSON_H

bool isGoodEvent(unsigned int run, unsigned int lumi)
{
  if(run == 304899){
    if(lumi >= 33 && lumi <= 200) return true;
  }
  else if(run == 304906){
    if(lumi >= 1 && lumi <= 731) return true;
  }

  return false;
}

#endif

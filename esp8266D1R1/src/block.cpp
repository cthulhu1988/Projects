#include "block.h"
#include <string>
#include <iostream>
using namespace std;

block::block(uint32_t nIndexIn, const string &sDataIn) : sData(sDataIn) {

}

string block::GetHash() {
  return sHash;
}


String block::CalculateHash() {
  String ss;
  ss += nIndex;  //+= sData  += sPrevHash;
  String hash = sha1(ss);

  return hash;


}

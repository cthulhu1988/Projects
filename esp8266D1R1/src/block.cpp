#include "block.h"
#include <string>

block::block(uint32_t nIndexIn, const String &sDataIn) : sData(sDataIn) {

}

String block::GetHash() {
  return CalculateHash();
}


String block::CalculateHash() {
  String ss;
  ss += nIndex;
  ss += sData;
  ss += sPrevHash;
  String hash = sha1(ss);

  return hash;


}

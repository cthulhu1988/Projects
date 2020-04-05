#include "block.h"
#include <string>

block::block(String nIndexIn, const String &sDataIn) : nIndex(nIndexIn), sData(sDataIn){

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

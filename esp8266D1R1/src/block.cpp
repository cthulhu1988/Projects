#include "block.h"
//#include <sha256.h>
#include <string>
#include <iostream>
using namespace std;


block::block(uint32_t nIndexIn, const string &sDataIn) : _nIndex(nIndexIn), _sData(sDataIn) {

_nNonce = -1;
//_tTime = time(nullptr);

}

string block::GetHash() {
  return _sHash;
}

void block::MineBlock(uint32_t nDifficulty) {
    cout << "Block mined: " << _sHash << endl;
}

inline string block::_CalculateHash() const {
  stringstream ss;
  ss << _nIndex << _sData << _nNonce << sPrevHash;


  return ss.str();//sha256(ss.str());
}

#ifndef BLOCK_H
#define BLOCK_H
#include <Arduino.h>
#include <Hash.h>
#include <cstdint>

class block {

  public:
    String sPrevHash;
    block(uint32_t nIndexIn, const String &sDataIn);
    String GetHash();

    uint32_t nIndex;
    String sData;
    String sHash;
    String CalculateHash();

};
#endif

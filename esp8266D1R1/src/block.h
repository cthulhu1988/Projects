#ifndef BLOCK_H
#define BLOCK_H
#include <Arduino.h>
#include <Hash.h>
#include <cstdint>

class block {

  public:
    String sPrevHash;
    block(String nIndexIn, const String &sDataIn);
    String GetHash();

    String nIndex;
    String sData;
    String sHash;
    String CalculateHash();

};
#endif

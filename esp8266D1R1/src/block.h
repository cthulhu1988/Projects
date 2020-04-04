#ifndef BLOCK_H
#define BLOCK_H
#include <Arduino.h>
#include <Hash.h>
#include <sstream>
#include <cstdint>
#include <iostream>

using namespace std;


class block {

  public:
    string sPrevHash;
    block(uint32_t nIndexIn, const string &sDataIn);
    string GetHash();

    uint32_t nIndex;
    string sData;
    string sHash;
    String CalculateHash();

};
#endif

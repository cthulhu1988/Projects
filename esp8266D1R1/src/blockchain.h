#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <iostream>
#include <cstdint>
#include <vector>
#include "block.h"
#include <Arduino.h>
#include <Hash.h>

using namespace std;
class blockchain {

  public:
    blockchain();
    void AddBlock(block bNew);
    void printChain();

  private:
    vector<block> vChain;
    block GetLastBlock() const;
};


#endif

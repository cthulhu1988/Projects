#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <cstdint>
#include <vector>
#include "block.h"
#include <Arduino.h>
#include <Hash.h>

using namespace std;
class blockchain {

  public:
    blockchain();
    blockchain(String nodenum);
    void AddBlock(block bNew);
    void printChain();
    String GetStringChain();
    String GetLastRecord();

  private:
    vector<block> vChain;
    block GetLastBlock() const;
};


#endif

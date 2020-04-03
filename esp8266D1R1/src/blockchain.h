#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <iostream>
#include <cstdint>
#include <vector>
#include "block.h"
using namespace std;

class blockchain {

  public:
    blockchain();
    void AddBlock(block bNew);

  private:
    uint32_t _nDifficulty;
    vector<block> _vChain;
    block _GetLastBlock() const;
};


#endif

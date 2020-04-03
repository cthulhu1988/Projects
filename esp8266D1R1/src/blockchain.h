#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <cstdint>
#include <vector>
#include "block.h"
using namespace std;

class Blockchain {

  public:
    Blockchain();
    void AddBlock(block bNew);

  private:
    uint32_t _nDifficulty;
    vector<block> _vChain;
    block _GetLastBlock() const;
};


#endif

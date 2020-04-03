#include "blockchain.h"



blockchain::blockchain() {
    _vChain.emplace_back(block(0, "Genesis Block"));
    _nDifficulty = 6;
}


void blockchain::AddBlock(block bNew){

}

#include "blockchain.h"

blockchain::blockchain() {
    vChain.emplace_back(block(0, "GENESISBLOCK"));
}

void blockchain::AddBlock(block bNew){
  bNew.sPrevHash = GetLastBlock().GetHash();
  vChain.push_back(bNew);
}

block blockchain::GetLastBlock() const{
  return vChain.back();
}


void blockchain::printChain()
{
  for (size_t i = 0; i < vChain.size(); i++) {
    Serial.print("BlockCount: ");
    Serial.println(i);

    Serial.print("Data: ");
    Serial.println(vChain.at(i).sData.c_str());

    Serial.print("Hash");
    Serial.println(vChain.at(i).sPrevHash.c_str());

  }
}

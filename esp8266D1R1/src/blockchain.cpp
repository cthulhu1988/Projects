#include "blockchain.h"

blockchain::blockchain() {
    block genBlock = block(0, "GENBLOCK");
    genBlock.sPrevHash = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    genBlock.sHash = genBlock.CalculateHash();
    vChain.emplace_back(genBlock);

}

void blockchain::AddBlock(block bNew){
  bNew.sPrevHash = GetLastBlock().GetHash();
  bNew.sHash = bNew.GetHash();
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

    Serial.print("Previous Hash: ");
    Serial.println(vChain.at(i).sPrevHash.c_str());

    Serial.print("Data: ");
    Serial.println(vChain.at(i).sData.c_str());

    Serial.print("This Block's Hash: ");
    Serial.println(vChain.at(i).sHash.c_str());

    Serial.println();

  }
}


String blockchain::GetStringChain()
{
  String wholeChain ="";
  for (size_t i = 0; i < vChain.size(); i++) {
    wholeChain += '{';
    wholeChain += (vChain.at(i).sPrevHash.c_str());
    wholeChain += '%';
    wholeChain += (vChain.at(i).sData.c_str());
    wholeChain += '%';
    wholeChain += (vChain.at(i).sHash.c_str());
    wholeChain += '}';
  }
  return wholeChain;
}

String blockchain::GetLastRecord()
{
    String wholeChain = "{";
    wholeChain += (vChain.back().sPrevHash.c_str());
    wholeChain += " <> ";
    wholeChain += (vChain.back().sData.c_str());
    wholeChain += " <> ";
    wholeChain += (vChain.back().sHash.c_str());
    wholeChain += '}';

  return wholeChain;
}

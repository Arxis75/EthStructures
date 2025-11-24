#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <EthStructures/EthComposite.h>

int main(int argc, char *argv[])
{
    //["0x13","0x077fe0a5c0","0x01d8a8","0x640c65fd089d0b758eba08ae0baf80f2017ec4ff","0x5a34a38fc00a0000","0x","0x1b","0x08dd1868e88ab3ca6b4e4548ca9d7307c8c6257c5a6e655f6d32e48d0aaad462","0x421e5ffb987c7efa0d36d3ba18d82b743f5c69988209ff4cbba164757af28e13"]
    RLPByteSet txs("0xf86Ff86d1385077fe0a5c08301d8a894640c65fd089d0b758eba08ae0baf80f2017ec4ff885a34a38fc00a0000801ba008dd1868e88ab3ca6b4e4548ca9d7307c8c6257c5a6e655f6d32e48d0aaad462a0421e5ffb987c7efa0d36d3ba18d82b743f5c69988209ff4cbba164757af28e13");

    BlockTransactions Txs;
    Txs.parse(txs);
    auto calculated_Txs_hash = Txs.hash();

    return 0;
}

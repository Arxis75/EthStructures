#include <EthStructures/EthComposite.h>

using std::make_unique;

const Block* BlockChain::newBlockFromRawRLP(ByteSet<BYTE> &b) {
    ByteSet payload = b.RLPparse();         //Removes the top brackets
    auto block = make_unique<Block>();
    block->RLPparse(payload);
    auto result= block.get();
    m_blocks.insert({block->getHeight(), std::move(block)});
    return result;
} 

void Block::RLPparse(ByteSet<BYTE> &b) {   
    create<BlockHeader>(b);
    if(auto h = getHeader(); h)
        if(auto f = h->get<RLPListField>(8); f)
            setHeight(f->getValue().asInteger());
    create<BlockTransactions>(b);
    create<BlockUncles>(b);
    create<BlockWithdrawals>(b);
}

void BlockTransaction::RLPparse(ByteSet<BYTE> &b)
{
    switch(getType()) {
        case 1: //EIP-2930 (11 fields)
            //chain_id, nonce, gas_price, gas_limit, to, value, data, access_list, signature_y_parity, signature_r, signature_s
            for(uint i = 0; i < 7; i++) create<RLPListField>(b);
            create<BlockAccessLists>(b);
            for(uint i = 8; i < 11; i++) create<RLPListField>(b);
            break;
        case 2: //EIP-1559 (12 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, destination, amount, data, access_list, signature_y_parity, signature_r, signature_s
            for(uint i = 0; i < 8; i++) create<RLPListField>(b);
            create<BlockAccessLists>(b);
            for(uint i = 9; i < 12; i++) create<RLPListField>(b);
            break;
        case 3: //EIP-4844 (14 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, to, value, data, access_list, max_fee_per_blob_gas, blob_versioned_hashes, y_parity, r, s
            for(uint i = 0; i < 8; i++) create<RLPListField>(b);
            create<BlockAccessLists>(b);
            create<RLPListField>(b);
            create<BlockBlobVersionHashes>(b);
            for(uint i = 11; i < 14; i++) create<RLPListField>(b);
            break;
        case 4: //EIP-7702 (13 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, to, value, data, access_list, authorization_list, signature_y_parity, signature_r, signature_s
            for(uint i = 0; i < 8; i++) create<RLPListField>(b);
            create<BlockAccessLists>(b);
            create<BlockAuthorizationList>(b);
            for(uint i = 10; i < 13; i++) create<RLPListField>(b);
            break;
        default: //Legacy/EIP-155 (9 fields)
            //nonce, gas_price, gas_limit, to, value, data, v, signature_r, signature_s
            for(uint i = 0; i < 9; i++) create<RLPListField>(b);
            break;
    }
}
#include <EthComposite.h>
#include <System.h>

template<typename T>
void EthContainer::parseItem(ByteSet<BYTE> &b) {
        bool has_list_header = b.hasRLPListHeader();
        ByteSet payload = b.RLPparse();
        T* item = new T();
        if(auto typed_item = dynamic_cast<EthTypedContainer*>(item); typed_item && !has_list_header) { 
            typed_item->setType(payload.pop_front_elem());
            payload = payload.RLPparse();
        }
        item->RLPparse(payload);
        item->setParent(this);
        push_back(item);  //implicit call of unique_ptr ctor
}

const ByteSet<BYTE> EthTypedContainer::RLPserialize() const {
    ByteSet result = EthContainer::RLPserialize();
    if(uint8_t type = getType(); type) {
        result.push_front_elem(type);
        result = result.RLPserialize(false);
    }
    return result;
}

const Block* BlockChain::newBlockFromRawRLP(ByteSet<BYTE> &b) {
    ByteSet payload = b.RLPparse();         //Removes the top brackets
    auto block = make_unique<Block>();
    block->RLPparse(payload);
    auto result= block.get();
    m_blocks.insert({block->getHeight(), std::move(block)});
    return result;
} 

void Block::RLPparse(ByteSet<BYTE> &b) {
        parseItem<BlockHeader>(b);
        parseItem<BlockTransactions>(b);
        parseItem<BlockUncles>(b);
        parseItem<BlockWithdrawals>(b);

        if(auto h = getHeader(); h)
            if(auto f = h->get<ByteSetField>(8); f)
                setHeight(f->getValue().asInteger());
}

void BlockTransaction::RLPparse(ByteSet<BYTE> &b)
{
    switch(getType()) {
        case 1: //EIP-2930 (11 fields)
            //chain_id, nonce, gas_price, gas_limit, to, value, data, access_list, signature_y_parity, signature_r, signature_s
            for(uint i = 0; i < 7; i++) parseItem<ByteSetField>(b);
            parseItem<BlockAccessLists>(b);
            for(uint i = 8; i < 11; i++) parseItem<ByteSetField>(b);
            break;
        case 2: //EIP-1559 (12 fields)
            cout << b.asString() << endl;
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, destination, amount, data, access_list, signature_y_parity, signature_r, signature_s
            for(uint i = 0; i < 8; i++) parseItem<ByteSetField>(b);
            parseItem<BlockAccessLists>(b);
            for(uint i = 9; i < 12; i++) parseItem<ByteSetField>(b);
            break;
        case 3: //EIP-4844 (14 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, to, value, data, access_list, max_fee_per_blob_gas, blob_versioned_hashes, y_parity, r, s
            for(uint i = 0; i < 8; i++) parseItem<ByteSetField>(b);
            parseItem<BlockAccessLists>(b);
            parseItem<ByteSetField>(b);
            parseItem<BlockBlobVersionHashes>(b);
            for(uint i = 11; i < 14; i++) parseItem<ByteSetField>(b);
            break;
        case 4: //EIP-7702 (13 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, to, value, data, access_list, authorization_list, signature_y_parity, signature_r, signature_s
            for(uint i = 0; i < 8; i++) parseItem<ByteSetField>(b);
            parseItem<BlockAccessLists>(b);
            parseItem<BlockAuthorizationList>(b);
            for(uint i = 10; i < 13; i++) parseItem<ByteSetField>(b);
            break;
        default: //Legacy/EIP-155 (9 fields)
            //nonce, gas_price, gas_limit, to, value, data, v, signature_r, signature_s
            parseAllItems<ByteSetField>(b);
            break;
    }
}
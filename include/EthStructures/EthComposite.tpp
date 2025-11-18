#include <EthStructures/EthComposite.h>

using std::make_unique;

const Block* BlockChain::newBlockFromRawRLP(ByteSet<BYTE> &b) {
    ByteSet payload = b.parse();         //Removes the top brackets
    auto block = make_unique<Block>();
    block->parse(payload);
    auto result= block.get();
    m_blocks.insert({block->getHeight(), std::move(block)});
    return result;
}

IComponent* Block::newChild(uint creation_index) {
    switch(creation_index) {
        case 0:
            return new BlockHeader;
        break;
        case 1:
            return new BlockTransactions;
        break;
        case 2:
            return new BlockUncles;
        break;
        case 3:
            return new BlockWithdrawals;
        break;
        default:
            return nullptr;
        break;
    }
}

void Block::parse(ByteSet<BYTE> &b) {
    VectorNode::parse(b);
    if(auto h = getHeader(); h)
        if(auto f = h->get<ByteSet<BYTE>>(8); f)
            setHeight(f->asInteger());
}

IComponent* BlockAccessList::newChild(uint creation_index) {
    switch(creation_index) {
        case 0:
            return new ByteSet<BYTE>;
        break;
        case 1:
            return new BlockStorageKeys;
        break;
        default:
            return nullptr;
        break;
    }
}

IComponent* BlockTransaction::newChild(uint creation_index)
{
    switch(getTyped()) {
        case 1: //EIP-2930 (11 fields)
            //chain_id, nonce, gas_price, gas_limit, to, value, data, access_list, signature_y_parity, signature_r, signature_s
            switch(creation_index) {
                case 7:
                    return new BlockAccessLists;
                break;
                default:
                    return new ByteSet<BYTE>;
                break;
            }
            break;
        case 2: //EIP-1559 (12 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, destination, amount, data, access_list, signature_y_parity, signature_r, signature_s
            switch(creation_index) {
                case 8:
                    return new BlockAccessLists;
                break;
                default:
                    return new ByteSet<BYTE>;
                break;
            }
            break;
        case 3: //EIP-4844 (14 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, to, value, data, access_list, max_fee_per_blob_gas, blob_versioned_hashes, y_parity, r, s
            switch(creation_index) {
                case 8:
                    return new BlockAccessLists;
                break;
                case 10:
                    return new BlockBlobVersionHashes;
                break;
                default:
                    return new ByteSet<BYTE>;
                break;
            }
            break;
        case 4: //EIP-7702 (13 fields)
            //chain_id, nonce, max_priority_fee_per_gas, max_fee_per_gas, gas_limit, to, value, data, access_list, authorization_list, signature_y_parity, signature_r, signature_s
            switch(creation_index) {
                case 8:
                    return new BlockAccessLists;
                break;
                case 9:
                    return new BlockAuthorizationList;
                break;
                default:
                    return new ByteSet<BYTE>;
                break;
            }
            break;
        default: //Legacy/EIP-155 (9 fields)
            //nonce, gas_price, gas_limit, to, value, data, v, signature_r, signature_s
            return new ByteSet<BYTE>;
            break;
    }
}
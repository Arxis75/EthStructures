#pragma once

#include <ByteSet/ByteSetComposite.h>
#include <map>

struct BlockHeader : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<ByteSetField>(b); }
};
struct BlockAuthorization : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<ByteSetField>(b); }
};
struct BlockAuthorizationList : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockAuthorization>(b); }
};
struct BlockBlobVersionHashes : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<ByteSetField>(b); }
};
struct BlockStorageKeys : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<ByteSetField>(b); }
};
struct BlockAccessList : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { create<ByteSetField>(b); create<BlockStorageKeys>(b); }
};
struct BlockAccessLists : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockAccessList>(b); }
};
struct BlockTransaction : public TypedByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override;
};
struct BlockTransactions : public ByteSetComposite { 
     virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockTransaction>(b); }
};
struct BlockUncles : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockHeader>(b); }
};
struct BlockWithdrawal : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<ByteSetField>(b); }
};
struct BlockWithdrawals : public ByteSetComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockWithdrawal>(b); }
};

class Block : public ByteSetComposite {
    public:
        Block() : ByteSetComposite(), m_block_height(-1) {}
        virtual ~Block() = default;
      
        virtual void RLPparse(ByteSet<BYTE> &b) override;

        const BlockHeader* getHeader() const { return get<const BlockHeader>(0); }
        const BlockTransactions* getTransactions() const { return get<const BlockTransactions>(1); }
        const BlockUncles* getUncles() const { return get<const BlockUncles>(2); }
        const BlockWithdrawals* getWithdrawals() const { return get<const BlockWithdrawals>(3); }

        inline uint64_t getHeight() const { return m_block_height; }

    protected:
        inline void setHeight(uint64_t height) { m_block_height = height; }

    private:
        int64_t m_block_height;
};

class BlockChain {
    public:
        BlockChain() = default;

        const Block* newBlockFromRawRLP(ByteSet<BYTE> &b);
        inline const Block* getBlock(uint64_t block_height) const { return m_blocks.find(block_height)->second.get(); };

    private:
        std::map<uint64_t, unique_ptr<const Block>> m_blocks;
};

using Header = const BlockHeader;
using Transactions = const BlockTransactions;
using Transaction = const BlockTransaction;
using Uncles = const BlockUncles;
using Withdrawals = const BlockWithdrawals;
using Withdrawal = const BlockWithdrawal;
using Field = const ByteSetField;

#include <EthStructures/EthComposite.tpp>

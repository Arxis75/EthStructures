#pragma once

#include <ByteSet/Composite.h>
#include <map>

struct BlockHeader : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<RLPListField>(b); }
};
struct BlockAuthorization : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<RLPListField>(b); }
};
struct BlockAuthorizationList : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockAuthorization>(b); }
};
struct BlockBlobVersionHashes : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<RLPListField>(b); }
};
struct BlockStorageKeys : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<RLPListField>(b); }
};
struct BlockAccessList : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { create<RLPListField>(b); create<BlockStorageKeys>(b); }
};
struct BlockAccessLists : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockAccessList>(b); }
};
struct BlockTransaction : public TypedRLPListComposite, virtual public ITrieable {
    //****************************** ITrieable Interface *******************************
    inline virtual bool isEmpty() const override{ return getChildrenCount() == 0; };
    inline virtual void clear() override { TypedRLPListComposite::reset(); }
    inline virtual ByteSet<BYTE> serialize() const override { return RLPSerialize(); };
    //**********************************************************************************
    virtual void RLPparse(ByteSet<BYTE> &b) override;
};
struct BlockTransactions : public RLPListComposite { 
     virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockTransaction>(b); }
};
struct BlockUncles : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockHeader>(b); }
};
struct BlockWithdrawal : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<RLPListField>(b); }
};
struct BlockWithdrawals : public RLPListComposite {
    virtual void RLPparse(ByteSet<BYTE> &b) override { createAll<BlockWithdrawal>(b); }
};

class Block : public RLPListComposite {
    public:
        Block() : RLPListComposite(), m_block_height(-1) {}
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
using Field = const RLPListField;

#include <EthStructures/EthComposite.tpp>

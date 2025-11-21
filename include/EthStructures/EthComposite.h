#pragma once

#include <ByteSet/VectorNode.h>
#include <ByteSet/TrieNode.h>
#include <map>

struct BlockHeader : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new ByteSet<BYTE>; }
};
struct BlockAuthorization : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new ByteSet<BYTE>; }
};
struct BlockAuthorizationList : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new BlockAuthorization; }
};
struct BlockBlobVersionHashes : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new ByteSet<BYTE>; }
};
struct BlockStorageKeys : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new ByteSet<BYTE>; }
};
struct BlockAccessList : public VectorNode {
    virtual IComponent* newChild(uint creation_index = 0) override;
};
struct BlockAccessLists : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new BlockAccessList; }
};
struct BlockTransaction : public VectorNode {
    virtual IComponent* newChild(uint creation_index = 0) override;
};
struct BlockTransactions : public TrieRoot<BlockTransaction> {
    virtual IComponent* newChild(uint = 0) override { return new BlockTransaction; }
};
struct BlockUncles : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new BlockHeader; }
};
struct BlockWithdrawal : public VectorNode {
    virtual IComponent* newChild(uint = 0) override { return new ByteSet<BYTE>; }
};
struct BlockWithdrawals : public TrieRoot<BlockWithdrawal> {
    virtual IComponent* newChild(uint = 0) override { return new BlockWithdrawal; }
};

class Block : public VectorNode {
    public:
        Block() : VectorNode(), m_block_height(-1) {}
        virtual ~Block() = default;
      
        virtual IComponent* newChild(uint creation_index = 0) override;
        void parse(ByteSet<BYTE> &b) override;

        const BlockHeader* getHeader() const { return get<BlockHeader>(0); }
        const BlockTransactions* getTransactions() const { return get<const BlockTransactions>(1); }
        const BlockUncles* getUncles() const { return get<BlockUncles>(2); }
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
using Field = const ByteSet<BYTE>;
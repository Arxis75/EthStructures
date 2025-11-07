#pragma once

#include <ByteSetComposite.h>
#include <Tools.h>
#include <map>

class EthContainer : public ByteSetComposite {
    public:
        virtual ~EthContainer() = default;

        virtual void RLPparse(ByteSet<BYTE> &b) override = 0;

        template<typename T>
            void parseItem(ByteSet<BYTE> &b);
        template<typename T>
            inline void parseAllItems(ByteSet<BYTE> &b) { while(b.byteSize()) parseItem<T>(b); }

        template<typename T>
            inline const T* get(uint64_t index) const { return (index < getChildrenCount() ? dynamic_cast<const T*>(getChildAt(index)) : nullptr); }
    
    protected:
        EthContainer() = default;
};
class EthTypedContainer : public EthContainer {
    public:
        virtual ~EthTypedContainer() = default;

        virtual const ByteSet<BYTE> RLPserialize() const override;

        inline uint64_t getType() const { return m_type; }
        inline void setType(uint64_t type) { m_type = type; }
    
    protected:
        EthTypedContainer() = default;

    private:
        int64_t m_type;
};

struct BlockHeader : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<ByteSetField>(b); }
};
struct BlockAuthorization : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<ByteSetField>(b); }
};
struct BlockAuthorizationList : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<BlockAuthorization>(b); }
};
struct BlockBlobVersionHashes : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<ByteSetField>(b); }
};
struct BlockStorageKeys : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<ByteSetField>(b); }
};
struct BlockAccessList : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseItem<ByteSetField>(b); parseItem<BlockStorageKeys>(b); }
};
struct BlockAccessLists : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<BlockAccessList>(b); }
};
struct BlockTransaction : public EthTypedContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override;
};
struct BlockTransactions : public EthContainer { 
     virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<BlockTransaction>(b); }
};
struct BlockUncles : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<BlockHeader>(b); }
};
struct BlockWithdrawal : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<ByteSetField>(b); }
};
struct BlockWithdrawals : public EthContainer {
    virtual void RLPparse(ByteSet<BYTE> &b) override { parseAllItems<BlockWithdrawal>(b); }
};

class Block : public EthContainer {
    public:
        Block() : EthContainer(), m_block_height(-1) {}
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

#include <EthComposite.tpp>

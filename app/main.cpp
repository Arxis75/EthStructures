//#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <EthStructures/EthComposite.h>

#include <evmc/evmc.h>
#include <evmc/loader.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <cstring>

// --------------------
// Simplified Trie
// --------------------
struct Trie {
    std::unordered_map<std::string, std::string> data;
    void set(const std::string &key, const std::string &value) { data[key] = value; }
    std::string get(const std::string &key) { return data.count(key) ? data[key] : ""; }
    void remove(const std::string &key) { data.erase(key); }
    Trie clone() const { Trie t; t.data = data; return t; }
    std::string compute_root() {
        std::string concat;
        for (auto &kv : data) concat += kv.first + kv.second;
        return concat.substr(0, std::min<size_t>(32, concat.size()));
    }
};

// --------------------
// Log, Account, Receipt
// --------------------
struct Log {
    evmc_address address;
    std::vector<evmc_bytes32> topics;
    std::vector<uint8_t> data;
};

struct Account {
    uint64_t nonce = 0;
    uint64_t balance = 0;
    std::vector<uint8_t> code;
    Trie storage;
};

struct Receipt {
    uint8_t status;
    uint64_t cumulativeGas;
    std::vector<Log> logs;
};

// --------------------
// TrieHost with staging
// --------------------
struct TrieHost {
    std::unordered_map<std::string, Account> state_accounts;
    std::unordered_map<std::string, Account> staged_accounts;
    std::vector<Log> logs;

    void begin_tx() { staged_accounts = state_accounts; logs.clear(); }
    void commit_tx() { state_accounts = staged_accounts; }
    void revert_tx() { staged_accounts = state_accounts; }

    Account& get_account(const std::string &addr) { return staged_accounts[addr]; }

    void create_contract(const std::string &addr, const std::vector<uint8_t>& code) {
        Account& acct = staged_accounts[addr];
        acct.code = code;
        acct.balance = 100; // assign initial balance for demo
        acct.nonce = 0;
        acct.storage = Trie();
        std::cout << "CREATE contract: " << addr << "\n";
    }

    void selfdestruct(const std::string &addr, const std::string &beneficiary) {
        Account& target = staged_accounts[addr];
        Account& ben = staged_accounts[beneficiary];
        ben.balance += target.balance;
        staged_accounts.erase(addr);
        std::cout << "SELFDESTRUCT: " << addr << " -> " << beneficiary << "\n";
    }
};

// --------------------
// EVMC callbacks
// --------------------
evmc_storage_status host_get_storage(evmc_host_context* ctx,
                                     const evmc_address* addr,
                                     const evmc_bytes32* key,
                                     evmc_bytes32* value) noexcept
{
    TrieHost* host = ctx->host;
    std::string addr_str(reinterpret_cast<const char*>(addr->bytes), 20);
    std::string key_str(reinterpret_cast<const char*>(key->bytes), 32);
    std::string val = host->get_account(addr_str).storage.get(key_str);
    std::memset(value->bytes, 0, 32);
    std::memcpy(value->bytes, val.c_str(), std::min<size_t>(32, val.size()));
    return EVMC_STORAGE_UNCHANGED;
}

evmc_storage_status host_set_storage(evmc_host_context* ctx,
                                     const evmc_address* addr,
                                     const evmc_bytes32* key,
                                     const evmc_bytes32* value) noexcept
{
    TrieHost* host = ctx->host;
    std::string addr_str(reinterpret_cast<const char*>(addr->bytes), 20);
    std::string key_str(reinterpret_cast<const char*>(key->bytes), 32);
    std::string val(reinterpret_cast<const char*>(value->bytes), 32);
    host->get_account(addr_str).storage.set(key_str, val);
    return EVMC_STORAGE_MODIFIED;
}

bool host_account_exists(evmc_host_context* ctx, const evmc_address* addr) noexcept {
    TrieHost* host = ctx->host;
    std::string addr_str(reinterpret_cast<const char*>(addr->bytes), 20);
    return host->staged_accounts.count(addr_str) != 0;
}

evmc_result host_emit_log(evmc_host_context* ctx,
                          const evmc_address* addr,
                          const uint8_t* data, size_t data_size,
                          const evmc_bytes32 topics[], size_t topics_count) noexcept
{
    TrieHost* host = ctx->host;
    Log log;
    log.address = *addr;
    for (size_t i = 0; i < topics_count; ++i)
        log.topics.push_back(topics[i]);
    log.data.assign(data, data + data_size);
    host->logs.push_back(log);
    return EVMC_SUCCESS;
}

evmc_bytes32 host_create(evmc_host_context* ctx,
                         const evmc_address*, const evmc_bytes32*,
                         const uint8_t* code, size_t code_size,
                         const evmc_address* addr) noexcept
{
    TrieHost* host = ctx->host;
    std::vector<uint8_t> code_vec(code, code + code_size);
    std::string addr_str(reinterpret_cast<const char*>(addr->bytes), 20);
    host->create_contract(addr_str, code_vec);
    evmc_bytes32 hash{};
    return hash;
}

void host_selfdestruct(evmc_host_context* ctx,
                       const evmc_address* addr,
                       const evmc_address* beneficiary) noexcept
{
    TrieHost* host = ctx->host;
    std::string addr_str(reinterpret_cast<const char*>(addr->bytes), 20);
    std::string ben_str(reinterpret_cast<const char*>(beneficiary->bytes), 20);
    host->selfdestruct(addr_str, ben_str);
}

// --------------------
// Demo transaction lifecycle
// --------------------
int main(int argc, char *argv[])
{
    TrieHost host;

    // Setup VM
    auto vm = evmc_load("evmone");
    if (!vm) { std::cerr << "Failed to load EVMOne\n"; return 1; }

    evmc_host_interface host_iface{};
    host_iface.get_storage = host_get_storage;
    host_iface.set_storage = host_set_storage;
    host_iface.account_exists = host_account_exists;
    host_iface.emit_log = host_emit_log;
    host_iface.create = host_create;
    host_iface.selfdestruct = host_selfdestruct;

    evmc_host_context ctx{ &host };

    // Transaction: arbitrary bytecode
    std::vector<uint8_t> bytecode = {0x60,0x01,0x60,0x02,0x01}; // ADD 1+2
    evmc_message msg{};
    msg.kind = EVMC_CALL;
    msg.gas = 1000000;
    msg.input_data = bytecode.data();
    msg.input_size = bytecode.size();

    Trie receipt_trie;
    uint64_t cumulative_gas = 0;

    // Begin transaction
    host.begin_tx();
    evmc_result result = evmc_execute(vm, &host_iface, &ctx, EVMC_MAX_REVISION, &msg);

    // Build receipt
    Receipt receipt{};
    receipt.status = (result.status_code == EVMC_SUCCESS) ? 1 : 0;
    uint64_t gas_used = msg.gas - result.gas_left;
    cumulative_gas += gas_used;
    receipt.cumulativeGas = cumulative_gas;
    receipt.logs = host.logs;

    // Commit or revert
    if (result.status_code == EVMC_SUCCESS) host.commit_tx();
    else host.revert_tx();

    // Insert receipt into trie
    std::string rlp_receipt = "rlp_placeholder";
    receipt_trie.set(std::string(1,0), rlp_receipt);

    // --------------------
    // Demonstrate CREATE + SSTORE + LOG + SELFDESTRUCT manually
    // --------------------
    host.begin_tx();

    // CREATE contract at addr "contract1"
    std::string contract_addr = "contract1_contract1_contract1";
    std::vector<uint8_t> code = {0x60,0x00};
    host.create_contract(contract_addr, code);

    // SSTORE slots
    host.get_account(contract_addr).storage.set("slot1", "value1");
    host.get_account(contract_addr).storage.set("slot2", "value2");

    // Emit log manually
    Log log;
    log.address = {}; // zero address for demo
    log.data = {'H','e','l','l','o'};
    host.logs.push_back(log);

    // SELFDESTRUCT, transfer to beneficiary "beneficiary1"
    std::string beneficiary_addr = "beneficiary1_beneficiary1";
    host.staged_accounts[beneficiary_addr] = Account{0,0}; // create beneficiary
    host.selfdestruct(contract_addr, beneficiary_addr);

    host.commit_tx();

    // Print final state
    std::cout << "Beneficiary balance: " << host.state_accounts[beneficiary_addr].balance << "\n";
    std::cout << "Contract exists? " << (host.state_accounts.count(contract_addr) ? "yes" : "no") << "\n";
    std::cout << "Receipts root: " << receipt_trie.compute_root() << "\n";
}
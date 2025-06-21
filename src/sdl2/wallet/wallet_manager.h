// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_WALLET_WALLET_MANAGER_H
#define GOTHAM_SDL2_WALLET_WALLET_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

/**
 * Transaction data structure
 */
struct Transaction {
    std::string txid;
    std::string address;
    double amount;
    int confirmations;
    std::string category; // "send", "receive", "generate"
    std::string label;
    int64_t time;
    std::string comment;
    bool is_coinbase;
    
    Transaction() : amount(0.0), confirmations(0), time(0), is_coinbase(false) {}
};

/**
 * Address book entry
 */
struct AddressBookEntry {
    std::string address;
    std::string label;
    std::string purpose; // "send", "receive"
    
    AddressBookEntry() = default;
    AddressBookEntry(const std::string& addr, const std::string& lbl, const std::string& purp = "send")
        : address(addr), label(lbl), purpose(purp) {}
};

/**
 * Wallet balance information
 */
struct WalletBalance {
    double confirmed;
    double unconfirmed;
    double immature;
    double total;
    
    WalletBalance() : confirmed(0.0), unconfirmed(0.0), immature(0.0), total(0.0) {}
};

/**
 * Network information
 */
struct NetworkInfo {
    std::string network; // "main", "test", "regtest"
    int connections;
    int blocks;
    double difficulty;
    bool is_syncing;
    double sync_progress;
    
    NetworkInfo() : connections(0), blocks(0), difficulty(0.0), is_syncing(false), sync_progress(0.0) {}
};

/**
 * Real Bitcoin wallet manager
 * This would interface with Bitcoin Core or other wallet backends
 */
class WalletManager {
public:
    WalletManager();
    ~WalletManager();

    // Initialization
    bool Initialize();
    void Shutdown();

    // Wallet operations
    bool CreateWallet(const std::string& name, const std::string& passphrase = "");
    bool LoadWallet(const std::string& name);
    bool UnloadWallet();
    bool IsWalletLoaded() const { return m_wallet_loaded; }
    std::string GetWalletName() const { return m_wallet_name; }

    // Balance and transactions
    WalletBalance GetBalance();
    std::vector<Transaction> GetTransactions(int count = 100, int skip = 0);
    Transaction GetTransaction(const std::string& txid);

    // Sending
    std::string SendToAddress(const std::string& address, double amount, const std::string& comment = "");
    double EstimateFee(const std::string& address, double amount);
    bool ValidateAddress(const std::string& address);

    // Receiving
    std::string GetNewAddress(const std::string& label = "");
    std::string GetReceiveAddress();
    std::vector<std::string> GetAddresses();

    // Address book
    std::vector<AddressBookEntry> GetAddressBook();
    bool AddAddressBookEntry(const AddressBookEntry& entry);
    bool RemoveAddressBookEntry(const std::string& address);
    bool UpdateAddressLabel(const std::string& address, const std::string& label);

    // Network and sync
    NetworkInfo GetNetworkInfo();
    bool IsConnected() const { return m_connected; }
    bool IsSynced() const { return m_synced; }

    // Security
    bool EncryptWallet(const std::string& passphrase);
    bool ChangePassphrase(const std::string& old_passphrase, const std::string& new_passphrase);
    bool UnlockWallet(const std::string& passphrase, int timeout = 60);
    bool LockWallet();
    bool IsWalletLocked() const { return m_wallet_locked; }

    // Backup and restore
    bool BackupWallet(const std::string& destination);
    bool ImportPrivateKey(const std::string& private_key, const std::string& label = "");
    std::string DumpPrivateKey(const std::string& address);

    // Callbacks for GUI updates
    void SetBalanceUpdateCallback(std::function<void(const WalletBalance&)> callback) { m_balance_callback = callback; }
    void SetTransactionCallback(std::function<void(const Transaction&)> callback) { m_transaction_callback = callback; }
    void SetSyncProgressCallback(std::function<void(double)> callback) { m_sync_callback = callback; }

private:
    bool m_initialized{false};
    bool m_wallet_loaded{false};
    bool m_connected{false};
    bool m_synced{false};
    bool m_wallet_locked{true};
    
    std::string m_wallet_name;
    std::string m_data_dir;
    
    // Cached data
    WalletBalance m_cached_balance;
    std::vector<Transaction> m_cached_transactions;
    std::vector<AddressBookEntry> m_address_book;
    NetworkInfo m_network_info;
    
    // Callbacks
    std::function<void(const WalletBalance&)> m_balance_callback;
    std::function<void(const Transaction&)> m_transaction_callback;
    std::function<void(double)> m_sync_callback;
    
    // Internal methods
    void UpdateBalance();
    void UpdateTransactions();
    void UpdateNetworkInfo();
    bool ConnectToNode();
    void SimulateWalletData(); // For demo purposes
};

#endif // GOTHAM_SDL2_WALLET_WALLET_MANAGER_H
#pragma once

#include <string>
#include <deque>
#include <mutex>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <quickfix/Application.h>
#include <quickfix/Session.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>

class SimpleScalper : public FIX::Application, public FIX::MessageCracker {
private:
    // Connection parameters
    std::string senderCompID;
    std::string targetCompID;
    std::string fixVersion;
    std::string hostname;
    int port;
    
    // Trading parameters
    std::string symbol;
    double stopLossPercentage;
    double profitTargetPercentage;
    double tradeAmount;
    
    // Trading state
    std::atomic<bool> inPosition;
    std::atomic<bool> isLongPosition;
    double entryPrice;
    std::string currentOrderId;
    
    // Market data
    std::deque<double> priceHistory;
    std::mutex priceHistoryMutex;
    double lastBid;
    double lastAsk;
    
    // FIX connection
    FIX::SessionSettings* settings;
    FIX::SocketInitiator* initiator;
    
    // Order ID generation
    std::string generateOrderID();
    
    // Initialize connection to the exchange
    void initializeFixConnection();
    
    // Subscribe to market data
    void subscribeToMarketData(const std::string& symbol);
    
    // Indicator calculation
    double calculateRSI(int periods);
    
    // Trading logic
    bool shouldEnterLong(double bid, double ask);
    bool shouldEnterShort(double bid, double ask);
    bool shouldExitPosition(double bid, double ask);
    
    // Order management
    void enterLong(double price);
    void enterShort(double price);
    void exitPosition(double bid, double ask);
    void sendBuyOrder(double amount, double price);
    void sendSellOrder(double amount, double price);
    void cancelOrder(const std::string& orderId);

    // QuickFIX::Application interface implementation
    void onCreate(const FIX::SessionID&) override;
    void onLogon(const FIX::SessionID&) override;
    void onLogout(const FIX::SessionID&) override;
    void toAdmin(FIX::Message&, const FIX::SessionID&) override;
    void toApp(FIX::Message&, const FIX::SessionID&) override;
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) override;
    void fromApp(const FIX::Message&, const FIX::SessionID&) override;

    // MessageCracker handlers
    void onMessage(const FIX42::ExecutionReport&, const FIX::SessionID&) override;
    void onMessage(const FIX42::MarketDataSnapshotFullRefresh&, const FIX::SessionID&) override;
    void onMessage(const FIX42::MarketDataIncrementalRefresh&, const FIX::SessionID&) override;

public:
    SimpleScalper(
        const std::string& sender, 
        const std::string& target, 
        const std::string& version,
        const std::string& host,
        int port,
        const std::string& symbol = "BTCUSD",
        double stopLoss = 0.05,
        double profitTarget = 0.1,
        double amount = 0.1
    );
    
    ~SimpleScalper();
    
    // Public method for testing market data processing
    void processMarketData(double bid, double ask);
    
    // Start scalper
    void run();
    
    // Stop scalper
    void stop();
    
    // Print performance statistics
    void printPerformanceStats();
}; 
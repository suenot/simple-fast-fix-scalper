#include "SimpleScalper.h"
#include <quickfix/fix42/ExecutionReport.h>
#include <quickfix/fix42/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix42/MarketDataIncrementalRefresh.h>
#include <quickfix/FileLog.h>

// TODO: #include <onixs/fix/engine.h> // или другой FIX движок

SimpleScalper::SimpleScalper(
    const std::string& sender, 
    const std::string& target, 
    const std::string& version,
    const std::string& host,
    int p,
    const std::string& sym,
    double stopLoss,
    double profitTarget,
    double amount
) : senderCompID(sender), 
    targetCompID(target), 
    fixVersion(version),
    hostname(host),
    port(p),
    symbol(sym),
    stopLossPercentage(stopLoss),
    profitTargetPercentage(profitTarget),
    tradeAmount(amount),
    inPosition(false),
    isLongPosition(false),
    entryPrice(0.0),
    lastBid(0.0),
    lastAsk(0.0),
    settings(nullptr),
    initiator(nullptr) {
    initializeFixConnection();
}

SimpleScalper::~SimpleScalper() {
    stop();
    if (initiator) delete initiator;
    if (settings) delete settings;
}

void SimpleScalper::initializeFixConnection() {
    // Create session settings from config.cfg
    try {
        settings = new FIX::SessionSettings("config.cfg");
        FIX::FileStoreFactory storeFactory(*settings);
        FIX::FileLogFactory logFactory(*settings);
        initiator = new FIX::SocketInitiator(*this, storeFactory, *settings, logFactory);
    } catch (std::exception& e) {
        std::cerr << "FIX initialization error: " << e.what() << std::endl;
        throw;
    }
}

void SimpleScalper::run() {
    if (!initiator) return;
    initiator->start();
    std::cout << "Scalper started. Waiting for events..." << std::endl;
    // Main loop — you can add signal or command handling here
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SimpleScalper::stop() {
    if (initiator) {
        initiator->stop();
        std::cout << "FIX connection stopped." << std::endl;
    }
}

// --- QuickFIX::Application interface ---
void SimpleScalper::onCreate(const FIX::SessionID& sessionID) {
    std::cout << "FIX session created: " << sessionID << std::endl;
}
void SimpleScalper::onLogon(const FIX::SessionID& sessionID) {
    std::cout << "Logon to exchange: " << sessionID << std::endl;
    subscribeToMarketData(symbol);
}
void SimpleScalper::onLogout(const FIX::SessionID& sessionID) {
    std::cout << "Logout from exchange: " << sessionID << std::endl;
}
void SimpleScalper::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {
    // Add login/password if needed
}
void SimpleScalper::toApp(FIX::Message& message, const FIX::SessionID& sessionID) {
    // Log outgoing messages
}
void SimpleScalper::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) {
    // Log incoming admin messages
}
void SimpleScalper::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) {
    crack(message, sessionID); // Delegate to MessageCracker
}

// --- MessageCracker ---
void SimpleScalper::onMessage(const FIX42::ExecutionReport& msg, const FIX::SessionID& sessionID) {
    // TODO: handle execution report
}
void SimpleScalper::onMessage(const FIX42::MarketDataSnapshotFullRefresh& msg, const FIX::SessionID& sessionID) {
    // TODO: handle market data snapshot
}
void SimpleScalper::onMessage(const FIX42::MarketDataIncrementalRefresh& msg, const FIX::SessionID& sessionID) {
    // TODO: handle market data incremental
}

void SimpleScalper::subscribeToMarketData(const std::string& symbol) {
    // TODO: send MarketDataRequest via QuickFIX
}

// --- Trading logic and indicators ---
double SimpleScalper::calculateRSI(int periods) {
    std::lock_guard<std::mutex> lock(priceHistoryMutex);
    if (priceHistory.size() < periods + 1) return 50.0;
    double gain = 0.0, loss = 0.0;
    for (size_t i = priceHistory.size() - periods; i < priceHistory.size() - 1; ++i) {
        double diff = priceHistory[i + 1] - priceHistory[i];
        if (diff > 0) gain += diff;
        else loss -= diff;
    }
    if (loss == 0) return 100.0;
    double rs = gain / loss;
    return 100.0 - (100.0 / (1.0 + rs));
}

bool SimpleScalper::shouldEnterLong(double bid, double ask) {
    double rsi = calculateRSI(14);
    return rsi < 30.0;
}

bool SimpleScalper::shouldEnterShort(double bid, double ask) {
    double rsi = calculateRSI(14);
    return rsi > 70.0;
}

bool SimpleScalper::shouldExitPosition(double bid, double ask) {
    if (!inPosition) return false;
    double price = isLongPosition ? bid : ask;
    double pnl = isLongPosition ? (price - entryPrice) : (entryPrice - price);
    if (pnl / entryPrice >= profitTargetPercentage) return true;
    if (pnl / entryPrice <= -stopLossPercentage) return true;
    return false;
}

void SimpleScalper::enterLong(double price) {
    inPosition = true;
    isLongPosition = true;
    entryPrice = price;
    sendBuyOrder(tradeAmount, price);
}
void SimpleScalper::enterShort(double price) {
    inPosition = true;
    isLongPosition = false;
    entryPrice = price;
    sendSellOrder(tradeAmount, price);
}
void SimpleScalper::exitPosition(double bid, double ask) {
    if (!inPosition) return;
    if (isLongPosition) sendSellOrder(tradeAmount, bid);
    else sendBuyOrder(tradeAmount, ask);
    inPosition = false;
}
void SimpleScalper::sendBuyOrder(double amount, double price) {
    // TODO: send buy order via QuickFIX
}
void SimpleScalper::sendSellOrder(double amount, double price) {
    // TODO: send sell order via QuickFIX
}
void SimpleScalper::cancelOrder(const std::string& orderId) {
    // TODO: send order cancel via QuickFIX
}
std::string SimpleScalper::generateOrderID() {
    static std::atomic<int> counter{0};
    std::stringstream ss;
    ss << "ORD" << std::chrono::system_clock::now().time_since_epoch().count() << counter++;
    return ss.str();
}
void SimpleScalper::processMarketData(double bid, double ask) {
    std::lock_guard<std::mutex> lock(priceHistoryMutex);
    double mid = (bid + ask) / 2.0;
    priceHistory.push_back(mid);
    if (priceHistory.size() > 100) priceHistory.pop_front();
    if (inPosition) {
        if (shouldExitPosition(bid, ask)) exitPosition(bid, ask);
    } else {
        if (shouldEnterLong(bid, ask)) enterLong(ask);
        else if (shouldEnterShort(bid, ask)) enterShort(bid);
    }
}
void SimpleScalper::printPerformanceStats() {
    // TODO: print trade statistics
} 
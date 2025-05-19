#include "SimpleScalper.h"
#include <csignal>
#include <string>
#include <iostream>

SimpleScalper* globalScalper = nullptr;

void signalHandler(int signal) {
    std::cout << "\nTermination signal received. Stopping scalper..." << std::endl;
    if (globalScalper) {
        globalScalper->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    std::string senderCompID = "SENDER";
    std::string targetCompID = "TARGET";
    std::string fixVersion = "FIX.4.2";
    std::string hostname = "localhost";
    int port = 12345;
    std::string symbol = "BTCUSD";
    double stopLoss = 0.05;
    double profitTarget = 0.1;
    double tradeAmount = 0.1;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sender" && i + 1 < argc) senderCompID = argv[++i];
        else if (arg == "--target" && i + 1 < argc) targetCompID = argv[++i];
        else if (arg == "--version" && i + 1 < argc) fixVersion = argv[++i];
        else if (arg == "--host" && i + 1 < argc) hostname = argv[++i];
        else if (arg == "--port" && i + 1 < argc) port = std::stoi(argv[++i]);
        else if (arg == "--symbol" && i + 1 < argc) symbol = argv[++i];
        else if (arg == "--stop-loss" && i + 1 < argc) stopLoss = std::stod(argv[++i]);
        else if (arg == "--profit-target" && i + 1 < argc) profitTarget = std::stod(argv[++i]);
        else if (arg == "--amount" && i + 1 < argc) tradeAmount = std::stod(argv[++i]);
    }

    SimpleScalper scalper(
        senderCompID,
        targetCompID,
        fixVersion,
        hostname,
        port,
        symbol,
        stopLoss,
        profitTarget,
        tradeAmount
    );
    globalScalper = &scalper;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    try {
        scalper.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 
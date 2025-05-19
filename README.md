# Simple Fast/FIX Scalper

## Overview

**Simple Fast/FIX Scalper** is a high-performance trading bot written in modern C++ for algorithmic and high-frequency trading (HFT) on cryptocurrency and financial markets. The bot is designed to work with both the classic FIX protocol (via QuickFIX) and the ultra-low latency FAST protocol (via mFAST), allowing you to choose the optimal library for your trading needs.

- **QuickFIX**: Standard, widely used open-source FIX engine for most exchanges.
- **mFAST**: High-performance C++ encoder/decoder for the FAST protocol, ideal for HFT and ultra-low latency environments.

## Features

- Modular architecture with pluggable FIX/FAST backend
- Real-time market data processing
- Simple RSI-based scalping strategy (configurable)
- Automatic order management (entry, exit, stop-loss, profit target)
- Thread-safe price history and trading state
- Command-line configuration for all trading parameters
- Clean separation of trading logic and protocol implementation
- Ready for extension with custom strategies or connectors

## Architecture

```
+-------------------+
|   main.cpp        |
+-------------------+
          |
          v
+-------------------+
| SimpleScalper     |
+-------------------+
          |
          v
+-------------------+         +-------------------+
| IFixFastConnector | <------ | QuickFixConnector |
+-------------------+         +-------------------+
          ^
          |
          +-------------------+
                              |
                    +-------------------+
                    |  MfastConnector   |
                    +-------------------+
```

- **SimpleScalper**: Core trading logic, strategy, and state management.
- **IFixFastConnector**: Abstract interface for protocol connectors.
- **QuickFixConnector**: Implementation for FIX protocol using QuickFIX.
- **MfastConnector**: Implementation for FAST protocol using mFAST.

## Scalper Algorithm (Mermaid)

```mermaid
flowchart TD
    A([Start]) --> B{Receive Market Data}
    B --> C[Update Price History]
    C --> D{In Position?}
    D -- "No" --> E{RSI < 30?}
    E -- "Yes" --> F[Enter Long (Buy)]
    E -- "No" --> G{RSI > 70?}
    G -- "Yes" --> H[Enter Short (Sell)]
    G -- "No" --> B
    F --> B
    H --> B
    D -- "Yes" --> I{Should Exit? (Stop-loss or Profit Target)}
    I -- "Yes" --> J[Exit Position (Sell/Buy)]
    I -- "No" --> B
    J --> B
```

- **RSI**: Relative Strength Index, calculated over recent price history.
- **Should Exit?**: Checks if stop-loss or profit target is reached for the open position.
- The loop continues as new market data arrives.

## Library Selection

You can choose the protocol library at build time:
- **QuickFIX**: Use for standard FIX connectivity (default).
- **mFAST**: Use for ultra-low latency and HFT scenarios (if your exchange supports FAST protocol).

Switching between libraries is done via CMake options or preprocessor defines (see below).

## Build Instructions

### Prerequisites
- C++17 compatible compiler (GCC, Clang, MSVC)
- CMake >= 3.10
- [QuickFIX](https://github.com/quickfix/quickfix) (for FIX protocol)
- [mFAST](https://github.com/objectcomputing/mFAST) (for FAST protocol, optional)
- Boost (required by mFAST)

### Building with QuickFIX (default)

```bash
mkdir build
cd build
cmake .. -DUSE_QUICKFIX=ON
cmake --build .
```

### Building with mFAST (optional, for FAST protocol)

```bash
mkdir build
cd build
cmake .. -DUSE_MFAST=ON
cmake --build .
```

> **Note:** You must have the required library installed and available in your system paths. See each library's documentation for installation details.

## Configuration

The bot uses a standard QuickFIX-style configuration file (`config.cfg`). Example:

```
[DEFAULT]
ConnectionType=initiator
ReconnectInterval=60
SenderCompID=SENDER
TargetCompID=TARGET
FileStorePath=store
FileLogPath=log
StartTime=00:00:00
EndTime=23:59:59
HeartBtInt=30
UseDataDictionary=Y
DataDictionary=FIX42.xml
SocketConnectPort=12345
SocketConnectHost=localhost
# If login/password is required:
# Username=your_username
# Password=your_password

[SESSION]
BeginString=FIX.4.2
# Other parameters are inherited from DEFAULT
```

## Usage

Run the scalper with command-line arguments to override trading parameters:

```bash
./SimpleScalper \
  --host exchange.hostname.com \
  --port 9823 \
  --symbol BTCUSD \
  --stop-loss 0.05 \
  --profit-target 0.1 \
  --amount 0.1 \
  --sender SENDER \
  --target TARGET \
  --version FIX.4.2
```

### Command-Line Arguments
- `--host`           Exchange host (overrides config)
- `--port`           Exchange port (overrides config)
- `--symbol`         Trading symbol (e.g., BTCUSD)
- `--stop-loss`      Stop-loss percentage (e.g., 0.05 for 5%)
- `--profit-target`  Profit target percentage (e.g., 0.1 for 10%)
- `--amount`         Trade amount (e.g., 0.1)
- `--sender`         FIX SenderCompID
- `--target`         FIX TargetCompID
- `--version`        FIX version (e.g., FIX.4.2)

## Extending the Bot

- **Add new strategies**: Implement your own trading logic in `SimpleScalper` or create new strategy classes.
- **Add new protocol connectors**: Implement the `IFixFastConnector` interface for other protocols or custom APIs.
- **Performance tuning**: For HFT, use mFAST and optimize your system for low latency (CPU affinity, real-time kernel, etc).

## References
- [QuickFIX GitHub](https://github.com/quickfix/quickfix)
- [mFAST GitHub](https://github.com/objectcomputing/mFAST)
- [FIX Protocol](https://www.fixtrading.org/)
- [FAST Protocol](https://www.fixtrading.org/standards/fast/)

## Disclaimer
This project is for educational and research purposes. Use at your own risk. Always test thoroughly on demo environments before trading with real funds. 
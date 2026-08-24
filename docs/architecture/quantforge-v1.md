# QuantForge V1 — Architecture & Engineering Specification

> Version: 0.1.0  
> Status: Active Development  
> Last Updated: 2026-08-23

---

# 1. Overview

## 1.1 What is QuantForge?

QuantForge is a modular quantitative research and algorithmic trading platform designed to provide a complete pipeline from historical market data to strategy research, backtesting, risk management, portfolio management, and eventually paper and live trading.

The primary goal of QuantForge is not to implement a single trading strategy.

The goal is to build a reusable quantitative trading infrastructure in which strategies can be developed, tested, evaluated, and eventually deployed using the same underlying architecture.

The V1 architecture is centered around:

```text
Historical Market Data
        ↓
Data Pipeline
        ↓
Strategy Engine
        ↓
Signal Generator
        ↓
Risk Engine
        ↓
Portfolio / Position Manager
        ↓
Execution Engine
```

---

# 2. V1 Objectives

QuantForge V1 aims to establish a production-oriented foundation for quantitative research and algorithmic trading.

The major objectives are:

1. Build a reusable C++ trading engine core.
2. Build a Python research and data-analysis layer.
3. Establish a canonical market-data model.
4. Support historical data ingestion and storage.
5. Build a deterministic backtesting engine.
6. Implement strategy interfaces.
7. Implement signal generation.
8. Implement risk management.
9. Implement portfolio and position management.
10. Implement an execution abstraction.
11. Provide comprehensive automated testing.
12. Establish reproducible research and backtesting.
13. Maintain a clean architecture that can later support paper and live trading.

---

# 3. Core Design Philosophy

QuantForge follows these principles.

## 3.1 Correctness Over Premature Optimization

The first priority is:

```text
Correctness
    ↓
Testability
    ↓
Reproducibility
    ↓
Performance
```

Performance is important, but optimization should be driven by actual profiling and requirements rather than assumptions.

## 3.2 Modular Architecture

Subsystems should have clear responsibilities and interfaces.

We avoid creating one giant trading engine.

Instead:

```text
QuantForge
│
├── Core
├── Market Data
├── Strategy
├── Signal
├── Risk
├── Portfolio
├── Execution
├── Backtesting
└── Research / Python
```

Each subsystem should be independently testable.

## 3.3 Event-Driven Design

QuantForge uses events as a major communication mechanism between engine components.

Conceptually:

```text
Market Data
     ↓
Market Event
     ↓
Strategy
     ↓
Signal Event
     ↓
Risk Engine
     ↓
Order Event
     ↓
Execution
     ↓
Fill Event
     ↓
Portfolio
```

This allows the same event flow to be used in both historical simulation and future real-time environments.

## 3.4 No Silent Data Manipulation

Raw market data should not be silently modified.

For example, if data contains:

```text
09:00
09:01
09:01
09:03
```

QuantForge should not silently invent:

```text
09:02
```

unless an explicit processing or resampling operation requests it.

Raw data and processed data must remain distinguishable.

## 3.5 Reproducibility

A backtest should be reproducible.

A future experiment should be capable of recording:

```text
Dataset
Strategy version
Parameters
Time range
Market assumptions
Commission model
Slippage model
Engine version
```

so that results can be reproduced later.

---

# 4. Multi-Language Architecture

QuantForge intentionally uses multiple languages.

The language should be chosen according to the responsibility of the subsystem rather than forcing the entire project into one language.

Primary languages:

```text
C++
Python
```

Java is not currently part of the core V1 implementation.

---

# 5. C++ Responsibilities

C++ is the primary language for the performance-sensitive trading engine.

C++ will be responsible for:

- Core event system
- Market-data domain objects
- Strategy execution interfaces
- Signal processing
- Risk engine
- Portfolio and position management
- Order models
- Execution abstractions
- Backtesting engine
- Performance-critical calculations
- Event processing
- Future real-time trading infrastructure

The C++ core should be deterministic, strongly typed, testable, and independent of Python wherever practical.

---

# 6. Python Responsibilities

Python is the primary research, data-science, and analytical language.

Python will be responsible for:

- Data ingestion workflows
- Dataset preparation
- Research
- Exploratory analysis
- Feature engineering
- Statistical analysis
- Visualization
- Machine-learning experimentation
- Research notebooks
- Strategy prototyping
- Interaction with the C++ engine

Python should not duplicate the core trading engine unnecessarily.

---

# 7. C++ / Python Boundary

The architecture is:

```text
                 QuantForge
                     │
          ┌──────────┴──────────┐
          │                     │
      C++ Engine            Python Layer
          │                     │
   Trading / Backtest      Research / Data
          │                     │
          └──────────┬──────────┘
                     │
              Arrow / Interop
```

Apache Arrow is planned as an interoperability layer between C++ and Python.

The QuantForge domain model should not become tightly coupled to Arrow objects.

Instead:

```text
QuantForge Domain Model
        ↓
Arrow Interoperability
        ↓
Python / Storage / Analytics
```

---

# 8. Technology Stack

## 8.1 C++

Primary engine language:

```text
C++23
```

Compiler on the current development platform:

```text
Apple Clang
```

## 8.2 Build System

```text
CMake
```

CMake is responsible for:

- Build configuration
- Library targets
- Executable targets
- Testing integration
- Dependency discovery
- Cross-platform build support

## 8.3 Testing

Primary C++ testing framework:

```text
GoogleTest
```

Test discovery and execution:

```text
CTest
```

Testing principle:

> Every important engine component should have automated tests.

## 8.4 Python

Python target:

```text
Python 3.12
```

Development environment:

```text
.venv/
```

Python package configuration:

```text
pyproject.toml
```

The virtual environment must never be committed to Git.

## 8.5 Data Interoperability

Planned:

```text
Apache Arrow
```

Arrow will provide an efficient cross-language data representation and interoperability layer.

## 8.6 Historical Storage

Planned:

```text
Apache Parquet
```

Parquet will be the primary historical columnar storage format.

---

# 9. Repository Structure

Current foundation:

```text
QuantForge/
│
├── .gitignore
├── .vscode/
│   └── settings.json
│
├── CMakeLists.txt
├── LICENSE
├── README.md
├── pyproject.toml
│
├── docs/
│   └── architecture/
│       └── quantforge-v1.md
│
├── cpp/
│   │
│   ├── include/
│   │   └── quantforge/
│   │       └── core/
│   │           ├── event.hpp
│   │           └── event_type.hpp
│   │
│   ├── src/
│   │   ├── core/
│   │   │   ├── event.cpp
│   │   │   └── event_type.cpp
│   │   │
│   │   └── app/
│   │       └── main.cpp
│   │
│   └── tests/
│       └── core/
│           └── event_test.cpp
│
├── python/
│   └── quantforge/
│       └── __init__.py
│
└── build/                 # ignored
```

The repository will grow as additional subsystems are implemented.

---

# 10. C++ Core Library

The C++ engine is organized around a reusable core library:

```text
quantforge-core
```

Current relationship:

```text
quantforge-core
       │
       ├── Event
       ├── EventType
       └── future core components
       │
       ├───────────────┐
       ↓               ↓
 quantforge       quantforge-tests
```

This allows future applications to reuse the same engine.

Potential future executables include:

```text
quantforge
quantforge-backtest
quantforge-paper
quantforge-live
```

---

# 11. Event Architecture

The event system is the core communication mechanism of QuantForge.

Current event abstraction:

```text
Event
├── EventId
├── EventType
└── Timestamp
```

---

# 12. Event ID

Event IDs use:

```cpp
std::uint64_t
```

through the alias:

```cpp
using EventId = std::uint64_t;
```

The purpose is to provide a unique identity for individual events.

Event IDs will eventually help trace a chain such as:

```text
Market Event
     ↓
Signal Event
     ↓
Order Event
     ↓
Fill Event
```

---

# 13. Timestamp

The current C++ event timestamp uses:

```cpp
std::chrono::system_clock::time_point
```

through:

```cpp
using Timestamp =
    std::chrono::system_clock::time_point;
```

Time handling is considered critical infrastructure.

Future market-data models will distinguish between concepts such as:

```text
Event time
Received time
Processing time
```

when required.

---

# 14. Event Types

Current event types:

```text
MarketData
Signal
Order
Fill
PortfolioUpdate
System
```

Represented by:

```cpp
enum class EventType
```

The strongly typed enum is preferred over raw integer event identifiers.

---

# 15. Planned Event Hierarchy

The eventual event hierarchy is:

```text
Event
│
├── MarketDataEvent
│   ├── BarEvent
│   ├── TradeEvent
│   ├── QuoteEvent
│   └── OrderBookEvent
│
├── SignalEvent
│
├── OrderEvent
│
├── FillEvent
│
├── PortfolioUpdateEvent
│
└── SystemEvent
```

Only components required by V1 will be implemented initially.

---

# 16. Market Data Architecture

Market data is the first major trading-domain subsystem.

The central principle is:

> External provider formats must not become QuantForge's internal data model.

The intended pipeline is:

```text
External Data Source
        ↓
Data Ingestion
        ↓
Raw Data
        ↓
Normalization
        ↓
Validation
        ↓
Canonical Market Data
        ↓
┌───────────────┬───────────────┐
│               │               │
C++ Engine    Python         Parquet
              Research       Storage
```

---

# 17. Data Sources

QuantForge should eventually support multiple types of data sources:

```text
REST APIs
CSV / Files
WebSockets
Broker APIs
Exchange feeds
```

Each source should have an adapter.

Provider-specific logic belongs in ingestion adapters, not the core engine.

---

# 18. Raw vs Processed Data

Conceptual storage:

```text
data/
├── raw/
├── processed/
└── cache/
```

## Raw

Raw data should preserve the original source information as faithfully as practical.

## Processed

Processed data is normalized into QuantForge's canonical representation.

## Cache

Temporary/generated data that can be recreated.

---

# 19. Instrument Model

QuantForge should not rely solely on strings such as:

```text
AAPL
NIFTY
BTCUSDT
```

because symbols are not guaranteed to be globally unique.

The planned concept is:

```text
Instrument
├── InstrumentId
├── Symbol
├── AssetClass
├── Exchange
├── Currency
├── TickSize
├── LotSize
└── Metadata
```

The V1 implementation will begin with only the fields required by the actual supported markets.

---

# 20. Asset Classes

The architecture should be capable of supporting multiple asset classes.

Potential classes:

```text
Equity
Future
Option
Forex
Crypto
Index
ETF
```

Only required asset classes will be implemented initially.

---

# 21. Canonical Bar Model

The first market-data object will be the historical bar.

Canonical V1 concept:

```text
Bar
├── InstrumentId
├── StartTime
├── EndTime
├── Open
├── High
├── Low
├── Close
└── Volume
```

The bar timeframe must be explicit.

Example:

```text
5-minute bar

Start:
09:15:00

End:
09:20:00
```

Using both start and end times avoids ambiguity about what a single timestamp represents.

---

# 22. Market Data Levels

QuantForge will grow through multiple levels of market-data detail.

## Level 1 — Bars

```text
OHLCV
```

Used primarily for:

- Historical research
- Indicators
- Strategy development
- Many backtests

## Level 2 — Trades and Quotes

Trade:

```text
Timestamp
Price
Quantity
```

Quote:

```text
Timestamp
Bid Price
Bid Size
Ask Price
Ask Size
```

This level enables more realistic execution and market-microstructure modeling.

## Level 3 — Order Book

Future order-book representation:

```text
OrderBook
├── Timestamp
├── Bids
└── Asks
```

This is not part of the initial implementation.

---

# 23. Market Data Validation

Data must be validated before entering the canonical layer.

Validation includes:

```text
Timestamp ordering
Duplicate detection
Missing data detection
Invalid prices
NaN / Inf detection
Symbol consistency
Timezone consistency
OHLC consistency
Volume validation
```

For a bar:

```text
High >= max(Open, Close, Low)
Low  <= min(Open, Close, High)
Volume >= 0
```

Validation should identify problems rather than silently hide them.

---

# 24. Missing Data

Missing data and invalid data are different conditions.

Example:

```text
Missing:
09:00
09:01
09:03

Invalid:
09:02 exists but contains impossible values
```

QuantForge should represent these conditions separately.

Data filling, interpolation, and resampling must be explicit processing operations.

---

# 25. No Silent Corrections

QuantForge must not silently:

- invent missing bars
- alter prices
- remove duplicates without recording the operation
- change timestamps
- apply corporate-action adjustments without documenting them

Transformations should be explicit and reproducible.

---

# 26. Corporate Actions

Equity data may eventually need support for:

```text
Stock Splits
Dividends
Rights Issues
Symbol Changes
```

Corporate actions can materially affect historical data.

The architecture must therefore allow adjustment policies to be recorded as dataset metadata.

Implementation can be added when required by the supported equity data source.

---

# 27. Arrow Architecture

Apache Arrow is planned as the primary interoperability layer.

Conceptually:

```text
QuantForge Domain Model
        ↓
Apache Arrow
        ↓
Python
        ↓
Research / Analytics
```

and:

```text
Parquet
        ↓
Apache Arrow
        ↓
QuantForge / Python
```

Arrow should be treated as an interoperability and data-processing layer rather than the domain model itself.

---

# 28. Parquet Architecture

Historical data will primarily use Parquet.

Conceptual structure:

```text
data/processed/
└── bars/
    └── timeframe=5m/
        └── symbol=AAPL/
            └── year=2026/
```

The exact partitioning strategy will be determined by actual query patterns and dataset scale.

Partitioning should optimize common research and backtesting queries.

---

# 29. Dataset Metadata

Every processed dataset should eventually have metadata similar to:

```text
Dataset
├── DatasetId
├── Source
├── Instruments
├── Timeframe
├── StartTime
├── EndTime
├── Timezone
├── SchemaVersion
├── AdjustmentPolicy
├── CreatedAt
└── SourceVersion
```

This allows a backtest to record exactly which dataset it used.

Example:

```text
Dataset:
NIFTY-5M-2026-v3
```

A future experiment can reference this dataset rather than relying on an unspecified file.

---

# 30. Strategy Engine

The strategy engine will consume market events.

Conceptually:

```text
MarketDataEvent
       ↓
Strategy
       ↓
SignalEvent
```

Strategies should not directly place broker orders.

Instead, they produce signals or strategy decisions.

This allows risk management and portfolio management to remain independent.

---

# 31. Signal Generator

The signal layer converts strategy logic into structured decisions.

Example:

```text
Signal
├── Instrument
├── Direction
├── Strength / Score
├── Timestamp
└── Strategy Metadata
```

Potential directions:

```text
BUY
SELL
HOLD
```

The exact signal model will be finalized when the strategy subsystem is implemented.

---

# 32. Risk Engine

The risk engine sits between strategy decisions and orders.

Conceptually:

```text
Signal
   ↓
Risk Engine
   ↓
Approved / Rejected / Modified Order
```

Future risk controls may include:

```text
Position limits
Exposure limits
Maximum order size
Maximum portfolio risk
Drawdown limits
Leverage limits
Daily loss limits
Sector / asset exposure
```

The risk engine must be usable in both backtesting and live environments.

---

# 33. Portfolio / Position Manager

The portfolio subsystem maintains state such as:

```text
Cash
Positions
Average Entry
Unrealized P&L
Realized P&L
Exposure
Margin
Portfolio Value
```

Portfolio state should be driven by fills and other relevant events rather than strategy assumptions.

---

# 34. Execution Engine

The execution engine abstracts how orders are executed.

Conceptually:

```text
Order
  ↓
Execution Engine
  ↓
Fill
```

In backtesting:

```text
Order
  ↓
Simulated Execution
  ↓
Fill
```

In paper trading:

```text
Order
  ↓
Paper Broker
  ↓
Fill
```

In live trading:

```text
Order
  ↓
Broker / Exchange Adapter
  ↓
Fill
```

The strategy should not need to know which execution environment it is running in.

---

# 35. Backtesting Engine

The backtesting engine should reproduce the same conceptual event flow as live trading.

Historical mode:

```text
Historical Data
      ↓
Market Event
      ↓
Strategy
      ↓
Signal
      ↓
Risk
      ↓
Order
      ↓
Simulated Execution
      ↓
Fill
      ↓
Portfolio
```

This is critical for reducing the difference between backtesting and eventual deployment.

---

# 36. Avoiding Look-Ahead Bias

The backtesting architecture must explicitly prevent future information from becoming available before its valid timestamp.

For example:

```text
10:00 bar
```

must not be used to generate a decision that assumes knowledge of the completed:

```text
10:00–10:05 bar
```

before that information would actually have been available.

Timestamp semantics and event ordering are therefore core correctness requirements.

---

# 37. Execution Modeling

Backtesting must eventually model realistic execution assumptions.

Potential components:

```text
Commission
Fees
Slippage
Spread
Latency
Partial Fills
Liquidity
Market Impact
```

These should be explicit configuration rather than hidden assumptions.

---

# 38. Testing Architecture

C++ testing uses:

```text
GoogleTest
CTest
```

Current structure:

```text
cpp/tests/
└── core/
    └── event_test.cpp
```

Tests should verify behavior rather than implementation details wherever practical.

Testing levels will eventually include:

```text
Unit Tests
Integration Tests
Backtest Validation Tests
Data Validation Tests
End-to-End Tests
```

---

# 39. Current Test Coverage

The initial Event model has tests for:

```text
Event ID
Event Type
Timestamp
```

Current verification:

```text
3 tests
3 passed
0 failed
```

This establishes the testing foundation before additional engine components are implemented.

---

# 40. Python Package Architecture

Current Python package:

```text
python/
└── quantforge/
    └── __init__.py
```

Package configuration:

```text
pyproject.toml
```

Python is installed in editable mode during development.

The virtual environment:

```text
.venv/
```

is ignored by Git.

Generated packaging metadata such as:

```text
*.egg-info/
```

is also ignored.

---

# 41. Development Environment

Current development environment:

```text
Platform:
macOS / Apple Silicon

Compiler:
Apple Clang 21

Build:
CMake 4.3.2

C++:
C++23

Python:
3.12.x

Testing:
GoogleTest 1.18.x
CTest

Package Manager:
Homebrew

Version Control:
Git / GitHub
```

Exact dependency versions may evolve.

---

# 42. IDE Configuration

The repository contains:

```text
.vscode/settings.json
```

which points C++ IntelliSense to the CMake compilation database:

```text
build/compile_commands.json
```

CMake generates the compilation database using:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

This avoids hard-coding dependency include paths into VS Code.

---

# 43. Git Strategy

QuantForge follows:

```text
Implement
   ↓
Build
   ↓
Test
   ↓
Verify
   ↓
Commit
   ↓
Push
```

Commits represent meaningful milestones.

Examples:

```text
chore: initialize QuantForge repository
build: configure QuantForge C++ core
feat: add core event model
refactor: introduce QuantForge core library
test: establish C++ and Python foundations
chore: configure CMake IntelliSense
docs: establish QuantForge V1 architecture
```

Tiny changes should not each become separate commits.

Broken or unverified work should generally not be committed to `main`.

---

# 44. Branch Strategy

During early solo development:

```text
main
```

is the primary development branch.

Feature branches can be introduced when subsystems become sufficiently large to justify them.

Potential future structure:

```text
main
│
├── feature/market-data
├── feature/backtesting
├── feature/risk-engine
├── feature/execution
└── feature/python-research
```

---

# 45. Generated Files

The following must not be committed:

```text
build/
.venv/
*.egg-info/
```

Build artifacts and local development environments are reproducible and therefore should remain outside source control.

---

# 46. V1 Development Roadmap

## Phase 0 — Foundation

**Status: Complete**

```text
Repository
CMake
C++23
Core library
Event model
GoogleTest
CTest
Python environment
Python package
IDE integration
```

---

## Phase 1 — Market Data

**Status: Next**

Planned:

```text
Instrument
AssetClass
Timeframe
Bar
MarketDataEvent
Data validation
Historical data model
Arrow integration
Parquet storage
Dataset metadata
Data loader
```

---

## Phase 2 — Strategy Engine

**Status: Planned**

Planned:

```text
Strategy interface
Strategy lifecycle
Signal model
Indicator infrastructure
Strategy configuration
```

---

## Phase 3 — Risk Engine

**Status: Planned**

Planned:

```text
Position limits
Exposure limits
Risk checks
Order validation
Portfolio risk
```

---

## Phase 4 — Portfolio Management

**Status: Planned**

Planned:

```text
Positions
Cash
P&L
Exposure
Portfolio valuation
Accounting
```

---

## Phase 5 — Execution Engine

**Status: Planned**

Planned:

```text
Order model
Execution interface
Simulated execution
Commission
Slippage
Fill model
```

---

## Phase 6 — Backtesting Engine

**Status: Planned**

Planned:

```text
Historical event loop
Event scheduling
Strategy execution
Risk integration
Portfolio integration
Execution simulation
Performance metrics
```

---

## Phase 7 — Research Layer

**Status: Planned**

Planned:

```text
Python API
Dataset loading
Feature engineering
Research notebooks
Visualization
Experiment management
```

---

## Phase 8 — Paper Trading

**Status: Future**

Planned:

```text
Live market data adapter
Paper execution
Real-time event loop
Monitoring
Logging
```

---

## Phase 9 — Live Trading

**Status: Future**

Planned:

```text
Broker adapters
Exchange adapters
Live risk controls
Order monitoring
Failure recovery
Operational monitoring
```

Live trading is intentionally outside the initial implementation scope.

---

# 47. Long-Term Architecture

The eventual system should look approximately like:

```text
                       QuantForge
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
   Data Platform       C++ Engine        Python Research
        │                  │                  │
        │            ┌─────┼─────┐            │
        │            │     │     │            │
        │         Strategy Risk Portfolio      │
        │            │     │     │            │
        │            └─────┼─────┘            │
        │                  │                  │
        │             Execution              │
        │                  │                  │
        └────────────── Backtest ─────────────┘
                           │
                     Paper Trading
                           │
                      Live Trading
```

The same conceptual trading pipeline should support all execution modes.

---

# 48. Architectural Invariants

The following principles should remain true unless explicitly revisited and documented.

### Invariant 1

Strategies do not directly communicate with brokers.

```text
Strategy
   ↓
Signal
   ↓
Risk
   ↓
Order
   ↓
Execution
```

### Invariant 2

Provider-specific market-data formats do not become engine-domain models.

### Invariant 3

Raw data is never silently modified.

### Invariant 4

Backtesting and live trading should share the same core domain concepts.

### Invariant 5

Important engine behavior must be testable.

### Invariant 6

Python should complement the C++ engine rather than duplicate it.

### Invariant 7

Data transformations must be explicit and reproducible.

### Invariant 8

Time semantics must be explicit.

### Invariant 9

Generated environments and build artifacts do not belong in Git.

---

# 49. Current Architectural Decisions

| Decision | Choice |
|---|---|
| Primary engine language | C++23 |
| Research language | Python 3.12 |
| Build system | CMake |
| C++ testing | GoogleTest |
| Test runner | CTest |
| Historical storage | Apache Parquet |
| Data interoperability | Apache Arrow |
| Core architecture | Modular |
| Engine communication | Event-driven |
| Historical first | Yes |
| Backtesting | Event-driven |
| Python environment | `.venv` |
| Python package config | `pyproject.toml` |
| Main development branch | `main` |

---

# 50. Decisions Still To Be Finalized

The following decisions have been finalized for the V1 Market Data foundation:

| Decision | V1 Choice |
|---|---|
| Instrument ID | `std::uint64_t` |
| Asset Class | Strongly typed `enum class` |
| Price representation | Fixed-point integer |
| Quantity representation | Fixed-point integer |
| Timestamp | UTC nanosecond precision |
| Core timestamp type | `std::chrono::sys_time<std::chrono::nanoseconds>` |
| Core timezone | UTC |
| Exchange timezone | Stored as instrument / market metadata |
| Initial market data | OHLCV bars |
| Historical storage | Apache Parquet |
| Data interoperability | Apache Arrow |
| Initial validation | Explicit and deterministic |
| Timeframe representation | Strongly typed value + unit |
| Supported V1 timeframe units | Tick, Second, Minute, Hour, Day |
| Calendar periods | Deferred until trading calendar subsystem |

The following architectural decisions remain open and will be finalized before their respective subsystems are implemented:

```text
Timeframe representation
Dataset partitioning
Dataset versioning
Data provider interfaces
Market calendar model
Corporate-action model
Arrow integration boundary
Python/C++ binding mechanism
Order model
Risk model
Execution model
Backtest clock
Slippage model
Commission model

---

# 51. Engineering Rule

Before introducing a major subsystem:

```text
Research
   ↓
Design
   ↓
Document
   ↓
Implement
   ↓
Test
   ↓
Benchmark / Validate
   ↓
Commit
```

The architecture document should be updated whenever a significant architectural decision changes.

---

# 52. Current Status

As of V1 architecture initialization:

```text
Repository                  COMPLETE
Build System                COMPLETE
C++ Core Library            COMPLETE
Event Model                 COMPLETE
Testing Foundation          COMPLETE
Python Foundation           COMPLETE
IDE Integration             COMPLETE

Market Data                 NEXT
Strategy Engine             PLANNED
Signal System               PLANNED
Risk Engine                 PLANNED
Portfolio Manager           PLANNED
Execution Engine            PLANNED
Backtesting Engine          PLANNED
Research Layer              PLANNED
Paper Trading               FUTURE
Live Trading                FUTURE
```

---

# 53. Definition of V1

QuantForge V1 will be considered complete when a user can:

1. Load historical market data.
2. Validate and normalize that data.
3. Store and load canonical datasets.
4. Define a strategy.
5. Generate signals.
6. Apply risk rules.
7. Create simulated orders.
8. Simulate execution.
9. Update portfolio state.
10. Calculate performance.
11. Run reproducible backtests.
12. Inspect and analyze results from Python.
13. Run the same conceptual strategy through the engine without rewriting its core logic.

The ultimate V1 workflow is:

```text
Historical Dataset
       ↓
Market Data
       ↓
Strategy
       ↓
Signals
       ↓
Risk
       ↓
Orders
       ↓
Execution Simulation
       ↓
Fills
       ↓
Portfolio
       ↓
Performance
       ↓
Research / Analysis
```

---

# 54. Final Vision

QuantForge is intended to become more than a backtesting script.

It is a quantitative trading infrastructure platform.

The long-term vision is:

```text
Research
   ↓
Backtest
   ↓
Validate
   ↓
Paper Trade
   ↓
Live Trade
```

while preserving a common:

```text
Data Model
Event Model
Strategy Interface
Risk Model
Portfolio Model
Execution Interface
```

The architecture should make moving from research to deployment an engineering transition rather than a complete rewrite.

---

**Document Status:** Active  
**Architecture Version:** V1  
**Next Major Subsystem:** Market Data
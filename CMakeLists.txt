cmake_minimum_required(VERSION 3.25)

project(QuantForge
    VERSION 0.1.0
    DESCRIPTION "QuantForge quantitative research and algorithmic trading platform"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# QuantForge Core Library
add_library(quantforge-core
   cpp/src/risk/risk_decision.cpp
   cpp/src/order/order_intent.cpp
   cpp/src/portfolio/position.cpp
   cpp/src/signal/signal.cpp

   cpp/src/backtest/backtest_engine.cpp

   cpp/src/backtest/backtest_clock.cpp

    cpp/src/core/event.cpp
    cpp/src/core/event_type.cpp
    cpp/src/market/instrument.cpp
    cpp/src/market/bar.cpp
    cpp/src/market/validation_result.cpp
    cpp/src/market/bar_validator.cpp
cpp/src/market/bar_continuity_validator.cpp
    cpp/src/marketdata/market_data_request_validator.cpp
    cpp/src/market/timeframe.cpp
    cpp/src/marketdata/csv_bar_parser.cpp
    cpp/src/marketdata/csv_market_data_source.cpp
cpp/src/marketdata/market_data_pipeline.cpp
)

target_include_directories(quantforge-core
    PUBLIC
        cpp/include
)


# QuantForge Application
add_executable(quantforge

    cpp/src/app/main.cpp
)

target_link_libraries(quantforge
    PRIVATE
        quantforge-core
)


# Testing
enable_testing()

find_package(GTest REQUIRED)

add_executable(quantforge-tests
   cpp/tests/risk/risk_decision_test.cpp
   cpp/tests/order/order_intent_test.cpp
   cpp/tests/portfolio/position_test.cpp
   cpp/tests/portfolio/portfolio_state_test.cpp
   cpp/tests/risk/risk_engine_test.cpp
   cpp/tests/signal/signal_test.cpp
   cpp/tests/strategy/strategy_test.cpp

   cpp/tests/backtest/backtest_engine_test.cpp

   cpp/tests/backtest/backtest_clock_test.cpp

    cpp/tests/core/event_test.cpp
    cpp/tests/market/instrument_id_test.cpp
    cpp/tests/market/instrument_test.cpp
    cpp/tests/market/price_test.cpp
    cpp/tests/market/quantity_test.cpp
    cpp/tests/market/timestamp_test.cpp
    cpp/tests/market/timeframe_test.cpp
    cpp/tests/market/bar_test.cpp
    cpp/tests/market/validation_result_test.cpp
    cpp/tests/market/bar_validator_test.cpp
cpp/tests/market/bar_continuity_validator_test.cpp
    cpp/tests/marketdata/market_data_request_test.cpp
    cpp/tests/marketdata/market_data_request_validator_test.cpp
    cpp/tests/market/timeframe_test.cpp
    cpp/tests/marketdata/market_data_exception_test.cpp
    cpp/tests/marketdata/csv_market_data_source_test.cpp
cpp/tests/marketdata/market_data_pipeline_test.cpp
    cpp/tests/marketdata/csv_bar_parser_test.cpp
)

target_link_libraries(quantforge-tests
    PRIVATE
        quantforge-core
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(quantforge-tests)
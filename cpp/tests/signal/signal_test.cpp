#include "quantforge/signal/signal.hpp"

#include <chrono>

#include <gtest/gtest.h>

TEST(SignalTest, StoresInstrumentId) {
    const quantforge::signal::Signal signal(
        quantforge::market::InstrumentId{42},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Buy,
        80
    );

    EXPECT_EQ(
        signal.instrument_id(),
        quantforge::market::InstrumentId{42}
    );
}

TEST(SignalTest, StoresTimestamp) {
    const auto timestamp = quantforge::market::Timestamp{
        std::chrono::seconds{123}
    };

    const quantforge::signal::Signal signal(
        quantforge::market::InstrumentId{1},
        timestamp,
        quantforge::signal::SignalDirection::Buy,
        80
    );

    EXPECT_EQ(signal.timestamp(), timestamp);
}

TEST(SignalTest, StoresDirection) {
    const quantforge::signal::Signal buy(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Buy,
        80
    );

    const quantforge::signal::Signal sell(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Sell,
        80
    );

    const quantforge::signal::Signal hold(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Hold,
        0
    );

    EXPECT_EQ(
        buy.direction(),
        quantforge::signal::SignalDirection::Buy
    );

    EXPECT_EQ(
        sell.direction(),
        quantforge::signal::SignalDirection::Sell
    );

    EXPECT_EQ(
        hold.direction(),
        quantforge::signal::SignalDirection::Hold
    );
}

TEST(SignalTest, StoresStrength) {
    const quantforge::signal::Signal signal(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Buy,
        95
    );

    EXPECT_EQ(signal.strength(), 95);
}

TEST(SignalTest, SupportsZeroStrength) {
    const quantforge::signal::Signal signal(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Hold,
        0
    );

    EXPECT_EQ(signal.strength(), 0);
}

TEST(SignalTest, SupportsMaximumStrength) {
    const quantforge::signal::Signal signal(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Buy,
        100
    );

    EXPECT_EQ(signal.strength(), 100);
}

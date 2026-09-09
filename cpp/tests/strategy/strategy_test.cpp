#include "quantforge/strategy/strategy.hpp"

#include <chrono>

#include <gtest/gtest.h>

namespace {

class TestStrategy final : public quantforge::strategy::Strategy {
public:
    void on_start() override {
        ++start_count;
    }

    void on_bar(const quantforge::market::Bar&) override {
        ++bar_count;
    }

    void on_finish() override {
        ++finish_count;
    }

    int start_count{0};
    int bar_count{0};
    int finish_count{0};
};

} // namespace

TEST(StrategyTest, SupportsLifecycleCallbacks) {
    TestStrategy strategy;

    strategy.on_start();
    strategy.on_finish();

    EXPECT_EQ(strategy.start_count, 1);
    EXPECT_EQ(strategy.finish_count, 1);
}

TEST(StrategyTest, ReceivesBars) {
    TestStrategy strategy;

    quantforge::market::InstrumentId instrument_id{1};

    quantforge::market::Timestamp timestamp{
        std::chrono::seconds{100}
    };

    quantforge::market::Timeframe timeframe{
        1,
        quantforge::market::TimeframeUnit::Minute
    };

    quantforge::market::Bar bar(
        instrument_id,
        timestamp,
        timeframe,
        quantforge::market::Price{10000, 2},
        quantforge::market::Price{10100, 2},
        quantforge::market::Price{9900, 2},
        quantforge::market::Price{10050, 2},
        quantforge::market::Quantity{1000, 0}
    );

    strategy.on_bar(bar);

    EXPECT_EQ(strategy.bar_count, 1);
}

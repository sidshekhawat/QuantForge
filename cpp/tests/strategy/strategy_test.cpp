#include "quantforge/strategy/strategy.hpp"

#include <chrono>
#include <vector>

#include <gtest/gtest.h>

namespace {

class TestSignalSink final : public quantforge::signal::SignalSink {
public:
    void emit(const quantforge::signal::Signal& signal) override {
        signals.push_back(signal);
    }

    std::vector<quantforge::signal::Signal> signals;
};

class TestStrategy final : public quantforge::strategy::Strategy {
public:
    void on_start(
        const quantforge::strategy::StrategyContext&
    ) override {
        ++start_count;
    }

    void on_bar(
        const quantforge::strategy::StrategyContext&,
        const quantforge::market::Bar&
    ) override {
        ++bar_count;
    }

    void on_finish(
        const quantforge::strategy::StrategyContext&
    ) override {
        ++finish_count;
    }

    int start_count{0};
    int bar_count{0};
    int finish_count{0};
};

} // namespace

TEST(StrategyTest, SupportsLifecycleCallbacks) {
    TestStrategy strategy;
    TestSignalSink sink;

    const quantforge::strategy::StrategyContext context{
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        sink
    };

    strategy.on_start(context);
    strategy.on_finish(context);

    EXPECT_EQ(strategy.start_count, 1);
    EXPECT_EQ(strategy.finish_count, 1);
}

TEST(StrategyTest, ReceivesBars) {
    TestStrategy strategy;
    TestSignalSink sink;

    const quantforge::strategy::StrategyContext context{
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        sink
    };

    quantforge::market::Bar bar(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::market::Timeframe{
            1,
            quantforge::market::TimeframeUnit::Minute
        },
        quantforge::market::Price{10000, 2},
        quantforge::market::Price{10100, 2},
        quantforge::market::Price{9900, 2},
        quantforge::market::Price{10050, 2},
        quantforge::market::Quantity{1000, 0}
    );

    strategy.on_bar(context, bar);

    EXPECT_EQ(strategy.bar_count, 1);
}

TEST(StrategyTest, ContextExposesCurrentTime) {
    TestSignalSink sink;

    const auto timestamp = quantforge::market::Timestamp{
        std::chrono::seconds{123}
    };

    const quantforge::strategy::StrategyContext context{
        timestamp,
        sink
    };

    EXPECT_EQ(context.now(), timestamp);
}

TEST(StrategyTest, ContextEmitsSignalThroughSink) {
    TestSignalSink sink;

    const auto timestamp = quantforge::market::Timestamp{
        std::chrono::seconds{123}
    };

    const quantforge::strategy::StrategyContext context{
        timestamp,
        sink
    };

    const quantforge::signal::Signal signal(
        quantforge::market::InstrumentId{42},
        timestamp,
        quantforge::signal::SignalDirection::Buy,
        80
    );

    context.emit_signal(signal);

    ASSERT_EQ(sink.signals.size(), 1);
    EXPECT_EQ(
        sink.signals[0].instrument_id(),
        quantforge::market::InstrumentId{42}
    );
    EXPECT_EQ(
        sink.signals[0].direction(),
        quantforge::signal::SignalDirection::Buy
    );
    EXPECT_EQ(sink.signals[0].strength(), 80);
}


#include "quantforge/backtest/backtest_engine.hpp"

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace {

quantforge::market::Bar MakeBar(
    quantforge::market::Timestamp timestamp)
{
    return quantforge::market::Bar(
        quantforge::market::InstrumentId{1},
        timestamp,
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
}

} // namespace

TEST(BacktestEngineTest, AdvancesClockBeforeHandlerRuns) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    const auto bar_time = start + 5s;

    std::vector<quantforge::market::Bar> bars{
        MakeBar(bar_time),
    };

    bool handler_called = false;

    engine.run(
        bars,
        [&](const quantforge::market::Bar& bar) {
            handler_called = true;
            EXPECT_EQ(clock.now(), bar.timestamp());
        }
    );

    EXPECT_TRUE(handler_called);
}

TEST(BacktestEngineTest, ProcessesBarsInProvidedOrder) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    std::vector<quantforge::market::Bar> bars{
        MakeBar(start),
        MakeBar(start + 1s),
        MakeBar(start + 2s),
    };

    std::vector<quantforge::market::Timestamp> processed;

    engine.run(
        bars,
        [&](const quantforge::market::Bar& bar) {
            processed.push_back(bar.timestamp());
        }
    );

    ASSERT_EQ(processed.size(), 3);
    EXPECT_EQ(processed[0], start);
    EXPECT_EQ(processed[1], start + 1s);
    EXPECT_EQ(processed[2], start + 2s);
}

TEST(BacktestEngineTest, ProcessesEveryBar) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    std::vector<quantforge::market::Bar> bars{
        MakeBar(start),
        MakeBar(start + 1s),
        MakeBar(start + 2s),
    };

    std::size_t count = 0;

    engine.run(
        bars,
        [&](const quantforge::market::Bar&) {
            ++count;
        }
    );

    EXPECT_EQ(count, 3);
}

TEST(BacktestEngineTest, EmptyBarSequenceDoesNothing) {
    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    const std::vector<quantforge::market::Bar> bars;

    std::size_t count = 0;

    engine.run(
        bars,
        [&](const quantforge::market::Bar&) {
            ++count;
        }
    );

    EXPECT_EQ(count, 0);
    EXPECT_EQ(clock.now(), start);
}

TEST(BacktestEngineTest, RejectsMissingHandler) {
    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    const std::vector<quantforge::market::Bar> bars;

    EXPECT_THROW(
        engine.run(bars, {}),
        std::invalid_argument
    );
}

TEST(BacktestEngineTest, RejectsBarsThatMoveTimeBackwards) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    std::vector<quantforge::market::Bar> bars{
        MakeBar(start + 2s),
        MakeBar(start + 1s),
    };

    std::size_t processed = 0;

    EXPECT_THROW(
        engine.run(
            bars,
            [&](const quantforge::market::Bar&) {
                ++processed;
            }
        ),
        std::logic_error
    );

    EXPECT_EQ(processed, 1);
    EXPECT_EQ(clock.now(), start + 2s);
}

namespace {

class LifecycleStrategy final : public quantforge::strategy::Strategy {
public:
    void on_start(
        const quantforge::strategy::StrategyContext&
    ) override {
        events.push_back("start");
    }

    void on_bar(
        const quantforge::strategy::StrategyContext&,
        const quantforge::market::Bar&
    ) override {
        events.push_back("bar");
        ++bar_count;
    }

    void on_finish(
        const quantforge::strategy::StrategyContext&
    ) override {
        events.push_back("finish");
    }

    std::vector<std::string> events;
    std::size_t bar_count{0};
};

} // namespace

TEST(BacktestEngineTest, RunsStrategyLifecycleInOrder) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);
    LifecycleStrategy strategy;

    std::vector<quantforge::market::Bar> bars{
        MakeBar(start),
        MakeBar(start + 1s),
    };

    engine.run(bars, strategy);

    ASSERT_EQ(strategy.events.size(), 4);
    EXPECT_EQ(strategy.events[0], "start");
    EXPECT_EQ(strategy.events[1], "bar");
    EXPECT_EQ(strategy.events[2], "bar");
    EXPECT_EQ(strategy.events[3], "finish");
}

TEST(BacktestEngineTest, CallsStrategyForEveryBar) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);
    LifecycleStrategy strategy;

    std::vector<quantforge::market::Bar> bars{
        MakeBar(start),
        MakeBar(start + 1s),
        MakeBar(start + 2s),
    };

    engine.run(bars, strategy);

    EXPECT_EQ(strategy.bar_count, 3);
}

TEST(BacktestEngineTest, AdvancesClockBeforeStrategyBar) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    class ClockAwareStrategy final
        : public quantforge::strategy::Strategy {
    public:
        void on_bar(
            const quantforge::strategy::StrategyContext& context,
            const quantforge::market::Bar& bar
        ) override {
            EXPECT_EQ(context.now(), bar.timestamp());
        }
    };

    ClockAwareStrategy strategy;

    const auto bar_time = start + 5s;

    std::vector<quantforge::market::Bar> bars{
        MakeBar(bar_time),
    };

    engine.run(bars, strategy);
}

TEST(BacktestEngineTest, CallsFinishAfterLastBar) {
    using namespace std::chrono_literals;

    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);

    class FinishAwareStrategy final
        : public quantforge::strategy::Strategy {
    public:
        void on_bar(
            const quantforge::strategy::StrategyContext&,
            const quantforge::market::Bar&
        ) override {}

        void on_finish(
            const quantforge::strategy::StrategyContext& context
        ) override {
            finish_time = context.now();
        }

        quantforge::market::Timestamp finish_time{};
    };

    FinishAwareStrategy strategy;

    const auto final_time = start + 10s;

    std::vector<quantforge::market::Bar> bars{
        MakeBar(final_time),
    };

    engine.run(bars, strategy);

    EXPECT_EQ(strategy.finish_time, final_time);
}

TEST(BacktestEngineTest, RunsEmptyStrategyBacktestLifecycle) {
    const auto start = quantforge::market::Timestamp{
        std::chrono::seconds{100}
    };

    quantforge::backtest::BacktestClock clock(start);
    quantforge::backtest::BacktestEngine engine(clock);
    LifecycleStrategy strategy;

    const std::vector<quantforge::market::Bar> bars;

    engine.run(bars, strategy);

    ASSERT_EQ(strategy.events.size(), 2);
    EXPECT_EQ(strategy.events[0], "start");
    EXPECT_EQ(strategy.events[1], "finish");

    EXPECT_EQ(clock.now(), start);
}


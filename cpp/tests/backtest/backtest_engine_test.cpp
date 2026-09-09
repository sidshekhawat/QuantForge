#include "quantforge/backtest/backtest_engine.hpp"

#include <chrono>
#include <stdexcept>
#include <vector>

#include <gtest/gtest.h>

namespace {

using quantforge::backtest::BacktestClock;
using quantforge::backtest::BacktestEngine;
using quantforge::market::Bar;
using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::market::Timeframe;
using quantforge::market::TimeframeUnit;
using quantforge::market::Timestamp;

Bar make_bar(
    std::int64_t timestamp_seconds,
    std::int64_t close)
{
    const auto timestamp =
        Timestamp{
            std::chrono::seconds{timestamp_seconds}
        };

    const Price open{close, 0};
    const Price high{close, 0};
    const Price low{close, 0};

    return Bar{
        InstrumentId{1},
        timestamp,
        Timeframe{1, TimeframeUnit::Minute},
        open,
        high,
        low,
        Price{close, 0},
        Quantity{100, 0}
    };
}

TEST(BacktestEngineTest, AdvancesClockBeforeHandlerRuns)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const auto bar =
        make_bar(160, 250);

    BacktestClock clock{start_time};
    BacktestEngine engine{clock};

    Timestamp observed_time{};

    engine.run(
        std::vector<Bar>{bar},
        [&](const Bar&) {
            observed_time = clock.now();
        }
    );

    EXPECT_EQ(observed_time, bar.timestamp());
    EXPECT_EQ(clock.now(), bar.timestamp());
}

TEST(BacktestEngineTest, ProcessesBarsInProvidedOrder)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const auto first_bar = make_bar(160, 250);
    const auto second_bar = make_bar(220, 255);
    const auto third_bar = make_bar(280, 260);

    BacktestClock clock{start_time};
    BacktestEngine engine{clock};

    std::vector<Timestamp> observed_times;

    engine.run(
        std::vector<Bar>{
            first_bar,
            second_bar,
            third_bar
        },
        [&](const Bar& bar) {
            observed_times.push_back(bar.timestamp());
        }
    );

    ASSERT_EQ(observed_times.size(), 3U);
    EXPECT_EQ(observed_times[0], first_bar.timestamp());
    EXPECT_EQ(observed_times[1], second_bar.timestamp());
    EXPECT_EQ(observed_times[2], third_bar.timestamp());
}

TEST(BacktestEngineTest, ProcessesEveryBar)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    const auto first_bar = make_bar(160, 250);
    const auto second_bar = make_bar(220, 255);
    const auto third_bar = make_bar(280, 260);

    BacktestClock clock{start_time};
    BacktestEngine engine{clock};

    std::size_t call_count = 0;

    engine.run(
        std::vector<Bar>{
            first_bar,
            second_bar,
            third_bar
        },
        [&](const Bar&) {
            ++call_count;
        }
    );

    EXPECT_EQ(call_count, 3U);
}

TEST(BacktestEngineTest, EmptyBarSequenceDoesNothing)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    BacktestClock clock{start_time};
    BacktestEngine engine{clock};

    std::size_t call_count = 0;

    engine.run(
        {},
        [&](const Bar&) {
            ++call_count;
        }
    );

    EXPECT_EQ(call_count, 0U);
    EXPECT_EQ(clock.now(), start_time);
}

TEST(BacktestEngineTest, RejectsMissingHandler)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{100}
        };

    BacktestClock clock{start_time};
    BacktestEngine engine{clock};

    EXPECT_THROW(
        engine.run(
            {},
            BacktestEngine::BarHandler{}
        ),
        std::invalid_argument
    );
}

TEST(BacktestEngineTest, RejectsBarsThatMoveTimeBackwards)
{
    const auto start_time =
        Timestamp{
            std::chrono::seconds{200}
        };

    const auto earlier_bar = make_bar(160, 250);

    BacktestClock clock{start_time};
    BacktestEngine engine{clock};

    bool handler_called = false;

    EXPECT_THROW(
        engine.run(
            std::vector<Bar>{earlier_bar},
            [&](const Bar&) {
                handler_called = true;
            }
        ),
        std::logic_error
    );

    EXPECT_FALSE(handler_called);
    EXPECT_EQ(clock.now(), start_time);
}

} // namespace

namespace {

class LifecycleStrategy final : public quantforge::strategy::Strategy {
public:
    void on_start() override {
        events.push_back("start");
    }

    void on_bar(const quantforge::market::Bar& bar) override {
        events.push_back("bar");

        timestamps.push_back(bar.timestamp());
        clock_times.push_back(clock_time);

        ++bar_count;
    }

    void on_finish() override {
        events.push_back("finish");
    }

    std::vector<std::string> events;
    std::vector<quantforge::market::Timestamp> timestamps;
    std::vector<quantforge::market::Timestamp> clock_times;
    std::size_t bar_count{0};
    quantforge::market::Timestamp clock_time{};
};

} // namespace

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

    class ClockAwareStrategy final : public quantforge::strategy::Strategy {
    public:
        explicit ClockAwareStrategy(
            quantforge::backtest::BacktestClock& clock
        )
            : clock_(clock) {}

        void on_bar(
            const quantforge::market::Bar& bar
        ) override {
            EXPECT_EQ(clock_.now(), bar.timestamp());
        }

    private:
        quantforge::backtest::BacktestClock& clock_;
    };

    ClockAwareStrategy strategy(clock);

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

    class FinishAwareStrategy final : public quantforge::strategy::Strategy {
    public:
        explicit FinishAwareStrategy(
            quantforge::backtest::BacktestClock& clock
        )
            : clock_(clock) {}

        void on_bar(const quantforge::market::Bar&) override {}

        void on_finish() override {
            finish_time = clock_.now();
        }

        quantforge::market::Timestamp finish_time{};

    private:
        quantforge::backtest::BacktestClock& clock_;
    };

    FinishAwareStrategy strategy(clock);

    const auto final_time = start + 10s;

    std::vector<quantforge::market::Bar> bars{
        MakeBar(final_time),
    };

    engine.run(bars, strategy);

    EXPECT_EQ(strategy.finish_time, final_time);
}

TEST(BacktestEngineTest, RunsEmptyStrategyBacktestLifecycle) {
    using namespace std::chrono_literals;

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


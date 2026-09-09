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

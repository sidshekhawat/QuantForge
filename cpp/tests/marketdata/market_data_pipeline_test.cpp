#include "quantforge/marketdata/market_data_pipeline.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <vector>

namespace quantforge::marketdata {

namespace {

class FakeMarketDataSource final : public MarketDataSource {
public:
    std::vector<market::Bar> bars_to_return;

    int call_count{0};

    MarketDataRequest last_request{
        market::InstrumentId{0},
        market::Timestamp{},
        market::Timestamp{},
        market::Timeframe{
            1,
            market::TimeframeUnit::Minute
        }
    };

    [[nodiscard]] std::vector<market::Bar> get_bars(
        const MarketDataRequest& request
    ) override
    {
        ++call_count;
        last_request = request;
        return bars_to_return;
    }
};

market::Bar make_bar(
    std::uint64_t instrument_id,
    std::int64_t minute
)
{
    return market::Bar{
        market::InstrumentId{instrument_id},
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{minute}
        },
        market::Timeframe{
            5,
            market::TimeframeUnit::Minute
        },
        market::Price{250000, 2},
        market::Price{251000, 2},
        market::Price{249000, 2},
        market::Price{250500, 2},
        market::Quantity{100000, 0}
    };
}

} // namespace

TEST(MarketDataPipelineTest, ForwardsRequestToSource)
{
    FakeMarketDataSource source;
    MarketDataPipeline pipeline{source};

    const MarketDataRequest request{
        market::InstrumentId{42},
        market::Timestamp{},
        market::Timestamp{} + std::chrono::hours{1},
        market::Timeframe{
            5,
            market::TimeframeUnit::Minute
        }
    };

    static_cast<void>(pipeline.get_bars(request));

    EXPECT_EQ(source.call_count, 1);
    EXPECT_EQ(source.last_request.instrument_id(), request.instrument_id());
    EXPECT_EQ(source.last_request.start(), request.start());
    EXPECT_EQ(source.last_request.end(), request.end());
    EXPECT_EQ(source.last_request.timeframe(), request.timeframe());
}

TEST(MarketDataPipelineTest, ReturnsSourceResultUnchanged)
{
    FakeMarketDataSource source;

    source.bars_to_return = {
        make_bar(42, 20),
        make_bar(42, 15),
        make_bar(42, 25)
    };

    MarketDataPipeline pipeline{source};

    const MarketDataRequest request{
        market::InstrumentId{42},
        market::Timestamp{},
        market::Timestamp{} + std::chrono::hours{1},
        market::Timeframe{
            5,
            market::TimeframeUnit::Minute
        }
    };

    const auto bars = pipeline.get_bars(request);

    ASSERT_EQ(bars.size(), 3);

    EXPECT_EQ(
        bars[0].timestamp(),
        source.bars_to_return[0].timestamp()
    );

    EXPECT_EQ(
        bars[1].timestamp(),
        source.bars_to_return[1].timestamp()
    );

    EXPECT_EQ(
        bars[2].timestamp(),
        source.bars_to_return[2].timestamp()
    );
}

TEST(MarketDataPipelineTest, ReturnsEmptySourceResult)
{
    FakeMarketDataSource source;
    MarketDataPipeline pipeline{source};

    const MarketDataRequest request{
        market::InstrumentId{42},
        market::Timestamp{},
        market::Timestamp{} + std::chrono::hours{1},
        market::Timeframe{
            5,
            market::TimeframeUnit::Minute
        }
    };

    const auto bars = pipeline.get_bars(request);

    EXPECT_TRUE(bars.empty());
}

} // namespace quantforge::marketdata

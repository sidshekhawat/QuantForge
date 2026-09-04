#include "quantforge/marketdata/market_data_request.hpp"

#include <gtest/gtest.h>

namespace quantforge::marketdata {

TEST(MarketDataRequestTest, StoresRequestFields)
{
    const market::InstrumentId instrument_id{42};

    const market::Timestamp start{
        std::chrono::seconds{100}
    };

    const market::Timestamp end{
        std::chrono::seconds{200}
    };

    const market::Timeframe timeframe{
        5,
        market::TimeframeUnit::Minute
    };

    const MarketDataRequest request{
        instrument_id,
        start,
        end,
        timeframe
    };

    EXPECT_EQ(request.instrument_id(), instrument_id);
    EXPECT_EQ(request.start(), start);
    EXPECT_EQ(request.end(), end);
    EXPECT_EQ(request.timeframe(), timeframe);
}

TEST(MarketDataRequestTest, SupportsExclusiveEndSemantics)
{
    const market::Timestamp start{
        std::chrono::seconds{100}
    };

    const market::Timestamp end{
        std::chrono::seconds{200}
    };

    const MarketDataRequest request{
        market::InstrumentId{1},
        start,
        end,
        market::Timeframe{
            1,
            market::TimeframeUnit::Minute
        }
    };

    EXPECT_LT(request.start(), request.end());
}

} // namespace quantforge::marketdata

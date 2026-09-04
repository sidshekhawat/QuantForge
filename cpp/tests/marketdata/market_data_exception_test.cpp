#include "quantforge/marketdata/market_data_exception.hpp"

#include <gtest/gtest.h>

namespace quantforge::marketdata {

TEST(MarketDataExceptionTest, StoresRowNumber)
{
    const MarketDataException exception{
        1842,
        "Invalid timeframe."
    };

    EXPECT_EQ(exception.row_number(), 1842);
}

TEST(MarketDataExceptionTest, BuildsUsefulMessage)
{
    const MarketDataException exception{
        1842,
        "Invalid timeframe."
    };

    EXPECT_STREQ(
        exception.what(),
        "Market data error at row 1842: Invalid timeframe."
    );
}

TEST(MarketDataExceptionTest, CanBeCaughtAsRuntimeError)
{
    EXPECT_THROW(
        (throw MarketDataException{
            10,
            "Invalid data."
        }),
        std::runtime_error
    );
}

} // namespace quantforge::marketdata

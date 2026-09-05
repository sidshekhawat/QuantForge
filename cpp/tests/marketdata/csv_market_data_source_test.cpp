#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/market_data_exception.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>

namespace quantforge::marketdata {

TEST(CsvMarketDataSourceTest, CanBeConstructed)
{
    CsvMarketDataSource source{
        std::filesystem::path{"data/test.csv"}
    };

    SUCCEED();
}

TEST(CsvMarketDataSourceTest, ThrowsWhenFileCannotBeOpened)
{
    CsvMarketDataSource source{
        std::filesystem::path{
            "data/does_not_exist.csv"
        }
    };

    const MarketDataRequest request{
        market::InstrumentId{1},
        market::Timestamp{},
        market::Timestamp{} + std::chrono::hours{1},
        market::Timeframe{
            1,
            market::TimeframeUnit::Hour
        }
    };

    try {
        static_cast<void>(source.get_bars(request));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 0);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 0: "
            "Unable to open market data file: data/does_not_exist.csv"
        );
    }
    catch (...) {
        FAIL() << "Expected MarketDataException.";
    }
}

} // namespace quantforge::marketdata

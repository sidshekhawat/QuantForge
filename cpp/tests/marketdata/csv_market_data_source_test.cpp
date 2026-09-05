#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/market_data_exception.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>

namespace quantforge::marketdata {

namespace {

constexpr const char* kValidHeader =
    "timestamp,instrument_id,timeframe,open,high,low,close,volume,price_scale,volume_scale";

const MarketDataRequest make_request()
{
    return MarketDataRequest{
        market::InstrumentId{1},
        market::Timestamp{},
        market::Timestamp{} + std::chrono::hours{1},
        market::Timeframe{
            1,
            market::TimeframeUnit::Hour
        }
    };
}

std::filesystem::path make_test_path(const char* filename)
{
    return std::filesystem::temp_directory_path() / filename;
}

} // namespace

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

    try {
        static_cast<void>(source.get_bars(make_request()));
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

TEST(CsvMarketDataSourceTest, AcceptsValidHeader)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_valid_header.csv");

    {
        std::ofstream file{path};
        ASSERT_TRUE(file.is_open());

        file << kValidHeader << '\n';
    }

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsMissingHeader)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_missing_header.csv");

    {
        std::ofstream file{path};
        ASSERT_TRUE(file.is_open());
    }

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 1);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 1: Missing CSV header."
        );
    }
    catch (...) {
        FAIL() << "Expected MarketDataException.";
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsInvalidHeader)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_invalid_header.csv");

    {
        std::ofstream file{path};
        ASSERT_TRUE(file.is_open());

        file << "timestamp,instrument_id,open,close\n";
    }

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 1);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 1: Invalid CSV header."
        );
    }
    catch (...) {
        FAIL() << "Expected MarketDataException.";
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsReorderedHeader)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_reordered_header.csv");

    {
        std::ofstream file{path};
        ASSERT_TRUE(file.is_open());

        file
            << "instrument_id,timestamp,timeframe,open,high,low,"
               "close,volume,price_scale,volume_scale\n";
    }

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 1);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 1: Invalid CSV header."
        );
    }
    catch (...) {
        FAIL() << "Expected MarketDataException.";
    }

    std::filesystem::remove(path);
}

} // namespace quantforge::marketdata

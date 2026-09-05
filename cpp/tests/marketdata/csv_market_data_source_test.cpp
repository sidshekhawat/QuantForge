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

constexpr const char* kValidRow =
    "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,0";

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

void write_csv(
    const std::filesystem::path& path,
    const char* row
)
{
    std::ofstream file{path};
    ASSERT_TRUE(file.is_open());

    file << kValidHeader << '\n';
    file << row << '\n';
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

TEST(CsvMarketDataSourceTest, AcceptsValidInstrumentId)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_valid_instrument_id.csv");

    write_csv(path, kValidRow);

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyInstrumentId)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_empty_instrument_id.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 2);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 2: "
            "Invalid instrument_id: value is empty."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNonNumericInstrumentId)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_invalid_instrument_id.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,ABC,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 2);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 2: "
            "Invalid instrument_id: expected an unsigned integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsDecimalInstrumentId)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_decimal_instrument_id.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1.5,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 2);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 2: "
            "Invalid instrument_id: expected an unsigned integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNegativeInstrumentId)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_negative_instrument_id.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,-1,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 2);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 2: "
            "Invalid instrument_id: expected an unsigned integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsIncorrectFieldCount)
{
    const std::filesystem::path path =
        make_test_path("quantforge_test_invalid_field_count.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000"
    );

    CsvMarketDataSource source{path};

    try {
        static_cast<void>(source.get_bars(make_request()));
        FAIL() << "Expected MarketDataException.";
    }
    catch (const MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 2);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 2: "
            "Invalid CSV row: expected 10 fields."
        );
    }

    std::filesystem::remove(path);
}

}

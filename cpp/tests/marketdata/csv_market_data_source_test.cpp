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

std::filesystem::path make_test_path(const char* filename)
{
    return std::filesystem::temp_directory_path() / filename;
}

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

void expect_timestamp_rejected(
    const char* filename,
    const char* timestamp
)
{
    const std::filesystem::path path =
        make_test_path(filename);

    write_csv(
        path,
        (
            std::string{timestamp}
            + ",1,1m,250000,251050,249875,250825,125000,2,0"
        ).c_str()
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
            "Invalid timestamp: expected UTC ISO-8601 format."
        );
    }

    std::filesystem::remove(path);
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
}

TEST(CsvMarketDataSourceTest, AcceptsValidHeader)
{
    const auto path =
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
    const auto path =
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

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsInvalidHeader)
{
    const auto path =
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

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsReorderedHeader)
{
    const auto path =
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

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsValidInstrumentId)
{
    const auto path =
        make_test_path("quantforge_test_valid_instrument_id.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyInstrumentId)
{
    const auto path =
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
    const auto path =
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
    const auto path =
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
    const auto path =
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
    const auto path =
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

TEST(CsvMarketDataSourceTest, AcceptsValidTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_valid_timeframe.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,5m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_empty_timeframe.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,,250000,251050,249875,250825,125000,2,0"
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
            "Invalid timeframe: expected canonical format such as 1m or 1h."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsInvalidTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_invalid_timeframe.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,5x,250000,251050,249875,250825,125000,2,0"
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
            "Invalid timeframe: expected canonical format such as 1m or 1h."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsZeroTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_zero_timeframe.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,0m,250000,251050,249875,250825,125000,2,0"
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
            "Invalid timeframe: expected canonical format such as 1m or 1h."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsUppercaseTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_uppercase_timeframe.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,5M,250000,251050,249875,250825,125000,2,0"
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
            "Invalid timeframe: expected canonical format such as 1m or 1h."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsTimestampWithoutFraction)
{
    const auto path =
        make_test_path("quantforge_test_timestamp_seconds.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsNanosecondTimestamp)
{
    const auto path =
        make_test_path("quantforge_test_timestamp_nanoseconds.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00.123456789Z,1,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsMidnightTimestamp)
{
    const auto path =
        make_test_path("quantforge_test_timestamp_midnight.csv");

    write_csv(
        path,
        "2026-01-02T00:00:00Z,1,1m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsTimestampWithoutZuluSuffix)
{
    expect_timestamp_rejected(
        "quantforge_test_timestamp_no_z.csv",
        "2026-01-02T09:15:00"
    );
}

TEST(CsvMarketDataSourceTest, RejectsTimestampWithTimezoneOffset)
{
    expect_timestamp_rejected(
        "quantforge_test_timestamp_offset.csv",
        "2026-01-02T09:15:00+05:30"
    );
}

TEST(CsvMarketDataSourceTest, RejectsInvalidCalendarDate)
{
    expect_timestamp_rejected(
        "quantforge_test_timestamp_invalid_date.csv",
        "2026-02-30T09:15:00Z"
    );
}

TEST(CsvMarketDataSourceTest, RejectsInvalidTime)
{
    expect_timestamp_rejected(
        "quantforge_test_timestamp_invalid_time.csv",
        "2026-01-02T25:15:00Z"
    );
}

TEST(CsvMarketDataSourceTest, RejectsTooManyFractionalDigits)
{
    expect_timestamp_rejected(
        "quantforge_test_timestamp_too_precise.csv",
        "2026-01-02T09:15:00.1234567890Z"
    );
}

}

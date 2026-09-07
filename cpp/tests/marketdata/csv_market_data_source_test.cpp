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


TEST(CsvMarketDataSourceTest, AcceptsValidPrices)
{
    const auto path =
        make_test_path("quantforge_test_valid_prices.csv");

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

TEST(CsvMarketDataSourceTest, AcceptsNegativeRawPrice)
{
    const auto path =
        make_test_path("quantforge_test_negative_price.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,-250000,-249000,-251000,-249500,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsZeroPrice)
{
    const auto path =
        make_test_path("quantforge_test_zero_price.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,0,251050,0,250000,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyPrice)
{
    const auto path =
        make_test_path("quantforge_test_empty_price.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,,251050,249875,250825,125000,2,0"
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
            "Invalid price: value is empty."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNonNumericPrice)
{
    const auto path =
        make_test_path("quantforge_test_invalid_price.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,ABC,251050,249875,250825,125000,2,0"
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
            "Invalid price: expected a signed integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsDecimalPrice)
{
    const auto path =
        make_test_path("quantforge_test_decimal_price.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,2500.50,251050,249875,250825,125000,2,0"
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
            "Invalid price: expected a signed integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyPriceScale)
{
    const auto path =
        make_test_path("quantforge_test_empty_price_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,,0"
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
            "Invalid price_scale: value is empty."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNonNumericPriceScale)
{
    const auto path =
        make_test_path("quantforge_test_invalid_price_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,ABC,0"
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
            "Invalid price_scale: expected an integer from 0 to 255."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsPriceScaleAboveUint8Range)
{
    const auto path =
        make_test_path("quantforge_test_large_price_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,256,0"
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
            "Invalid price_scale: expected an integer from 0 to 255."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsMaximumPriceScale)
{
    const auto path =
        make_test_path("quantforge_test_max_price_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,255,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}


}

namespace quantforge::marketdata {

TEST(CsvMarketDataSourceTest, AcceptsValidVolume)
{
    const auto path =
        make_test_path("quantforge_test_valid_volume.csv");

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

TEST(CsvMarketDataSourceTest, AcceptsZeroVolume)
{
    const auto path =
        make_test_path("quantforge_test_zero_volume.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,0,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNegativeRawVolume)
{
    const auto path =
        make_test_path("quantforge_test_negative_volume.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,-125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_THROW(
        static_cast<void>(source.get_bars(make_request())),
        MarketDataException
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyVolume)
{
    const auto path =
        make_test_path("quantforge_test_empty_volume.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,,2,0"
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
            "Invalid volume: value is empty."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNonNumericVolume)
{
    const auto path =
        make_test_path("quantforge_test_invalid_volume.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,ABC,2,0"
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
            "Invalid volume: expected a signed integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsDecimalVolume)
{
    const auto path =
        make_test_path("quantforge_test_decimal_volume.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,1250.50,2,0"
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
            "Invalid volume: expected a signed integer."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsEmptyVolumeScale)
{
    const auto path =
        make_test_path("quantforge_test_empty_volume_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,"
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
            "Invalid volume_scale: value is empty."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNonNumericVolumeScale)
{
    const auto path =
        make_test_path("quantforge_test_invalid_volume_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,ABC"
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
            "Invalid volume_scale: expected an integer from 0 to 255."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsVolumeScaleAboveUint8Range)
{
    const auto path =
        make_test_path("quantforge_test_large_volume_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,256"
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
            "Invalid volume_scale: expected an integer from 0 to 255."
        );
    }

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, AcceptsMaximumVolumeScale)
{
    const auto path =
        make_test_path("quantforge_test_max_volume_scale.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,1,1m,250000,251050,249875,250825,125000,2,255"
    );

    CsvMarketDataSource source{path};

    EXPECT_NO_THROW(
        static_cast<void>(source.get_bars(make_request()))
    );

    std::filesystem::remove(path);
}


TEST(CsvMarketDataSourceTest, ConstructsBarFromCsvRow)
{
    const auto path =
        make_test_path("quantforge_test_constructs_bar.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00.123456789Z,42,5m,250000,251050,249875,250825,125000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
            },
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{3}
                }
            },
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 1);

    const auto& bar = bars.front();

    EXPECT_EQ(
        bar.instrument_id(),
        market::InstrumentId{42}
    );

    EXPECT_EQ(
        bar.timestamp(),
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{15}
            + std::chrono::nanoseconds{123456789}
        }
    );

    EXPECT_EQ(
        bar.timeframe(),
        (market::Timeframe{
            5,
            market::TimeframeUnit::Minute
        })
    );

    EXPECT_EQ(
        bar.open(),
        (market::Price{250000, 2})
    );

    EXPECT_EQ(
        bar.high(),
        (market::Price{251050, 2})
    );

    EXPECT_EQ(
        bar.low(),
        (market::Price{249875, 2})
    );

    EXPECT_EQ(
        bar.close(),
        (market::Price{250825, 2})
    );

    EXPECT_EQ(
        bar.volume(),
        (market::Quantity{125000, 0})
    );

    std::filesystem::remove(path);
}


TEST(CsvMarketDataSourceTest, FiltersBarsByInstrumentId)
{
    const auto path =
        make_test_path("quantforge_test_filter_instrument.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,125000,2,0\n"
        "2026-01-02T09:20:00Z,99,5m,300000,301000,299000,300500,50000,2,0\n"
        "2026-01-02T09:25:00Z,42,5m,251000,252000,250500,251500,100000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
            },
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{3}
                }
            },
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 2);
    EXPECT_EQ(bars[0].instrument_id(), market::InstrumentId{42});
    EXPECT_EQ(bars[1].instrument_id(), market::InstrumentId{42});

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, FiltersBarsByTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_filter_timeframe.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,125000,2,0\n"
        "2026-01-02T09:20:00Z,42,1m,251000,251500,250500,251250,100000,2,0\n"
        "2026-01-02T09:25:00Z,42,5m,252000,253000,251500,252500,110000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
            },
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{3}
                }
            },
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 2);
    EXPECT_EQ(
        bars[0].timeframe(),
        (market::Timeframe{5, market::TimeframeUnit::Minute})
    );
    EXPECT_EQ(
        bars[1].timeframe(),
        (market::Timeframe{5, market::TimeframeUnit::Minute})
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, FiltersBarsByExclusiveTimeRange)
{
    const auto path =
        make_test_path("quantforge_test_filter_time.csv");

    write_csv(
        path,
        "2026-01-02T09:00:00Z,42,5m,249000,250000,248500,249500,100000,2,0\n"
        "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,125000,2,0\n"
        "2026-01-02T09:30:00Z,42,5m,251000,252000,250500,251500,100000,2,0\n"
        "2026-01-02T09:45:00Z,42,5m,252000,253000,251500,252500,90000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
                + std::chrono::hours{9}
                + std::chrono::minutes{15}
            },
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
                + std::chrono::hours{9}
                + std::chrono::minutes{45}
            },
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 2);

    EXPECT_EQ(
        bars[0].timestamp(),
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{15}
        }
    );

    EXPECT_EQ(
        bars[1].timestamp(),
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{30}
        }
    );

    std::filesystem::remove(path);
}


TEST(CsvMarketDataSourceTest, PreservesCsvRowOrder)
{
    const auto path =
        make_test_path("quantforge_test_preserves_csv_order.csv");

    write_csv(
        path,
        "2026-01-02T09:30:00Z,42,5m,252000,253000,251500,252500,100000,2,0\n"
        "2026-01-02T09:15:00Z,42,5m,250000,251000,249500,250500,90000,2,0\n"
        "2026-01-02T09:20:00Z,42,5m,251000,252000,250500,251500,95000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
            },
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{3}
                }
            },
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 3);

    EXPECT_EQ(
        bars[0].timestamp(),
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{30}
        }
    );

    EXPECT_EQ(
        bars[1].timestamp(),
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{15}
        }
    );

    EXPECT_EQ(
        bars[2].timestamp(),
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{2026}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
            + std::chrono::hours{9}
            + std::chrono::minutes{20}
        }
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, PreservesSourceOrder)
{
    const auto path =
        make_test_path("quantforge_test_source_order.csv");

    write_csv(
        path,
        "2026-01-02T09:20:00Z,42,5m,252000,253000,251500,252500,100000,2,0"
    );

    {
        std::ofstream file{path, std::ios::app};
        ASSERT_TRUE(file.is_open());

        file
            << "2026-01-02T09:15:00Z,42,5m,"
               "250000,251000,249500,250500,100000,2,0"
            << '\n';

        file
            << "2026-01-02T09:25:00Z,42,5m,"
               "253000,254000,252500,253500,100000,2,0"
            << '\n';
    }

    CsvMarketDataSource source{path};

    const auto start = market::Timestamp{
        std::chrono::sys_days{
            std::chrono::year{2026}
            / std::chrono::month{1}
            / std::chrono::day{2}
        }
        + std::chrono::hours{9}
        + std::chrono::minutes{15}
    };

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            start,
            start + std::chrono::minutes{15},
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 3);

    EXPECT_EQ(
        bars[0].timestamp(),
        start + std::chrono::minutes{5}
    );

    EXPECT_EQ(
        bars[1].timestamp(),
        start
    );

    EXPECT_EQ(
        bars[2].timestamp(),
        start + std::chrono::minutes{10}
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, IncludesBarAtStartBoundary)
{
    const auto path =
        make_test_path("quantforge_test_start_boundary.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,42,5m,250000,251000,249500,250500,100000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto start = market::Timestamp{
        std::chrono::sys_days{
            std::chrono::year{2026}
            / std::chrono::month{1}
            / std::chrono::day{2}
        }
        + std::chrono::hours{9}
        + std::chrono::minutes{15}
    };

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            start,
            start + std::chrono::minutes{5},
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 1);
    EXPECT_EQ(bars.front().timestamp(), start);

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, ExcludesBarAtEndBoundary)
{
    const auto path =
        make_test_path("quantforge_test_end_boundary.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,42,5m,250000,251000,249500,250500,100000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto end = market::Timestamp{
        std::chrono::sys_days{
            std::chrono::year{2026}
            / std::chrono::month{1}
            / std::chrono::day{2}
        }
        + std::chrono::hours{9}
        + std::chrono::minutes{15}
    };

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{42},
            end - std::chrono::minutes{5},
            end,
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    ASSERT_EQ(bars.size(), 0);

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, ReturnsEmptyResultWhenNoBarsMatch)
{
    const auto path =
        make_test_path("quantforge_test_empty_result.csv");

    write_csv(
        path,
        "2026-01-02T09:15:00Z,42,5m,250000,251000,249500,250500,100000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto bars = source.get_bars(
        MarketDataRequest{
            market::InstrumentId{99},
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{2}
                }
            },
            market::Timestamp{
                std::chrono::sys_days{
                    std::chrono::year{2026}
                    / std::chrono::month{1}
                    / std::chrono::day{3}
                }
            },
            market::Timeframe{
                5,
                market::TimeframeUnit::Minute
            }
        }
    );

    EXPECT_TRUE(bars.empty());

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsBarWithHighBelowOpen)
{
    const auto path =
        make_test_path("quantforge_test_invalid_high.csv");

    write_csv(
        path,
        "1970-01-01T00:00:00Z,1,1h,250000,249000,249000,250000,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_THROW(
        static_cast<void>(source.get_bars(make_request())),
        MarketDataException
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsBarWithLowAboveClose)
{
    const auto path =
        make_test_path("quantforge_test_invalid_low.csv");

    write_csv(
        path,
        "1970-01-01T00:00:00Z,1,1h,250000,251000,251500,250500,125000,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_THROW(
        static_cast<void>(source.get_bars(make_request())),
        MarketDataException
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsNegativeVolume)
{
    const auto path =
        make_test_path("quantforge_test_negative_volume.csv");

    write_csv(
        path,
        "1970-01-01T00:00:00Z,1,1h,250000,251000,249000,250500,-1,2,0"
    );

    CsvMarketDataSource source{path};

    EXPECT_THROW(
        static_cast<void>(source.get_bars(make_request())),
        MarketDataException
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsRequestWithStartAfterEnd)
{
    const auto path =
        make_test_path("quantforge_test_invalid_request_order.csv");

    write_csv(
        path,
        "1970-01-01T00:00:00Z,1,1h,250000,251000,249000,250500,125000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto start =
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{1970}
                / std::chrono::month{1}
                / std::chrono::day{2}
            }
        };

    const auto end =
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{1970}
                / std::chrono::month{1}
                / std::chrono::day{1}
            }
        };

    EXPECT_THROW(
        static_cast<void>(
            source.get_bars(
                MarketDataRequest{
                    market::InstrumentId{1},
                    start,
                    end,
                    market::Timeframe{
                        1,
                        market::TimeframeUnit::Hour
                    }
                }
            )
        ),
        MarketDataException
    );

    std::filesystem::remove(path);
}

TEST(CsvMarketDataSourceTest, RejectsZeroValueRequestTimeframe)
{
    const auto path =
        make_test_path("quantforge_test_zero_request_timeframe.csv");

    write_csv(
        path,
        "1970-01-01T00:00:00Z,1,1h,250000,251000,249000,250500,125000,2,0"
    );

    CsvMarketDataSource source{path};

    const auto start =
        market::Timestamp{
            std::chrono::sys_days{
                std::chrono::year{1970}
                / std::chrono::month{1}
                / std::chrono::day{1}
            }
        };

    EXPECT_THROW(
        static_cast<void>(
            source.get_bars(
                MarketDataRequest{
                    market::InstrumentId{1},
                    start,
                    start + std::chrono::hours{1},
                    market::Timeframe{
                        0,
                        market::TimeframeUnit::Hour
                    }
                }
            )
        ),
        MarketDataException
    );

    std::filesystem::remove(path);
}

} // namespace quantforge::marketdata

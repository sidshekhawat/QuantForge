#include "quantforge/marketdata/csv_bar_parser.hpp"
#include "quantforge/marketdata/market_data_exception.hpp"

#include <gtest/gtest.h>

#include <string>

namespace {

constexpr const char* kValidRow =
    "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,125000,2,0";

TEST(CsvBarParserTest, ParsesValidRow)
{
    const auto bar =
        quantforge::marketdata::CsvBarParser::parse(kValidRow, 2);

    EXPECT_EQ(
        bar.instrument_id(),
        quantforge::market::InstrumentId{42}
    );

    EXPECT_EQ(
        bar.timeframe(),
        (quantforge::market::Timeframe{
            5,
            quantforge::market::TimeframeUnit::Minute
        })
    );

    EXPECT_EQ(
        bar.open(),
        (quantforge::market::Price{250000, 2})
    );

    EXPECT_EQ(
        bar.high(),
        (quantforge::market::Price{251050, 2})
    );

    EXPECT_EQ(
        bar.low(),
        (quantforge::market::Price{249875, 2})
    );

    EXPECT_EQ(
        bar.close(),
        (quantforge::market::Price{250825, 2})
    );

    EXPECT_EQ(
        bar.volume(),
        (quantforge::market::Quantity{125000, 0})
    );
}

TEST(CsvBarParserTest, RejectsInvalidFieldCount)
{
    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(
            "2026-01-02T09:15:00Z,42,5m",
            7
        ),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, RejectsInvalidTimestamp)
{
    const std::string row =
        "invalid,42,5m,250000,251050,249875,250825,125000,2,0";

    try {
        (void)quantforge::marketdata::CsvBarParser::parse(row, 8);
        FAIL();
    }
    catch (const quantforge::marketdata::MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 8);
        EXPECT_STREQ(
            exception.what(),
            "Market data error at row 8: "
            "Invalid timestamp: expected UTC ISO-8601 format."
        );
    }
}

TEST(CsvBarParserTest, RejectsInvalidInstrumentId)
{
    const std::string row =
        "2026-01-02T09:15:00Z,invalid,5m,250000,251050,249875,250825,125000,2,0";

    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(row, 9),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, RejectsInvalidTimeframe)
{
    const std::string row =
        "2026-01-02T09:15:00Z,42,invalid,250000,251050,249875,250825,125000,2,0";

    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(row, 10),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, RejectsInvalidPrice)
{
    const std::string row =
        "2026-01-02T09:15:00Z,42,5m,invalid,251050,249875,250825,125000,2,0";

    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(row, 11),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, RejectsInvalidVolume)
{
    const std::string row =
        "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,invalid,2,0";

    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(row, 12),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, RejectsInvalidPriceScale)
{
    const std::string row =
        "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,125000,256,0";

    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(row, 13),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, RejectsInvalidVolumeScale)
{
    const std::string row =
        "2026-01-02T09:15:00Z,42,5m,250000,251050,249875,250825,125000,2,256";

    EXPECT_THROW(
        (void)quantforge::marketdata::CsvBarParser::parse(row, 14),
        quantforge::marketdata::MarketDataException
    );
}

TEST(CsvBarParserTest, PreservesRowNumberInException)
{
    const std::string row =
        "2026-01-02T09:15:00Z,42,5m,invalid,251050,249875,250825,125000,2,0";

    try {
        (void)quantforge::marketdata::CsvBarParser::parse(row, 42);
        FAIL();
    }
    catch (const quantforge::marketdata::MarketDataException& exception) {
        EXPECT_EQ(exception.row_number(), 42);
    }
}

}
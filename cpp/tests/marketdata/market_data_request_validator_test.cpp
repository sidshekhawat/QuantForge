#include "quantforge/marketdata/market_data_request_validator.hpp"

#include <gtest/gtest.h>

namespace quantforge::marketdata {

namespace {

MarketDataRequest makeRequest(
    market::Timestamp start,
    market::Timestamp end,
    market::Timeframe timeframe = {
        1,
        market::TimeframeUnit::Minute
    }
)
{
    return MarketDataRequest{
        market::InstrumentId{1},
        start,
        end,
        timeframe
    };
}

} // namespace

TEST(MarketDataRequestValidatorTest, AcceptsValidRequest)
{
    const auto request = makeRequest(
        market::Timestamp{std::chrono::seconds{100}},
        market::Timestamp{std::chrono::seconds{200}}
    );

    const auto result = MarketDataRequestValidator::validate(request);

    EXPECT_TRUE(result.valid());
    EXPECT_TRUE(result.message().empty());
}

TEST(MarketDataRequestValidatorTest, RejectsEqualStartAndEnd)
{
    const auto timestamp =
        market::Timestamp{std::chrono::seconds{100}};

    const auto request = makeRequest(timestamp, timestamp);

    const auto result = MarketDataRequestValidator::validate(request);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Market data request start must be before end."
    );
}

TEST(MarketDataRequestValidatorTest, RejectsStartAfterEnd)
{
    const auto request = makeRequest(
        market::Timestamp{std::chrono::seconds{200}},
        market::Timestamp{std::chrono::seconds{100}}
    );

    const auto result = MarketDataRequestValidator::validate(request);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Market data request start must be before end."
    );
}

TEST(MarketDataRequestValidatorTest, RejectsZeroTimeframe)
{
    const auto request = makeRequest(
        market::Timestamp{std::chrono::seconds{100}},
        market::Timestamp{std::chrono::seconds{200}},
        market::Timeframe{
            0,
            market::TimeframeUnit::Minute
        }
    );

    const auto result = MarketDataRequestValidator::validate(request);

    EXPECT_FALSE(result.valid());
    EXPECT_EQ(
        result.message(),
        "Market data request timeframe must be greater than zero."
    );
}

TEST(MarketDataRequestValidatorTest, AcceptsPositiveTimeframe)
{
    const auto request = makeRequest(
        market::Timestamp{std::chrono::seconds{100}},
        market::Timestamp{std::chrono::seconds{200}},
        market::Timeframe{
            5,
            market::TimeframeUnit::Minute
        }
    );

    const auto result = MarketDataRequestValidator::validate(request);

    EXPECT_TRUE(result.valid());
}

} // namespace quantforge::marketdata

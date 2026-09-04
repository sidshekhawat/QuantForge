#include <gtest/gtest.h>

#include "quantforge/market/validation_result.hpp"


namespace quantforge::market {


TEST(ValidationResultTest, SuccessIsValid)
{
    auto result = ValidationResult::success();

    EXPECT_TRUE(result.valid());
}


TEST(ValidationResultTest, FailureIsInvalid)
{
    auto result =
        ValidationResult::failure(
            "Invalid candle"
        );

    EXPECT_FALSE(result.valid());
}


TEST(ValidationResultTest, StoresMessage)
{
    auto result =
        ValidationResult::failure(
            "High price below open"
        );

    EXPECT_EQ(
        result.message(),
        "High price below open"
    );
}


}
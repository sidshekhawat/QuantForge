#include <gtest/gtest.h>

#include "quantforge/market/price.hpp"

namespace quantforge::market {

TEST(PriceTest, StoresValue) {
    Price price{500, 2};

    EXPECT_EQ(price.value(), 500);
}

TEST(PriceTest, StoresScale) {
    Price price{500, 2};

    EXPECT_EQ(price.scale(), 2);
}

TEST(PriceTest, RepresentsScaledDecimal) {
    Price price{500, 2};

    EXPECT_EQ(price.value(), 500);
    EXPECT_EQ(price.scale(), 2);
}

TEST(PriceTest, ComparesEqualPrices) {
    Price first{500, 2};
    Price second{500, 2};

    EXPECT_EQ(first, second);
}

TEST(PriceTest, ComparesDifferentPrices) {
    Price first{500, 2};
    Price second{500, 3};

    EXPECT_NE(first, second);
}

TEST(PriceTest, EqualValuesWithDifferentScales) {
    Price first{500, 2};
    Price second{50, 1};

    EXPECT_EQ(first, second);
}

TEST(PriceTest, EqualValuesWithTrailingZeros) {
    Price first{10, 2};
    Price second{100, 3};

    EXPECT_EQ(first, second);
}

TEST(PriceTest, DifferentValuesWithDifferentScales) {
    Price first{500, 2};
    Price second{501, 2};

    EXPECT_NE(first, second);
}

TEST(PriceTest, ZeroValuesWithDifferentScalesAreEqual) {
    Price first{0, 0};
    Price second{0, 8};

    EXPECT_EQ(first, second);
}

} // namespace quantforge::market
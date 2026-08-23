#include <gtest/gtest.h>

#include "quantforge/market/quantity.hpp"

namespace quantforge::market {

TEST(QuantityTest, StoresValue) {
    Quantity quantity{1500, 3};

    EXPECT_EQ(quantity.value(), 1500);
}

TEST(QuantityTest, StoresScale) {
    Quantity quantity{1500, 3};

    EXPECT_EQ(quantity.scale(), 3);
}

TEST(QuantityTest, ComparesEqualQuantities) {
    Quantity first{100, 0};
    Quantity second{100, 0};

    EXPECT_EQ(first, second);
}

TEST(QuantityTest, EqualValuesWithDifferentScales) {
    Quantity first{1000, 2};
    Quantity second{10, 0};

    EXPECT_EQ(first, second);
}

TEST(QuantityTest, EqualValuesWithTrailingZeros) {
    Quantity first{125, 2};
    Quantity second{1250, 3};

    EXPECT_EQ(first, second);
}

TEST(QuantityTest, DifferentQuantities) {
    Quantity first{100, 0};
    Quantity second{101, 0};

    EXPECT_NE(first, second);
}

TEST(QuantityTest, ZeroValuesWithDifferentScalesAreEqual) {
    Quantity first{0, 0};
    Quantity second{0, 8};

    EXPECT_EQ(first, second);
}

} // namespace quantforge::market
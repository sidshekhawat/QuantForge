#include <gtest/gtest.h>

#include "quantforge/market/instrument_id.hpp"

namespace quantforge::market {

TEST(InstrumentIdTest, StoresValue) {
    InstrumentId id{12345};

    EXPECT_EQ(id.value(), 12345);
}

TEST(InstrumentIdTest, ComparesEqualIds) {
    InstrumentId first{12345};
    InstrumentId second{12345};

    EXPECT_EQ(first, second);
}

TEST(InstrumentIdTest, ComparesDifferentIds) {
    InstrumentId first{12345};
    InstrumentId second{67890};

    EXPECT_NE(first, second);
}

} // namespace quantforge::market
#include <gtest/gtest.h>

#include "quantforge/market/bar.hpp"

namespace quantforge::market {

TEST(BarTest, StoresInstrumentId) {
    Bar bar{
        InstrumentId{100001},
        Timestamp{std::chrono::nanoseconds{1000}},
        Timeframe{5, TimeframeUnit::Minute},
        Price{10000, 2},
        Price{10500, 2},
        Price{9900, 2},
        Price{10300, 2},
        Quantity{50000, 0}
    };

    EXPECT_EQ(bar.instrument_id().value(), 100001);
}

TEST(BarTest, StoresTimestamp) {
    Timestamp timestamp{
        std::chrono::nanoseconds{1000}
    };

    Bar bar{
        InstrumentId{100001},
        timestamp,
        Timeframe{5, TimeframeUnit::Minute},
        Price{10000, 2},
        Price{10500, 2},
        Price{9900, 2},
        Price{10300, 2},
        Quantity{50000, 0}
    };

    EXPECT_EQ(bar.timestamp(), timestamp);
}

TEST(BarTest, StoresOHLC) {
    Bar bar{
        InstrumentId{100001},
        Timestamp{std::chrono::nanoseconds{1000}},
        Timeframe{5, TimeframeUnit::Minute},
        Price{10000, 2},
        Price{10500, 2},
        Price{9900, 2},
        Price{10300, 2},
        Quantity{50000, 0}
    };

    const Price expected_open{10000, 2};
    const Price expected_high{10500, 2};
    const Price expected_low{9900, 2};
    const Price expected_close{10300, 2};

    EXPECT_EQ(bar.open(), expected_open);
    EXPECT_EQ(bar.high(), expected_high);
    EXPECT_EQ(bar.low(), expected_low);
    EXPECT_EQ(bar.close(), expected_close);
}

TEST(BarTest, StoresVolume) {
    Bar bar{
        InstrumentId{100001},
        Timestamp{std::chrono::nanoseconds{1000}},
        Timeframe{5, TimeframeUnit::Minute},
        Price{10000, 2},
        Price{10500, 2},
        Price{9900, 2},
        Price{10300, 2},
        Quantity{50000, 0}
    };

    const Quantity expected_volume{50000,0};
    
    EXPECT_EQ(bar.volume(), expected_volume);
}

}
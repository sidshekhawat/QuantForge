#include <gtest/gtest.h>

#include "quantforge/market/instrument.hpp"

namespace quantforge::market {

TEST(InstrumentTest, StoresInstrumentId) {
    Instrument instrument{
        InstrumentId{100001},
        "NIFTY",
        AssetClass::Index,
        "NSE",
        "INR",
        Price{5, 2},
        Quantity{50, 0}
    };

    EXPECT_EQ(instrument.id().value(), 100001);
}

TEST(InstrumentTest, StoresSymbol) {
    Instrument instrument{
        InstrumentId{100001},
        "NIFTY",
        AssetClass::Index,
        "NSE",
        "INR",
        Price{5, 2},
        Quantity{50, 0}
    };

    EXPECT_EQ(instrument.symbol(), "NIFTY");
}

TEST(InstrumentTest, StoresAssetClass) {
    Instrument instrument{
        InstrumentId{100001},
        "NIFTY",
        AssetClass::Index,
        "NSE",
        "INR",
        Price{5, 2},
        Quantity{50, 0}
    };

    EXPECT_EQ(instrument.asset_class(), AssetClass::Index);
}

TEST(InstrumentTest, StoresExchangeAndCurrency) {
    Instrument instrument{
        InstrumentId{100001},
        "NIFTY",
        AssetClass::Index,
        "NSE",
        "INR",
        Price{5, 2},
        Quantity{50, 0}
    };

    EXPECT_EQ(instrument.exchange(), "NSE");
    EXPECT_EQ(instrument.currency(), "INR");
}

TEST(InstrumentTest, StoresTickSizeAndLotSize) {
    Instrument instrument{
        InstrumentId{100001},
        "NIFTY",
        AssetClass::Index,
        "NSE",
        "INR",
        Price{5, 2},
        Quantity{50, 0}
    };

    const Price expected_tick_size{5, 2};
    const Quantity expected_lot_size{50, 0};

    EXPECT_EQ(instrument.tick_size(), expected_tick_size);
    EXPECT_EQ(instrument.lot_size(), expected_lot_size);
    }

} // namespace quantforge::market
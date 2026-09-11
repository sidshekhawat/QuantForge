#include "quantforge/portfolio/position.hpp"

#include <gtest/gtest.h>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::portfolio::Position;
using quantforge::portfolio::PositionSide;

TEST(PositionTest, StoresInstrumentId) {
    const Position position(
        InstrumentId{42},
        PositionSide::Long,
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(position.instrument_id(), InstrumentId{42});
}

TEST(PositionTest, StoresSide) {
    const Position position(
        InstrumentId{42},
        PositionSide::Short,
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(position.side(), PositionSide::Short);
}

TEST(PositionTest, StoresQuantity) {
    const Position position(
        InstrumentId{42},
        PositionSide::Long,
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(position.quantity(), (Quantity{100, 0}));
}

TEST(PositionTest, StoresAverageEntryPrice) {
    const Position position(
        InstrumentId{42},
        PositionSide::Long,
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(
        position.average_entry_price(),
        (Price{15000, 2})
    );
}

TEST(PositionTest, SupportsFlatPosition) {
    const Position position(
        InstrumentId{42},
        PositionSide::Flat,
        Quantity{0, 0},
        Price{0, 2}
    );

    EXPECT_EQ(position.side(), PositionSide::Flat);
    EXPECT_EQ(position.quantity(), (Quantity{0, 0}));
}

} // namespace

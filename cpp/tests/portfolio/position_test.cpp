#include "quantforge/portfolio/position.hpp"

#include <gtest/gtest.h>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::portfolio::Position;

TEST(PositionTest, StoresInstrumentId) {
    const Position position(
        InstrumentId{42},
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(position.instrument_id(), InstrumentId{42});
}

TEST(PositionTest, StoresQuantity) {
    const Position position(
        InstrumentId{42},
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(position.quantity(), (Quantity{100, 0}));
}

TEST(PositionTest, StoresAverageEntryPrice) {
    const Position position(
        InstrumentId{42},
        Quantity{100, 0},
        Price{15000, 2}
    );

    EXPECT_EQ(
        position.average_entry_price(),
        (Price{15000, 2})
    );
}

} // namespace

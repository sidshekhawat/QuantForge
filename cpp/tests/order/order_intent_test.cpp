#include "quantforge/order/order_intent.hpp"

#include <gtest/gtest.h>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Quantity;
using quantforge::order::OrderIntent;
using quantforge::order::OrderSide;

TEST(OrderIntentTest, StoresInstrumentId) {
    const OrderIntent intent(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{100, 0}
    );

    EXPECT_EQ(intent.instrument_id(), InstrumentId{42});
}

TEST(OrderIntentTest, StoresSide) {
    const OrderIntent intent(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{100, 0}
    );

    EXPECT_EQ(intent.side(), OrderSide::Sell);
}

TEST(OrderIntentTest, StoresQuantity) {
    const OrderIntent intent(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{250, 0}
    );

    EXPECT_EQ(intent.quantity(), (Quantity{250, 0}));
}

} // namespace

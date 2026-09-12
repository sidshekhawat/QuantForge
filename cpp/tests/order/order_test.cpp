#include "quantforge/order/order.hpp"

#include <gtest/gtest.h>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::order::Order;
using quantforge::order::OrderId;
using quantforge::order::OrderSide;
using quantforge::order::OrderType;

TEST(OrderTest, StoresOrderId) {
    const Order order(
        OrderId{1001},
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{100, 0},
        OrderType::Market
    );

    EXPECT_EQ(order.id(), OrderId{1001});
}

TEST(OrderTest, StoresInstrumentId) {
    const Order order(
        OrderId{1001},
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{100, 0},
        OrderType::Market
    );

    EXPECT_EQ(order.instrument_id(), InstrumentId{42});
}

TEST(OrderTest, StoresSide) {
    const Order order(
        OrderId{1001},
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{100, 0},
        OrderType::Market
    );

    EXPECT_EQ(order.side(), OrderSide::Sell);
}

TEST(OrderTest, StoresQuantity) {
    const Order order(
        OrderId{1001},
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{250, 0},
        OrderType::Market
    );

    EXPECT_EQ(order.quantity(), (Quantity{250, 0}));
}

TEST(OrderTest, StoresMarketOrderType) {
    const Order order(
        OrderId{1001},
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{100, 0},
        OrderType::Market
    );

    EXPECT_EQ(order.type(), OrderType::Market);
    EXPECT_FALSE(order.limit_price().has_value());
}

TEST(OrderTest, StoresLimitOrderTypeAndPrice) {
    const Order order(
        OrderId{1001},
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{100, 0},
        OrderType::Limit,
        Price{15000, 2}
    );

    EXPECT_EQ(order.type(), OrderType::Limit);

    ASSERT_TRUE(order.limit_price().has_value());
    EXPECT_EQ(
        *order.limit_price(),
        (Price{15000, 2})
    );
}

TEST(OrderTest, RejectsZeroOrderId) {
    EXPECT_THROW(
        Order(
            OrderId{0},
            InstrumentId{42},
            OrderSide::Buy,
            Quantity{100, 0},
            OrderType::Market
        ),
        std::invalid_argument
    );
}

TEST(OrderTest, RejectsLimitOrderWithoutPrice) {
    EXPECT_THROW(
        Order(
            OrderId{1001},
            InstrumentId{42},
            OrderSide::Buy,
            Quantity{100, 0},
            OrderType::Limit
        ),
        std::invalid_argument
    );
}

TEST(OrderTest, RejectsMarketOrderWithLimitPrice) {
    EXPECT_THROW(
        Order(
            OrderId{1001},
            InstrumentId{42},
            OrderSide::Buy,
            Quantity{100, 0},
            OrderType::Market,
            Price{15000, 2}
        ),
        std::invalid_argument
    );
}

} // namespace

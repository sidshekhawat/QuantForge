#include "quantforge/execution/fill.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

namespace {

using quantforge::execution::Fill;
using quantforge::execution::FillId;
using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::market::Timestamp;
using quantforge::order::OrderId;
using quantforge::order::OrderSide;

const Timestamp kTimestamp{
    std::chrono::nanoseconds{1'000'000'000}
};

Fill make_fill() {
    return Fill{
        FillId{100},
        OrderId{200},
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{10, 0},
        Price{250000, 2},
        kTimestamp
    };
}

TEST(FillTest, StoresFillId) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.id(), FillId{100});
}

TEST(FillTest, StoresOrderId) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.order_id(), OrderId{200});
}

TEST(FillTest, StoresInstrumentId) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.instrument_id(), InstrumentId{42});
}

TEST(FillTest, StoresSide) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.side(), OrderSide::Buy);
}

TEST(FillTest, StoresQuantity) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.quantity(), (Quantity{10, 0}));
}

TEST(FillTest, StoresPrice) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.price(), (Price{250000, 2}));
}

TEST(FillTest, StoresTimestamp) {
    const auto fill = make_fill();

    EXPECT_EQ(fill.timestamp(), kTimestamp);
}

TEST(FillTest, RejectsZeroFillId) {
    EXPECT_THROW(
        (Fill{
            FillId{0},
            OrderId{200},
            InstrumentId{42},
            OrderSide::Buy,
            Quantity{10, 0},
            Price{250000, 2},
            kTimestamp
        }),
        std::invalid_argument
    );
}

TEST(FillTest, RejectsZeroOrderId) {
    EXPECT_THROW(
        (Fill{
            FillId{100},
            OrderId{0},
            InstrumentId{42},
            OrderSide::Buy,
            Quantity{10, 0},
            Price{250000, 2},
            kTimestamp
        }),
        std::invalid_argument
    );
}

} // namespace

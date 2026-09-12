#include "quantforge/order/order_lifecycle.hpp"

#include <gtest/gtest.h>

namespace {

using quantforge::order::OrderLifecycle;
using quantforge::order::OrderStatus;

TEST(OrderLifecycleTest, StartsCreated) {
    const OrderLifecycle lifecycle;

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Created
    );

    EXPECT_FALSE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsSuccessfulLifecycle) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    EXPECT_EQ(lifecycle.status(), OrderStatus::Submitted);

    lifecycle.accept();
    EXPECT_EQ(lifecycle.status(), OrderStatus::Accepted);

    lifecycle.partially_fill();
    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::PartiallyFilled
    );

    lifecycle.fill();
    EXPECT_EQ(lifecycle.status(), OrderStatus::Filled);

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsDirectFullFillAfterAcceptance) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.accept();
    lifecycle.fill();

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Filled
    );

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsRejectionFromCreated) {
    OrderLifecycle lifecycle;

    lifecycle.reject();

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Rejected
    );

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsRejectionFromSubmitted) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.reject();

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Rejected
    );

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsCancellationFromSubmitted) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.cancel();

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Cancelled
    );

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsCancellationFromAccepted) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.accept();
    lifecycle.cancel();

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Cancelled
    );

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, SupportsCancellationAfterPartialFill) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.accept();
    lifecycle.partially_fill();
    lifecycle.cancel();

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Cancelled
    );

    EXPECT_TRUE(lifecycle.is_terminal());
}

TEST(OrderLifecycleTest, RejectsInvalidTransitionFromCreatedToAccepted) {
    OrderLifecycle lifecycle;

    EXPECT_THROW(
        lifecycle.accept(),
        std::logic_error
    );

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Created
    );
}

TEST(OrderLifecycleTest, RejectsInvalidTransitionFromSubmittedToFilled) {
    OrderLifecycle lifecycle;

    lifecycle.submit();

    EXPECT_THROW(
        lifecycle.fill(),
        std::logic_error
    );

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Submitted
    );
}

TEST(OrderLifecycleTest, RejectsInvalidTransitionFromAcceptedToRejected) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.accept();

    EXPECT_THROW(
        lifecycle.reject(),
        std::logic_error
    );

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Accepted
    );
}

TEST(OrderLifecycleTest, RejectsTransitionAfterFilled) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.accept();
    lifecycle.fill();

    EXPECT_THROW(
        lifecycle.cancel(),
        std::logic_error
    );

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Filled
    );
}

TEST(OrderLifecycleTest, RejectsTransitionAfterRejected) {
    OrderLifecycle lifecycle;

    lifecycle.reject();

    EXPECT_THROW(
        lifecycle.submit(),
        std::logic_error
    );

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Rejected
    );
}

TEST(OrderLifecycleTest, RejectsTransitionAfterCancelled) {
    OrderLifecycle lifecycle;

    lifecycle.submit();
    lifecycle.cancel();

    EXPECT_THROW(
        lifecycle.accept(),
        std::logic_error
    );

    EXPECT_EQ(
        lifecycle.status(),
        OrderStatus::Cancelled
    );
}

} // namespace

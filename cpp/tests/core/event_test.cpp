#include <chrono>

#include <gtest/gtest.h>

#include "quantforge/core/event.hpp"

namespace {

using quantforge::core::Event;
using quantforge::core::EventId;
using quantforge::core::EventType;
using quantforge::core::Timestamp;

TEST(EventTest, StoresEventId) {
    constexpr EventId expected_id = 42;
    const Timestamp timestamp{};

    const Event event(expected_id, EventType::MarketData, timestamp);

    EXPECT_EQ(event.id(), expected_id);
}

TEST(EventTest, StoresEventType) {
    constexpr EventId event_id = 42;
    const Timestamp timestamp{};

    const Event event(event_id, EventType::Signal, timestamp);

    EXPECT_EQ(event.type(), EventType::Signal);
}

TEST(EventTest, StoresTimestamp) {
    constexpr EventId event_id = 42;
    const Timestamp expected_timestamp{};

    const Event event(event_id, EventType::Order, expected_timestamp);

    EXPECT_EQ(event.timestamp(), expected_timestamp);
}

}  // namespace
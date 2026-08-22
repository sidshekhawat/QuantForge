#pragma once

#include <cstdint>
#include <chrono>

#include "quantforge/core/event_type.hpp"

namespace quantforge::core {

using EventId = std::uint64_t;
using Timestamp = std::chrono::system_clock::time_point;

class Event {
public:
    Event(EventId id, EventType type, Timestamp timestamp);

    EventId id() const noexcept;
    EventType type() const noexcept;
    Timestamp timestamp() const noexcept;

private:
    EventId id_;
    EventType type_;
    Timestamp timestamp_;
};

}  // namespace quantforge::core
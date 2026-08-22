#include "quantforge/core/event.hpp"

namespace quantforge::core {

Event::Event(EventId id, EventType type, Timestamp timestamp)
    : id_(id),
      type_(type),
      timestamp_(timestamp) {}

EventId Event::id() const noexcept {
    return id_;
}

EventType Event::type() const noexcept {
    return type_;
}

Timestamp Event::timestamp() const noexcept {
    return timestamp_;
}

}  // namespace quantforge::core
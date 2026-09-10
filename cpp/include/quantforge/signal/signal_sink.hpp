#pragma once

#include "quantforge/signal/signal.hpp"

namespace quantforge::signal {

class SignalSink {
public:
    virtual ~SignalSink() = default;

    virtual void emit(const Signal& signal) = 0;
};

} // namespace quantforge::signal

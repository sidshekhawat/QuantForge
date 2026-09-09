#pragma once

#include "quantforge/market/bar.hpp"

namespace quantforge::strategy {

class Strategy {
public:
    virtual ~Strategy() = default;

    virtual void on_start() {}

    virtual void on_bar(const market::Bar& bar) = 0;

    virtual void on_finish() {}
};

} // namespace quantforge::strategy

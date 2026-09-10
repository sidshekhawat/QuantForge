#pragma once

#include "quantforge/market/instrument_id.hpp"
#include "quantforge/portfolio/position.hpp"

#include <optional>

namespace quantforge::portfolio {

class PortfolioState {
public:
    virtual ~PortfolioState() = default;

    [[nodiscard]] virtual std::optional<Position> find_position(
        market::InstrumentId instrument_id
    ) const = 0;
};

} // namespace quantforge::portfolio

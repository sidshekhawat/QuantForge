#pragma once

#include "quantforge/order/order_intent.hpp"
#include "quantforge/portfolio/portfolio_state.hpp"
#include "quantforge/risk/risk_decision.hpp"

namespace quantforge::risk {

class MaxPositionRule {
public:
    explicit MaxPositionRule(
        market::Quantity maximum_position_quantity
    ) noexcept;

    [[nodiscard]] RiskDecision evaluate(
        const order::OrderIntent& order_intent,
        const portfolio::PortfolioState& portfolio_state
    ) const;

private:
    market::Quantity maximum_position_quantity_;
};

} // namespace quantforge::risk

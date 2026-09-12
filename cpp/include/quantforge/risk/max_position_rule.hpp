#pragma once

#include "quantforge/risk/risk_rule.hpp"

namespace quantforge::risk {

class MaxPositionRule final : public RiskRule {
public:
    explicit MaxPositionRule(
        market::Quantity maximum_position_quantity
    ) noexcept;

    [[nodiscard]] RiskDecision evaluate(
        const order::OrderIntent& order_intent,
        const portfolio::PortfolioState& portfolio_state
    ) const override;

private:
    market::Quantity maximum_position_quantity_;
};

} // namespace quantforge::risk

#pragma once

#include "quantforge/order/order_intent.hpp"
#include "quantforge/portfolio/portfolio_state.hpp"
#include "quantforge/risk/risk_decision.hpp"

namespace quantforge::risk {

class RiskRule {
public:
    virtual ~RiskRule() = default;

    [[nodiscard]] virtual RiskDecision evaluate(
        const order::OrderIntent& order_intent,
        const portfolio::PortfolioState& portfolio_state
    ) const = 0;
};

} // namespace quantforge::risk

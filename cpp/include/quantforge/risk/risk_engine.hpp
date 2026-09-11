#pragma once

#include "quantforge/portfolio/portfolio_state.hpp"
#include "quantforge/risk/risk_decision.hpp"
#include "quantforge/signal/signal.hpp"

namespace quantforge::risk {

class RiskEngine {
public:
    virtual ~RiskEngine() = default;

    [[nodiscard]] virtual RiskDecision evaluate(
        const signal::Signal& signal,
        const portfolio::PortfolioState& portfolio_state
    ) = 0;
};

} // namespace quantforge::risk

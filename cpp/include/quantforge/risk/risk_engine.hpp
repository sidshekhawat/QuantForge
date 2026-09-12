#pragma once

#include "quantforge/portfolio/portfolio_state.hpp"
#include "quantforge/risk/risk_decision.hpp"
#include "quantforge/risk/risk_rule.hpp"

#include <memory>
#include <vector>

namespace quantforge::risk {

class RiskEngine {
public:
    RiskEngine() = default;

    explicit RiskEngine(
        std::vector<std::unique_ptr<RiskRule>> rules
    );

    void add_rule(std::unique_ptr<RiskRule> rule);

    [[nodiscard]] RiskDecision evaluate(
        const order::OrderIntent& order_intent,
        const portfolio::PortfolioState& portfolio_state
    ) const;

private:
    std::vector<std::unique_ptr<RiskRule>> rules_;
};

} // namespace quantforge::risk

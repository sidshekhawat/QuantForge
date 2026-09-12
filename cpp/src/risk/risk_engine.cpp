#include "quantforge/risk/risk_engine.hpp"

#include <stdexcept>
#include <utility>

namespace quantforge::risk {

RiskEngine::RiskEngine(
    std::vector<std::unique_ptr<RiskRule>> rules)
    : rules_(std::move(rules))
{
    for (const auto& rule : rules_) {
        if (!rule) {
            throw std::invalid_argument(
                "Risk engine cannot contain a null rule."
            );
        }
    }
}

void RiskEngine::add_rule(std::unique_ptr<RiskRule> rule) {
    if (!rule) {
        throw std::invalid_argument(
            "Risk engine cannot add a null rule."
        );
    }

    rules_.push_back(std::move(rule));
}

RiskDecision RiskEngine::evaluate(
    const order::OrderIntent& order_intent,
    const portfolio::PortfolioState& portfolio_state) const
{
    for (const auto& rule : rules_) {
        const auto decision = rule->evaluate(
            order_intent,
            portfolio_state
        );

        if (decision.is_rejected()) {
            return decision;
        }
    }

    return RiskDecision::approved();
}

} // namespace quantforge::risk

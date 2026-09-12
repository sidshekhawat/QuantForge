#include "quantforge/risk/max_position_rule.hpp"
#include "quantforge/risk/risk_rule.hpp"

#include <gtest/gtest.h>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Quantity;
using quantforge::order::OrderIntent;
using quantforge::order::OrderSide;
using quantforge::portfolio::PortfolioState;
using quantforge::risk::MaxPositionRule;
using quantforge::risk::RiskRule;

class EmptyPortfolioState final : public PortfolioState {
public:
    [[nodiscard]] std::optional<
        quantforge::portfolio::Position
    > find_position(InstrumentId) const override {
        return std::nullopt;
    }
};

TEST(RiskRuleTest, MaxPositionRuleImplementsRiskRule) {
    MaxPositionRule concrete_rule(Quantity{100, 0});

    RiskRule& rule = concrete_rule;

    EmptyPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{50, 0}
    );

    const auto decision = rule.evaluate(
        order,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_approved());
}

TEST(RiskRuleTest, RiskRuleHasVirtualDestructor) {
    EXPECT_TRUE(std::has_virtual_destructor_v<RiskRule>);
}

} // namespace

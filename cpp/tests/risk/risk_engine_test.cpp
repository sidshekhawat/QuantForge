#include "quantforge/risk/max_position_rule.hpp"
#include "quantforge/risk/risk_engine.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::order::OrderIntent;
using quantforge::order::OrderSide;
using quantforge::portfolio::PortfolioState;
using quantforge::portfolio::Position;
using quantforge::portfolio::PositionSide;
using quantforge::risk::MaxPositionRule;
using quantforge::risk::RiskDecision;
using quantforge::risk::RiskEngine;
using quantforge::risk::RiskRule;

class TestPortfolioState final : public PortfolioState {
public:
    void add_position(Position position) {
        positions_.emplace(
            position.instrument_id(),
            std::move(position)
        );
    }

    [[nodiscard]] std::optional<Position> find_position(
        InstrumentId instrument_id
    ) const override {
        const auto it = positions_.find(instrument_id);

        if (it == positions_.end()) {
            return std::nullopt;
        }

        return it->second;
    }

private:
    struct InstrumentIdHash {
        std::size_t operator()(InstrumentId instrument_id) const noexcept {
            return std::hash<InstrumentId::ValueType>{}(
                instrument_id.value()
            );
        }
    };

    std::unordered_map<
        InstrumentId,
        Position,
        InstrumentIdHash
    > positions_;
};

class FixedDecisionRule final : public RiskRule {
public:
    explicit FixedDecisionRule(
        RiskDecision decision)
        : decision_(std::move(decision)) {}

    [[nodiscard]] RiskDecision evaluate(
        const OrderIntent&,
        const PortfolioState&
    ) const override {
        ++evaluation_count_;
        return decision_;
    }

    [[nodiscard]] int evaluation_count() const noexcept {
        return evaluation_count_;
    }

private:
    RiskDecision decision_;
    mutable int evaluation_count_ = 0;
};

class RecordingRule final : public RiskRule {
public:
    RecordingRule(
        std::string name,
        std::vector<std::string>& evaluations)
        : name_(std::move(name)),
          evaluations_(evaluations) {}

    [[nodiscard]] RiskDecision evaluate(
        const OrderIntent&,
        const PortfolioState&
    ) const override {
        evaluations_.push_back(name_);
        return RiskDecision::approved();
    }

private:
    std::string name_;
    std::vector<std::string>& evaluations_;
};

TEST(RiskEngineTest, ApprovesWhenNoRulesReject) {
    RiskEngine engine;

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{50, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_approved());
}

TEST(RiskEngineTest, ApprovesWhenAllRulesApprove) {
    RiskEngine engine;

    engine.add_rule(
        std::make_unique<MaxPositionRule>(
            Quantity{100, 0}
        )
    );

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{50, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_approved());
}

TEST(RiskEngineTest, RejectsWhenRuleRejects) {
    RiskEngine engine;

    engine.add_rule(
        std::make_unique<MaxPositionRule>(
            Quantity{100, 0}
        )
    );

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{101, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_rejected());
}

TEST(RiskEngineTest, PreservesRejectionReason) {
    RiskEngine engine;

    engine.add_rule(
        std::make_unique<FixedDecisionRule>(
            RiskDecision::rejected("test rejection")
        )
    );

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{1, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    ASSERT_TRUE(decision.is_rejected());
    EXPECT_EQ(decision.reason(), "test rejection");
}

TEST(RiskEngineTest, StopsAfterFirstRejection) {
    auto rejecting_rule =
        std::make_unique<FixedDecisionRule>(
            RiskDecision::rejected("first rejection")
        );

    auto approving_rule =
        std::make_unique<FixedDecisionRule>(
            RiskDecision::approved()
        );

    auto* approving_rule_ptr = approving_rule.get();

    RiskEngine engine;
    engine.add_rule(std::move(rejecting_rule));
    engine.add_rule(std::move(approving_rule));

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{1, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_rejected());
    EXPECT_EQ(decision.reason(), "first rejection");
    EXPECT_EQ(approving_rule_ptr->evaluation_count(), 0);
}

TEST(RiskEngineTest, EvaluatesRulesInInsertionOrder) {
    std::vector<std::string> evaluations;

    RiskEngine engine;

    engine.add_rule(
        std::make_unique<RecordingRule>(
            "first",
            evaluations
        )
    );

    engine.add_rule(
        std::make_unique<RecordingRule>(
            "second",
            evaluations
        )
    );

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{1, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    ASSERT_TRUE(decision.is_approved());

    ASSERT_EQ(evaluations.size(), 2U);
    EXPECT_EQ(evaluations[0], "first");
    EXPECT_EQ(evaluations[1], "second");
}

TEST(RiskEngineTest, RejectingRuleStopsLaterRules) {
    std::vector<std::string> evaluations;

    auto rejecting_rule =
        std::make_unique<FixedDecisionRule>(
            RiskDecision::rejected("blocked")
        );

    auto later_rule =
        std::make_unique<RecordingRule>(
            "later",
            evaluations
        );

    RiskEngine engine;
    engine.add_rule(std::move(rejecting_rule));
    engine.add_rule(std::move(later_rule));

    TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{1, 0}
    );

    const auto decision = engine.evaluate(
        order,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_rejected());
    EXPECT_EQ(decision.reason(), "blocked");
    EXPECT_TRUE(evaluations.empty());
}

TEST(RiskEngineTest, RejectsNullRule) {
    RiskEngine engine;

    EXPECT_THROW(
        engine.add_rule(nullptr),
        std::invalid_argument
    );
}

TEST(RiskEngineTest, ConstructorRejectsNullRule) {
    std::vector<std::unique_ptr<RiskRule>> rules;

    rules.push_back(nullptr);

    EXPECT_THROW(
        RiskEngine(std::move(rules)),
        std::invalid_argument
    );
}

} // namespace

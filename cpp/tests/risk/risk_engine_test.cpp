#include "quantforge/risk/risk_engine.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <utility>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::market::Timestamp;
using quantforge::portfolio::PortfolioState;
using quantforge::portfolio::Position;
using quantforge::risk::RiskDecision;
using quantforge::risk::RiskEngine;
using quantforge::signal::Signal;
using quantforge::signal::SignalDirection;

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

class TestRiskEngine final : public RiskEngine {
public:
    [[nodiscard]] RiskDecision evaluate(
        const Signal& signal,
        const PortfolioState& portfolio_state
    ) override {
        received_signal_ = &signal;
        received_portfolio_state_ = &portfolio_state;

        return RiskDecision::approved();
    }

    [[nodiscard]] const Signal* received_signal() const noexcept {
        return received_signal_;
    }

    [[nodiscard]] const PortfolioState* received_portfolio_state() const noexcept {
        return received_portfolio_state_;
    }

private:
    const Signal* received_signal_ = nullptr;
    const PortfolioState* received_portfolio_state_ = nullptr;
};

TEST(RiskEngineTest, CanApproveSignalWithPortfolioState) {
    TestRiskEngine risk_engine;
    TestPortfolioState portfolio_state;

    const Signal signal(
        InstrumentId{42},
        Timestamp{},
        SignalDirection::Buy,
        100
    );

    const auto decision = risk_engine.evaluate(
        signal,
        portfolio_state
    );

    EXPECT_TRUE(decision.is_approved());
}

TEST(RiskEngineTest, ReceivesSignalAndPortfolioState) {
    TestRiskEngine risk_engine;
    TestPortfolioState portfolio_state;

    const Signal signal(
        InstrumentId{42},
        Timestamp{},
        SignalDirection::Buy,
        100
    );

    const auto decision = risk_engine.evaluate(
        signal,
        portfolio_state
    );
    ASSERT_TRUE(decision.is_approved());

    EXPECT_EQ(risk_engine.received_signal(), &signal);
    EXPECT_EQ(
        risk_engine.received_portfolio_state(),
        &portfolio_state
    );
}

TEST(RiskEngineTest, CanInspectExistingPositionThroughPortfolioState) {
    TestRiskEngine risk_engine;
    TestPortfolioState portfolio_state;

    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            quantforge::portfolio::PositionSide::Long,
            Quantity{100, 0},
            Price{15000, 2}
        )
    );

    const Signal signal(
        InstrumentId{42},
        Timestamp{},
        SignalDirection::Buy,
        100
    );

    const auto decision = risk_engine.evaluate(
        signal,
        portfolio_state
    );
    ASSERT_TRUE(decision.is_approved());

    const auto position =
        portfolio_state.find_position(InstrumentId{42});

    ASSERT_TRUE(position.has_value());
    EXPECT_EQ(position->quantity(), (Quantity{100, 0}));
}

} // namespace

#include "quantforge/risk/risk_engine.hpp"

#include <chrono>

#include <gtest/gtest.h>

namespace {

quantforge::signal::Signal MakeSignal() {
    return quantforge::signal::Signal(
        quantforge::market::InstrumentId{1},
        quantforge::market::Timestamp{
            std::chrono::seconds{100}
        },
        quantforge::signal::SignalDirection::Buy,
        80
    );
}

class AllowAllRiskEngine final
    : public quantforge::risk::RiskEngine {
public:
    quantforge::risk::RiskDecision evaluate(
        const quantforge::signal::Signal&
    ) override {
        return quantforge::risk::RiskDecision::approved();
    }
};

class RejectAllRiskEngine final
    : public quantforge::risk::RiskEngine {
public:
    quantforge::risk::RiskDecision evaluate(
        const quantforge::signal::Signal&
    ) override {
        return quantforge::risk::RiskDecision::rejected(
            "Signal rejected by risk policy."
        );
    }
};

} // namespace

TEST(RiskEngineTest, CanApproveSignal) {
    AllowAllRiskEngine engine;

    const auto decision = engine.evaluate(MakeSignal());

    EXPECT_TRUE(decision.is_approved());
    EXPECT_FALSE(decision.is_rejected());
}

TEST(RiskEngineTest, CanRejectSignal) {
    RejectAllRiskEngine engine;

    const auto decision = engine.evaluate(MakeSignal());

    EXPECT_TRUE(decision.is_rejected());

    EXPECT_EQ(
        decision.reason(),
        "Signal rejected by risk policy."
    );
}

TEST(RiskEngineTest, ReceivesSignalWithoutModifyingIt) {
    class InspectingRiskEngine final
        : public quantforge::risk::RiskEngine {
    public:
        quantforge::risk::RiskDecision evaluate(
            const quantforge::signal::Signal& signal
        ) override {
            received_instrument = signal.instrument_id();
            received_direction = signal.direction();
            received_strength = signal.strength();

            return quantforge::risk::RiskDecision::approved();
        }

        quantforge::market::InstrumentId received_instrument{0};
        quantforge::signal::SignalDirection received_direction{
            quantforge::signal::SignalDirection::Hold
        };
        std::uint8_t received_strength{0};
    };

    InspectingRiskEngine engine;
    const auto signal = MakeSignal();

    engine.evaluate(signal);

    EXPECT_EQ(
        engine.received_instrument,
        quantforge::market::InstrumentId{1}
    );

    EXPECT_EQ(
        engine.received_direction,
        quantforge::signal::SignalDirection::Buy
    );

    EXPECT_EQ(engine.received_strength, 80);
}


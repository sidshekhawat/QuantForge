#include "quantforge/risk/max_position_rule.hpp"

#include <cstdint>
#include <limits>
#include <sstream>

namespace quantforge::risk {

namespace {

using SignedValue = std::int64_t;

SignedValue signed_quantity(
    const portfolio::Position& position)
{
    const auto value = position.quantity().value();

    if (value > static_cast<std::uint64_t>(
            std::numeric_limits<SignedValue>::max())) {
        throw std::overflow_error(
            "Position quantity exceeds signed risk calculation range."
        );
    }

    const auto signed_value = static_cast<SignedValue>(value);

    switch (position.side()) {
        case portfolio::PositionSide::Long:
            return signed_value;

        case portfolio::PositionSide::Short:
            return -signed_value;

        case portfolio::PositionSide::Flat:
            return 0;
    }

    throw std::logic_error("Unknown position side.");
}

SignedValue signed_order_quantity(
    const order::OrderIntent& order_intent)
{
    const auto value = order_intent.quantity().value();

    if (value > static_cast<std::uint64_t>(
            std::numeric_limits<SignedValue>::max())) {
        throw std::overflow_error(
            "Order quantity exceeds signed risk calculation range."
        );
    }

    const auto signed_value = static_cast<SignedValue>(value);

    switch (order_intent.side()) {
        case order::OrderSide::Buy:
            return signed_value;

        case order::OrderSide::Sell:
            return -signed_value;
    }

    throw std::logic_error("Unknown order side.");
}

} // namespace

MaxPositionRule::MaxPositionRule(
    market::Quantity maximum_position_quantity) noexcept
    : maximum_position_quantity_(maximum_position_quantity) {}

RiskDecision MaxPositionRule::evaluate(
    const order::OrderIntent& order_intent,
    const portfolio::PortfolioState& portfolio_state) const
{
    const auto current_position =
        portfolio_state.find_position(order_intent.instrument_id());

    const SignedValue current_quantity =
        current_position.has_value()
            ? signed_quantity(*current_position)
            : 0;

    const SignedValue order_quantity =
        signed_order_quantity(order_intent);

    const SignedValue projected_quantity =
        current_quantity + order_quantity;

    const auto absolute_projected_quantity =
        projected_quantity < 0
            ? -projected_quantity
            : projected_quantity;

    if (absolute_projected_quantity >
        static_cast<SignedValue>(
            maximum_position_quantity_.value())) {
        std::ostringstream reason;

        reason
            << "Projected position exceeds maximum allowed quantity.";

        return RiskDecision::rejected(reason.str());
    }

    return RiskDecision::approved();
}

} // namespace quantforge::risk

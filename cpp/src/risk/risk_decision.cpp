#include "quantforge/risk/risk_decision.hpp"

#include <utility>

namespace quantforge::risk {

RiskDecision::RiskDecision(
    RiskDecisionStatus status,
    std::string reason)
    : status_(status),
      reason_(std::move(reason)) {}

RiskDecision RiskDecision::approved() {
    return RiskDecision(
        RiskDecisionStatus::Approved,
        {}
    );
}

RiskDecision RiskDecision::rejected(std::string reason) {
    return RiskDecision(
        RiskDecisionStatus::Rejected,
        std::move(reason)
    );
}

RiskDecisionStatus RiskDecision::status() const noexcept {
    return status_;
}

bool RiskDecision::is_approved() const noexcept {
    return status_ == RiskDecisionStatus::Approved;
}

bool RiskDecision::is_rejected() const noexcept {
    return status_ == RiskDecisionStatus::Rejected;
}

const std::string& RiskDecision::reason() const noexcept {
    return reason_;
}

} // namespace quantforge::risk

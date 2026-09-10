#pragma once

#include <string>

namespace quantforge::risk {

enum class RiskDecisionStatus {
    Approved,
    Rejected
};

class RiskDecision {
public:
    static RiskDecision approved();

    static RiskDecision rejected(std::string reason);

    [[nodiscard]] RiskDecisionStatus status() const noexcept;

    [[nodiscard]] bool is_approved() const noexcept;

    [[nodiscard]] bool is_rejected() const noexcept;

    [[nodiscard]] const std::string& reason() const noexcept;

private:
    RiskDecision(
        RiskDecisionStatus status,
        std::string reason
    );

    RiskDecisionStatus status_;
    std::string reason_;
};

} // namespace quantforge::risk

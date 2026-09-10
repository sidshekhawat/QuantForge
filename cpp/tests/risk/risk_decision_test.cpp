#include "quantforge/risk/risk_decision.hpp"

#include <gtest/gtest.h>

TEST(RiskDecisionTest, ApprovedDecisionIsApproved) {
    const auto decision =
        quantforge::risk::RiskDecision::approved();

    EXPECT_EQ(
        decision.status(),
        quantforge::risk::RiskDecisionStatus::Approved
    );

    EXPECT_TRUE(decision.is_approved());
    EXPECT_FALSE(decision.is_rejected());
    EXPECT_TRUE(decision.reason().empty());
}

TEST(RiskDecisionTest, RejectedDecisionIsRejected) {
    const auto decision =
        quantforge::risk::RiskDecision::rejected(
            "Maximum position limit exceeded."
        );

    EXPECT_EQ(
        decision.status(),
        quantforge::risk::RiskDecisionStatus::Rejected
    );

    EXPECT_FALSE(decision.is_approved());
    EXPECT_TRUE(decision.is_rejected());

    EXPECT_EQ(
        decision.reason(),
        "Maximum position limit exceeded."
    );
}

TEST(RiskDecisionTest, PreservesEmptyRejectionReason) {
    const auto decision =
        quantforge::risk::RiskDecision::rejected({});

    EXPECT_TRUE(decision.is_rejected());
    EXPECT_TRUE(decision.reason().empty());
}

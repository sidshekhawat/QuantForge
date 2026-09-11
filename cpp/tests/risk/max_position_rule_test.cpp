#include "quantforge/risk/max_position_rule.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <unordered_map>
#include <utility>

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

TEST(MaxPositionRuleTest, ApprovesOpeningPositionWithinLimit) {
    const MaxPositionRule rule(Quantity{100, 0});
    const TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{75, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_approved());
}

TEST(MaxPositionRuleTest, RejectsOpeningPositionAboveLimit) {
    const MaxPositionRule rule(Quantity{100, 0});
    const TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{101, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_rejected());
}

TEST(MaxPositionRuleTest, ApprovesIncreasingLongWithinLimit) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Long,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{60, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_approved());
}

TEST(MaxPositionRuleTest, RejectsIncreasingLongAboveLimit) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Long,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{61, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_rejected());
}

TEST(MaxPositionRuleTest, ApprovesReducingLongPosition) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Long,
            Quantity{80, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{30, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_approved());
}

TEST(MaxPositionRuleTest, ApprovesIncreasingShortWithinLimit) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Short,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{60, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_approved());
}

TEST(MaxPositionRuleTest, RejectsIncreasingShortAboveLimit) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Short,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{61, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_rejected());
}

TEST(MaxPositionRuleTest, ApprovesCrossingLongThroughFlat) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Long,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{60, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_approved());
}

TEST(MaxPositionRuleTest, ApprovesCrossingIntoShortWithinLimit) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Long,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{80, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_approved());
}

TEST(MaxPositionRuleTest, RejectsCrossingIntoShortAboveLimit) {
    const MaxPositionRule rule(Quantity{100, 0});

    TestPortfolioState portfolio_state;
    portfolio_state.add_position(
        Position(
            InstrumentId{42},
            PositionSide::Long,
            Quantity{40, 0},
            Price{15000, 2}
        )
    );

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Sell,
        Quantity{141, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    EXPECT_TRUE(decision.is_rejected());
}

TEST(MaxPositionRuleTest, RejectionContainsReason) {
    const MaxPositionRule rule(Quantity{100, 0});
    const TestPortfolioState portfolio_state;

    const OrderIntent order(
        InstrumentId{42},
        OrderSide::Buy,
        Quantity{101, 0}
    );

    const auto decision = rule.evaluate(order, portfolio_state);

    ASSERT_TRUE(decision.is_rejected());
    EXPECT_FALSE(decision.reason().empty());
}

} // namespace

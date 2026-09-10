#include "quantforge/portfolio/portfolio_state.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <utility>

namespace {

using quantforge::market::InstrumentId;
using quantforge::market::Price;
using quantforge::market::Quantity;
using quantforge::portfolio::PortfolioState;
using quantforge::portfolio::Position;

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

TEST(PortfolioStateTest, ReturnsNoPositionForUnknownInstrument) {
    const TestPortfolioState state;

    const auto position = state.find_position(InstrumentId{42});

    EXPECT_FALSE(position.has_value());
}

TEST(PortfolioStateTest, ReturnsExistingPosition) {
    TestPortfolioState state;

    state.add_position(
        Position(
            InstrumentId{42},
            Quantity{100, 0},
            Price{15000, 2}
        )
    );

    const auto position = state.find_position(InstrumentId{42});

    ASSERT_TRUE(position.has_value());
    EXPECT_EQ(position->instrument_id(), InstrumentId{42});
}

TEST(PortfolioStateTest, ReturnsPositionState) {
    TestPortfolioState state;

    state.add_position(
        Position(
            InstrumentId{42},
            Quantity{100, 0},
            Price{15000, 2}
        )
    );

    const auto position = state.find_position(InstrumentId{42});

    ASSERT_TRUE(position.has_value());
    EXPECT_EQ(position->quantity(), (Quantity{100, 0}));
    EXPECT_EQ(
        position->average_entry_price(),
        (Price{15000, 2})
    );
}

} // namespace

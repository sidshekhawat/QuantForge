#pragma once

#include <string>
#include <utility>

#include "quantforge/market/asset_class.hpp"
#include "quantforge/market/instrument_id.hpp"
#include "quantforge/market/price.hpp"
#include "quantforge/market/quantity.hpp"

namespace quantforge::market {

class Instrument {
public:
    Instrument(
        InstrumentId id,
        std::string symbol,
        AssetClass asset_class,
        std::string exchange,
        std::string currency,
        Price tick_size,
        Quantity lot_size
    );

    [[nodiscard]] InstrumentId id() const noexcept;
    [[nodiscard]] const std::string& symbol() const noexcept;
    [[nodiscard]] AssetClass asset_class() const noexcept;
    [[nodiscard]] const std::string& exchange() const noexcept;
    [[nodiscard]] const std::string& currency() const noexcept;
    [[nodiscard]] Price tick_size() const noexcept;
    [[nodiscard]] Quantity lot_size() const noexcept;

private:
    InstrumentId id_;
    std::string symbol_;
    AssetClass asset_class_;
    std::string exchange_;
    std::string currency_;
    Price tick_size_;
    Quantity lot_size_;
};

} // namespace quantforge::market
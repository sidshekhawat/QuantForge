#pragma once

#include "quantforge/market/bar.hpp"
#include "quantforge/marketdata/market_data_request.hpp"

#include <vector>

namespace quantforge::marketdata {

class MarketDataSource {
public:
    virtual ~MarketDataSource() = default;

    [[nodiscard]] virtual std::vector<market::Bar> get_bars(
        const MarketDataRequest& request
    ) = 0;
};

} // namespace quantforge::marketdata

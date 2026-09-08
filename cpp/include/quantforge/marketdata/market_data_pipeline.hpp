#pragma once

#include "quantforge/market/bar.hpp"
#include "quantforge/marketdata/market_data_request.hpp"
#include "quantforge/marketdata/market_data_source.hpp"

#include <vector>

namespace quantforge::marketdata {

class MarketDataPipeline {
public:
    explicit MarketDataPipeline(MarketDataSource& source) noexcept;

    [[nodiscard]] std::vector<market::Bar> get_bars(
        const MarketDataRequest& request
    );

private:
    MarketDataSource& source_;
};

} // namespace quantforge::marketdata

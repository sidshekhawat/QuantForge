#pragma once

#include "quantforge/marketdata/market_data_request.hpp"
#include "quantforge/market/validation_result.hpp"

namespace quantforge::marketdata {

class MarketDataRequestValidator {
public:
    [[nodiscard]] static market::ValidationResult validate(
        const MarketDataRequest& request
    );
};

} // namespace quantforge::marketdata

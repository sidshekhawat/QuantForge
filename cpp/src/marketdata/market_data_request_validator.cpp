#include "quantforge/marketdata/market_data_request_validator.hpp"

namespace quantforge::marketdata {

market::ValidationResult MarketDataRequestValidator::validate(
    const MarketDataRequest& request
)
{
    if (request.start() >= request.end()) {
        return market::ValidationResult::failure(
            "Market data request start must be before end."
        );
    }

    if (request.timeframe().value() == 0) {
        return market::ValidationResult::failure(
            "Market data request timeframe must be greater than zero."
        );
    }

    return market::ValidationResult::success();
}

} // namespace quantforge::marketdata

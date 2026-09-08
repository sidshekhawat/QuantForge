#include "quantforge/marketdata/market_data_pipeline.hpp"

namespace quantforge::marketdata {

MarketDataPipeline::MarketDataPipeline(
    MarketDataSource& source
) noexcept
    : source_(source)
{
}

std::vector<market::Bar> MarketDataPipeline::get_bars(
    const MarketDataRequest& request
)
{
    return source_.get_bars(request);
}

} // namespace quantforge::marketdata

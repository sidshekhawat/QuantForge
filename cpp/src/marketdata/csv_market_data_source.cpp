#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/market_data_exception.hpp"

#include <fstream>
#include <utility>

namespace quantforge::marketdata {

CsvMarketDataSource::CsvMarketDataSource(
    std::filesystem::path file_path
)
    : file_path_(std::move(file_path))
{
}

std::vector<market::Bar> CsvMarketDataSource::get_bars(
    const MarketDataRequest& request
)
{
    (void)request;

    std::ifstream file{file_path_};

    if (!file.is_open()) {
        throw MarketDataException{
            0,
            "Unable to open market data file: " + file_path_.string()
        };
    }

    return {};
}

} // namespace quantforge::marketdata

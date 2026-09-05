#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/market_data_exception.hpp"

#include <fstream>
#include <string>
#include <utility>

namespace quantforge::marketdata {

namespace {

constexpr std::string_view kExpectedHeader =
    "timestamp,instrument_id,timeframe,open,high,low,close,volume,price_scale,volume_scale";

} // namespace

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

    std::string header;

    if (!std::getline(file, header)) {
        throw MarketDataException{
            1,
            "Missing CSV header."
        };
    }

    if (header != kExpectedHeader) {
        throw MarketDataException{
            1,
            "Invalid CSV header."
        };
    }

    return {};
}

}

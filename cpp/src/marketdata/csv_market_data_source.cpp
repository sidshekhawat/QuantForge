#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/csv_bar_parser.hpp"
#include "quantforge/marketdata/market_data_exception.hpp"
#include "quantforge/marketdata/market_data_request_validator.hpp"
#include "quantforge/market/bar_validator.hpp"

#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
    const market::ValidationResult request_validation =
        MarketDataRequestValidator::validate(request);

    if (!request_validation.valid()) {
        throw MarketDataException{
            0,
            "Invalid market data request: "
            + request_validation.message()
        };
    }

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

    std::vector<market::Bar> bars;

    std::string line;
    std::size_t row_number = 1;

    while (std::getline(file, line)) {
        ++row_number;

        const market::Bar bar =
            CsvBarParser::parse(line, row_number);

        const market::ValidationResult validation =
            market::BarValidator::validate(bar);

        if (!validation.valid()) {
            throw MarketDataException{
                row_number,
                "Invalid bar: " + validation.message()
            };
        }

        if (bar.instrument_id() != request.instrument_id()) {
            continue;
        }

        if (bar.timeframe() != request.timeframe()) {
            continue;
        }

        if (bar.timestamp() < request.start()) {
            continue;
        }

        if (bar.timestamp() >= request.end()) {
            continue;
        }

        bars.push_back(bar);
    }

    return bars;
}

} // namespace quantforge::marketdata

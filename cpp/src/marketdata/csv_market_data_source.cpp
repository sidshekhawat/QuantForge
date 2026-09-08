#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/csv_bar_parser.hpp"
#include "quantforge/marketdata/market_data_exception.hpp"
#include "quantforge/marketdata/market_data_request_validator.hpp"
#include "quantforge/market/bar_validator.hpp"

#include <fstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

namespace quantforge::marketdata {

namespace {

constexpr std::string_view kExpectedHeader =
    "timestamp,instrument_id,timeframe,open,high,low,close,volume,price_scale,volume_scale";

struct BarIdentity {
    market::InstrumentId instrument_id;
    market::Timestamp timestamp;
    market::Timeframe timeframe;

    bool operator==(const BarIdentity&) const = default;
};

struct BarIdentityHash {
    std::size_t operator()(const BarIdentity& identity) const noexcept
    {
        const std::size_t instrument_hash =
            std::hash<std::uint64_t>{}(identity.instrument_id.value());

        const std::size_t timestamp_hash =
            std::hash<std::int64_t>{}(
                identity.timestamp.time_since_epoch().count()
            );

        const std::size_t timeframe_value_hash =
            std::hash<std::uint32_t>{}(identity.timeframe.value());

        const std::size_t timeframe_unit_hash =
            std::hash<std::uint8_t>{}(
                static_cast<std::uint8_t>(
                    identity.timeframe.unit()
                )
            );

        std::size_t seed = instrument_hash;

        seed ^= timestamp_hash
            + static_cast<std::size_t>(0x9e3779b9)
            + (seed << 6)
            + (seed >> 2);

        seed ^= timeframe_value_hash
            + static_cast<std::size_t>(0x9e3779b9)
            + (seed << 6)
            + (seed >> 2);

        seed ^= timeframe_unit_hash
            + static_cast<std::size_t>(0x9e3779b9)
            + (seed << 6)
            + (seed >> 2);

        return seed;
    }
};

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
    std::unordered_set<BarIdentity, BarIdentityHash> seen_bars;

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

        const BarIdentity identity{
            bar.instrument_id(),
            bar.timestamp(),
            bar.timeframe()
        };

        if (!seen_bars.insert(identity).second) {
            throw MarketDataException{
                row_number,
                "Duplicate bar identity."
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

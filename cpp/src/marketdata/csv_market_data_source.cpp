#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/market_data_exception.hpp"

#include <charconv>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace quantforge::marketdata {

namespace {

constexpr std::string_view kExpectedHeader =
    "timestamp,instrument_id,timeframe,open,high,low,close,volume,price_scale,volume_scale";

std::vector<std::string_view> split_csv_line(std::string_view line)
{
    std::vector<std::string_view> fields;

    std::size_t start = 0;

    while (true) {
        const std::size_t comma = line.find(',', start);

        if (comma == std::string_view::npos) {
            fields.push_back(line.substr(start));
            break;
        }

        fields.push_back(line.substr(start, comma - start));
        start = comma + 1;
    }

    return fields;
}

market::InstrumentId parse_instrument_id(
    std::string_view value,
    std::size_t row_number
)
{
    if (value.empty()) {
        throw MarketDataException{
            row_number,
            "Invalid instrument_id: value is empty."
        };
    }

    std::uint64_t parsed_value{};

    const auto [pointer, error] = std::from_chars(
        value.data(),
        value.data() + value.size(),
        parsed_value
    );

    if (
        error != std::errc{}
        || pointer != value.data() + value.size()
    ) {
        throw MarketDataException{
            row_number,
            "Invalid instrument_id: expected an unsigned integer."
        };
    }

    return market::InstrumentId{parsed_value};
}

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

    std::string line;
    std::size_t row_number = 1;

    while (std::getline(file, line)) {
        ++row_number;

        const auto fields = split_csv_line(line);

        if (fields.size() != 10) {
            throw MarketDataException{
                row_number,
                "Invalid CSV row: expected 10 fields."
            };
        }

        const market::InstrumentId instrument_id =
            parse_instrument_id(fields[1], row_number);

        (void)instrument_id;
    }

    return {};
}

}

#include "quantforge/marketdata/csv_market_data_source.hpp"

#include "quantforge/marketdata/market_data_exception.hpp"
#include "quantforge/market/timeframe.hpp"

#include <charconv>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <stdexcept>
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

market::Timeframe parse_csv_timeframe(
    std::string_view value,
    std::size_t row_number
)
{
    try {
        return market::parse_timeframe(value);
    }
    catch (const std::invalid_argument&) {
        throw MarketDataException{
            row_number,
            "Invalid timeframe: expected canonical format such as 1m or 1h."
        };
    }
}

int parse_fixed_digits(
    std::string_view value,
    std::size_t offset,
    std::size_t count
)
{
    int result = 0;

    for (std::size_t i = 0; i < count; ++i) {
        const char character = value[offset + i];

        if (!std::isdigit(
                static_cast<unsigned char>(character)
            )) {
            throw std::invalid_argument{"Expected decimal digit."};
        }

        result = result * 10 + (character - '0');
    }

    return result;
}

market::Timestamp parse_csv_timestamp(
    std::string_view value,
    std::size_t row_number
)
{
    try {
        if (
            value.size() < 20
            || value.back() != 'Z'
        ) {
            throw std::invalid_argument{"Invalid timestamp format."};
        }

        const int year_value = parse_fixed_digits(value, 0, 4);
        const int month_value = parse_fixed_digits(value, 5, 2);
        const int day_value = parse_fixed_digits(value, 8, 2);
        const int hour = parse_fixed_digits(value, 11, 2);
        const int minute = parse_fixed_digits(value, 14, 2);
        const int second = parse_fixed_digits(value, 17, 2);

        if (
            value[4] != '-'
            || value[7] != '-'
            || value[10] != 'T'
            || value[13] != ':'
            || value[16] != ':'
        ) {
            throw std::invalid_argument{"Invalid timestamp format."};
        }

        if (
            month_value < 1 || month_value > 12
            || day_value < 1 || day_value > 31
            || hour > 23
            || minute > 59
            || second > 59
        ) {
            throw std::invalid_argument{"Invalid timestamp value."};
        }

        std::int64_t nanoseconds = 0;

        if (value.size() > 20) {
            if (value[19] != '.') {
                throw std::invalid_argument{
                    "Invalid fractional second format."
                };
            }

            const std::size_t fraction_length = value.size() - 21;

            if (
                fraction_length == 0
                || fraction_length > 9
            ) {
                throw std::invalid_argument{
                    "Invalid fractional second precision."
                };
            }

            for (std::size_t i = 0; i < fraction_length; ++i) {
                const char character = value[20 + i];

                if (!std::isdigit(
                        static_cast<unsigned char>(character)
                    )) {
                    throw std::invalid_argument{
                        "Invalid fractional second."
                    };
                }

                nanoseconds =
                    nanoseconds * 10
                    + (character - '0');
            }

            for (
                std::size_t i = fraction_length;
                i < 9;
                ++i
            ) {
                nanoseconds *= 10;
            }
        }

        const std::chrono::year_month_day calendar_date{
            std::chrono::year{year_value},
            std::chrono::month{
                static_cast<unsigned>(month_value)
            },
            std::chrono::day{
                static_cast<unsigned>(day_value)
            }
        };

        if (!calendar_date.ok()) {
            throw std::invalid_argument{
                "Invalid calendar date."
            };
        }

        const std::chrono::sys_days days{
            calendar_date
        };

        return market::Timestamp{
            days.time_since_epoch()
            + std::chrono::hours{hour}
            + std::chrono::minutes{minute}
            + std::chrono::seconds{second}
            + std::chrono::nanoseconds{nanoseconds}
        };
    }
    catch (const std::exception&) {
        throw MarketDataException{
            row_number,
            "Invalid timestamp: expected UTC ISO-8601 format."
        };
    }
}

std::uint8_t parse_price_scale(
    std::string_view value,
    std::size_t row_number
)
{
    if (value.empty()) {
        throw MarketDataException{
            row_number,
            "Invalid price_scale: value is empty."
        };
    }

    std::uint32_t parsed_value{};

    const auto [pointer, error] = std::from_chars(
        value.data(),
        value.data() + value.size(),
        parsed_value
    );

    if (
        error != std::errc{}
        || pointer != value.data() + value.size()
        || parsed_value > 255
    ) {
        throw MarketDataException{
            row_number,
            "Invalid price_scale: expected an integer from 0 to 255."
        };
    }

    return static_cast<std::uint8_t>(parsed_value);
}

std::uint8_t parse_volume_scale(
    std::string_view value,
    std::size_t row_number
)
{
    if (value.empty()) {
        throw MarketDataException{
            row_number,
            "Invalid volume_scale: value is empty."
        };
    }

    std::uint32_t parsed_value{};

    const auto [pointer, error] = std::from_chars(
        value.data(),
        value.data() + value.size(),
        parsed_value
    );

    if (
        error != std::errc{}
        || pointer != value.data() + value.size()
        || parsed_value > 255
    ) {
        throw MarketDataException{
            row_number,
            "Invalid volume_scale: expected an integer from 0 to 255."
        };
    }

    return static_cast<std::uint8_t>(parsed_value);
}

market::Quantity parse_volume(
    std::string_view value,
    std::uint8_t scale,
    std::size_t row_number
)
{
    if (value.empty()) {
        throw MarketDataException{
            row_number,
            "Invalid volume: value is empty."
        };
    }

    std::int64_t parsed_value{};

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
            "Invalid volume: expected a signed integer."
        };
    }

    return market::Quantity{
        parsed_value,
        scale
    };
}

market::Price parse_price(
    std::string_view value,
    std::uint8_t scale,
    std::size_t row_number
)
{
    if (value.empty()) {
        throw MarketDataException{
            row_number,
            "Invalid price: value is empty."
        };
    }

    std::int64_t parsed_value{};

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
            "Invalid price: expected a signed integer."
        };
    }

    return market::Price{
        parsed_value,
        scale
    };
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

    std::vector<market::Bar> bars;

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

        const market::Timestamp timestamp =
            parse_csv_timestamp(fields[0], row_number);

        const market::InstrumentId instrument_id =
            parse_instrument_id(fields[1], row_number);

        const market::Timeframe timeframe =
            parse_csv_timeframe(fields[2], row_number);

        const std::uint8_t price_scale =
            parse_price_scale(fields[8], row_number);

        const market::Price open =
            parse_price(fields[3], price_scale, row_number);

        const market::Price high =
            parse_price(fields[4], price_scale, row_number);

        const market::Price low =
            parse_price(fields[5], price_scale, row_number);

        const market::Price close =
            parse_price(fields[6], price_scale, row_number);

        const std::uint8_t volume_scale =
            parse_volume_scale(fields[9], row_number);

        const market::Quantity volume =
            parse_volume(fields[7], volume_scale, row_number);

        bars.emplace_back(
            instrument_id,
            timestamp,
            timeframe,
            open,
            high,
            low,
            close,
            volume
        );
    }

    return bars;
}

}

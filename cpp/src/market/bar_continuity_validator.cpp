#include "quantforge/market/bar_continuity_validator.hpp"

#include <chrono>

namespace quantforge::market {

namespace {

[[nodiscard]] std::chrono::nanoseconds timeframe_duration(
    Timeframe timeframe
)
{
    using namespace std::chrono;

    switch (timeframe.unit()) {
        case TimeframeUnit::Second:
            return seconds{timeframe.value()};

        case TimeframeUnit::Minute:
            return minutes{timeframe.value()};

        case TimeframeUnit::Hour:
            return hours{timeframe.value()};

        case TimeframeUnit::Day:
            return hours{24 * timeframe.value()};

        case TimeframeUnit::Tick:
            return nanoseconds{0};
    }

    return nanoseconds{0};
}

} // namespace

ValidationResult BarContinuityValidator::validate(
    const std::vector<Bar>& bars
)
{
    if (bars.size() < 2) {
        return ValidationResult::success();
    }

    const InstrumentId instrument_id = bars.front().instrument_id();
    const Timeframe timeframe = bars.front().timeframe();

    for (std::size_t index = 1; index < bars.size(); ++index) {
        const Bar& previous = bars[index - 1];
        const Bar& current = bars[index];

        if (current.instrument_id() != instrument_id) {
            return ValidationResult::failure(
                "Bars contain multiple instrument ids."
            );
        }

        if (current.timeframe() != timeframe) {
            return ValidationResult::failure(
                "Bars contain multiple timeframes."
            );
        }

        if (current.instrument_id() != previous.instrument_id()) {
            return ValidationResult::failure(
                "Consecutive bars have different instrument ids."
            );
        }

        if (current.timeframe() != previous.timeframe()) {
            return ValidationResult::failure(
                "Consecutive bars have different timeframes."
            );
        }

        if (timeframe.unit() == TimeframeUnit::Tick) {
            continue;
        }

        const Timestamp expected_timestamp =
            previous.timestamp() + timeframe_duration(timeframe);

        if (current.timestamp() != expected_timestamp) {
            return ValidationResult::failure(
                "Bar sequence contains a timestamp gap."
            );
        }
    }

    return ValidationResult::success();
}

} // namespace quantforge::market

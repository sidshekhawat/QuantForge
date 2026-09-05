#pragma once

#include "quantforge/marketdata/market_data_source.hpp"

#include <filesystem>
#include <vector>

namespace quantforge::marketdata {

class CsvMarketDataSource final : public MarketDataSource {
public:
    explicit CsvMarketDataSource(std::filesystem::path file_path);

    [[nodiscard]] std::vector<market::Bar> get_bars(
        const MarketDataRequest& request
    ) override;

private:
    std::filesystem::path file_path_;
};

} // namespace quantforge::marketdata

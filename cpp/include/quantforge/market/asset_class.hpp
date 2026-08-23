#pragma once

namespace quantforge::market {

enum class AssetClass {
    Equity,
    Future,
    Option,
    Forex,
    Crypto,
    Index,
    ETF
};

} // namespace quantforge::market
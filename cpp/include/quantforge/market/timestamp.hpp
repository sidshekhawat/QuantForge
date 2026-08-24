#pragma once

#include <chrono>

namespace quantforge::market {

using Timestamp = std::chrono::sys_time<std::chrono::nanoseconds>;

} // namespace quantforge::market
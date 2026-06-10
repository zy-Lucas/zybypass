#pragma once

#include <cstdint>

const int MILLIUNITS = 1000;      // milli units per base unit
const int MICROUNITS = 1000000;   // micro units per base unit
const int NANOUNITS = 1000000000; // nano units per base unit
const int NANOUNITS_PER_MILLIUNIT = NANOUNITS / MILLIUNITS;

constexpr int64_t nanos_to_millis(int64_t nanos) noexcept { return nanos / NANOUNITS_PER_MILLIUNIT; }
constexpr int64_t millis_to_nanos(int64_t millis) noexcept { return millis * NANOUNITS_PER_MILLIUNIT; }
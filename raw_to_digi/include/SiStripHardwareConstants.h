#ifndef SNDHLLHC_SISTRIP_HARDWARE_CONSTANTS_H
#define SNDHLLHC_SISTRIP_HARDWARE_CONSTANTS_H

#include <cstdint>

inline constexpr uint16_t FEDCH_PER_FEUNIT = 12;
inline constexpr uint16_t FEUNITS_PER_FED = 8;
inline constexpr uint16_t FEDCH_PER_FED = FEDCH_PER_FEUNIT * FEUNITS_PER_FED;
inline constexpr uint16_t SISTRIPS_PER_APV_PAIR = 256;

#endif
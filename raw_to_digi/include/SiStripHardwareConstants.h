#ifndef SNDHLLHC_SISTRIP_HARDWARE_CONSTANTS
#define SNDHLLHC_SISTRIP_HARDWARE_CONSTANTS

#include <cstdint>

static const uint16_t FEDCH_PER_FEUNIT = 12;
static const uint16_t FEUNITS_PER_FED = 8;
static const uint16_t FEDCH_PER_FED = FEDCH_PER_FEUNIT * FEUNITS_PER_FED;

#endif
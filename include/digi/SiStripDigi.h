#ifndef SNDHLLHC_SISTRIPDIGI_H
#define SNDHLLHC_SISTRIPDIGI_H

#include <cstdint>

class SiStripDigi {
public:
  SiStripDigi(const uint16_t& strip, const uint16_t& adc) : strip_(strip), adc_(adc) { ; }
  SiStripDigi() : strip_(0), adc_(0) { ; }

  inline const uint16_t& strip() const { return strip_; }
  inline const uint16_t& adc() const { return adc_; }

private:
  uint16_t strip_;
  uint16_t adc_;
};

#endif
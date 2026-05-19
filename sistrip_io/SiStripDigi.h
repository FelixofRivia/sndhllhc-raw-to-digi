#ifndef SNDHLLHC_SISTRIPDIGI_H
#define SNDHLLHC_SISTRIPDIGI_H

#include <cstdint>

class SiStripDigi {
public:
  SiStripDigi(const uint16_t& strip, const uint16_t& adc, const uint32_t& fed_key) : strip_(strip), adc_(adc), fed_key_(fed_key) { ; }
  SiStripDigi() : strip_(0), adc_(0), fed_key_(0) { ; }

  inline const uint16_t& strip() const { return strip_; }
  inline const uint16_t& adc() const { return adc_; }
  inline const uint32_t& fed_key() const { return fed_key_; }

private:
  uint16_t strip_;
  uint16_t adc_;
  uint32_t fed_key_;
};

#endif
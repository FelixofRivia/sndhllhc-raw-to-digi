#ifndef SNDHLLHC_SISTRIPDIGI_H
#define SNDHLLHC_SISTRIPDIGI_H

#include <cstdint>

class SiStripDigi {
public:
  SiStripDigi(const uint16_t& strip, const uint16_t& adc, const uint32_t& id) : strip_(strip), adc_(adc), det_id_(id) { ; }
  SiStripDigi() : strip_(0), adc_(0) { ; }

  inline const uint16_t& strip() const { return strip_; }
  inline const uint16_t& adc() const { return adc_; }
  inline const uint32_t& id() const { return det_id_; }

private:
  uint16_t strip_;
  uint16_t adc_;
  uint32_t det_id_;
};

#endif
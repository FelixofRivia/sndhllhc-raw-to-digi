#ifndef EventFilter_SiStripRawToDigi_FEDFullDebugHeader_H
#define EventFilter_SiStripRawToDigi_FEDFullDebugHeader_H

#include"SiStripHardwareConstants.h"

#include <cstdint>
#include <memory>

class FEDFullDebugHeader {
  public:
    FEDFullDebugHeader(const uint8_t* headerBuffer);
    uint16_t feUnitLength(const uint8_t internalFEUnitNum) const;
    bool fePresent(const uint8_t internalFEUnitNum) const;
    const uint8_t* feWord(const uint8_t internalFEUnitNum) const;
    uint32_t daqRegister() const;
    static uint32_t get32BitWordFrom(const uint8_t* startOfWord);
    static const size_t FULL_DEBUG_HEADER_SIZE_IN_64BIT_WORDS = FEUNITS_PER_FED * 2;
    static const size_t FULL_DEBUG_HEADER_SIZE_IN_BYTES = FULL_DEBUG_HEADER_SIZE_IN_64BIT_WORDS * 8;
  private:
    uint8_t header_[FULL_DEBUG_HEADER_SIZE_IN_BYTES];

};

inline FEDFullDebugHeader::FEDFullDebugHeader(const uint8_t* headerBuffer) {
  memcpy(header_, headerBuffer, FULL_DEBUG_HEADER_SIZE_IN_BYTES);
}

inline uint16_t FEDFullDebugHeader::feUnitLength(const uint8_t internalFEUnitNum) const {
  return ((feWord(internalFEUnitNum)[15] << 8) | (feWord(internalFEUnitNum)[14]));
}

inline bool FEDFullDebugHeader::fePresent(const uint8_t internalFEUnitNum) const {
  return (feUnitLength(internalFEUnitNum) != 0);
}

inline const uint8_t* FEDFullDebugHeader::feWord(const uint8_t internalFEUnitNum) const {
  return header_ + internalFEUnitNum * 2 * 8;
}

inline uint32_t FEDFullDebugHeader::daqRegister() const { return get32BitWordFrom(feWord(7) + 10); }

inline uint32_t FEDFullDebugHeader::get32BitWordFrom(const uint8_t* startOfWord) {
  return (startOfWord[0] | (startOfWord[1] << 8) | (startOfWord[2] << 16) | (startOfWord[3] << 24));
}
  
#endif  //ndef EventFilter_SiStripRawToDigi_FEDFullDebugHeader_H
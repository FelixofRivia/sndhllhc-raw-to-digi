#include "SiStripFEDBuffer.h"
#include "SiStripFEDChannel.h"
#include "SiStripHardwareConstants.h"
#include "FEDFullDebugHeader.h"

#include <cstdint>

FEDBuffer::FEDBuffer(const FEDRawData& fedBuffer) : originalBuffer_(fedBuffer.data()), bufferSize_(fedBuffer.size()) {

  orderedBuffer_ = originalBuffer_;
  validChannels_ = 0;
  
  channels_.reserve(FEDCH_PER_FED);
  const size_t header_lenght_in_bytes{128};
  feHeader_ = std::unique_ptr<FEDFullDebugHeader>(new FEDFullDebugHeader(getPointerToDataAfterTrackerSpecialHeader()));
  payloadPointer_ = getPointerToDataAfterTrackerSpecialHeader() + header_lenght_in_bytes;
  payloadLength_ = getPointerToByteAfterEndOfPayload() - payloadPointer_;

  if (feHeader_) {
    for (uint8_t iFE = 0; iFE < FEUNITS_PER_FED; ++iFE) {
      if (feHeader_->fePresent(iFE))
        fePresent_[iFE] = true;
      else
        fePresent_[iFE] = false;
    }
  }
}

inline const uint8_t* FEDBuffer::getPointerToDataAfterTrackerSpecialHeader() const { return orderedBuffer_ + 16; }

inline const uint8_t* FEDBuffer::getPointerToByteAfterEndOfPayload() const {
  return orderedBuffer_ + bufferSize_ - 8;
}

void FEDBuffer::findChannels() {
  uint16_t offsetBeginningOfChannel = 0;
  for (uint16_t i = 0; i < FEDCH_PER_FED; i++) {
    if (!(fePresent(i / FEDCH_PER_FEUNIT))) { //should also check for feEnabled but it seems always true
      channels_.insert(channels_.end(), uint16_t(FEDCH_PER_FEUNIT), FEDChannel(payloadPointer_, 0, 0));
      i += FEDCH_PER_FEUNIT - 1;
      validChannels_ += FEDCH_PER_FEUNIT;
      continue;
    }
    channels_.emplace_back(payloadPointer_, offsetBeginningOfChannel);
    uint16_t channelLength = channels_.back().length();

    validChannels_++;
    const uint16_t offsetEndOfChannel = offsetBeginningOfChannel + channelLength;
    //add padding if necessary and calculate offset for begining of next channel
    if (!((i + 1) % FEDCH_PER_FEUNIT)) {
      uint8_t numPaddingBytes = 8 - (offsetEndOfChannel % 8);
      if (numPaddingBytes == 8)
        numPaddingBytes = 0;
      offsetBeginningOfChannel = offsetEndOfChannel + numPaddingBytes;
    } else {
      offsetBeginningOfChannel = offsetEndOfChannel;
    }
  }
}

inline bool FEDBuffer::fePresent(uint8_t internalFEUnitNum) const { return fePresent_[internalFEUnitNum]; }

// inline bool FEDBufferBase::feEnabled(const uint8_t internalFEUnitNum) const {
//   return specialHeader_.feEnabled(internalFEUnitNum);
// }
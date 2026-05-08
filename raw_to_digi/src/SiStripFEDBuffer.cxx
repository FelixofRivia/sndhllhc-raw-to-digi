#include "SiStripFEDBuffer.h"
#include "SiStripFEDChannel.h"
#include "SiStripHardwareConstants.h"

#include <cstdint>

FEDBuffer::FEDBuffer(const FEDRawData& fedBuffer) : originalBuffer_(fedBuffer.data()), bufferSize_(fedBuffer.size()) {

  // //swap the buffer words so that the whole buffer is in slink ordering
  // uint8_t* newBuffer = new uint8_t[bufferSize_];
  // const uint32_t* originalU32 = reinterpret_cast<const uint32_t*>(originalBuffer_);
  // const size_t sizeU32 = bufferSize_ / 4;
  // uint32_t* newU32 = reinterpret_cast<uint32_t*>(newBuffer);

  // //copy DAQ header
  // memcpy(newU32, originalU32, 8);
  // //copy DAQ trailer
  // memcpy(newU32 + sizeU32 - 2, originalU32 + sizeU32 - 2, 8);
  // //swap the payload
  // for (size_t i = 2; i < sizeU32 - 2; i += 2) {
  //   newU32[i] = originalU32[i + 1];
  //   newU32[i + 1] = originalU32[i];
  // }
  orderedBuffer_ = originalBuffer_;
  //construct header object at begining of buffer
  // daqHeader_ = FEDDAQHeader(orderedBuffer_);
  // //construct trailer object using last 64 bit word of buffer
  // daqTrailer_ = FEDDAQTrailer(orderedBuffer_ + bufferSize_ - 8);

  validChannels_ = 0;
  channels_.reserve(FEDCH_PER_FED);
  const size_t header_lenght_in_bytes{128};
  //feHeader_ = FEDFEHeader::newFEHeader(headerType(), getPointerToDataAfterTrackerSpecialHeader());
  payloadPointer_ = getPointerToDataAfterTrackerSpecialHeader() + header_lenght_in_bytes;
  payloadLength_ = getPointerToByteAfterEndOfPayload() - payloadPointer_;
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

inline uint16_t FEDBuffer::feUnitLength(const uint8_t internalFEUnitNum) const
  {
    return ( (feWord(internalFEUnitNum)[15]<<8) | (feWord(internalFEUnitNum)[14]) );
  }

inline bool FEDBuffer::fePresent(const uint8_t internalFEUnitNum) const
  {
    return (feUnitLength(internalFEUnitNum) != 0);
  }

inline const uint8_t* FEDBuffer::feWord(const uint8_t internalFEUnitNum) const
  {
    return header_+internalFEUnitNum*2*8;
  }
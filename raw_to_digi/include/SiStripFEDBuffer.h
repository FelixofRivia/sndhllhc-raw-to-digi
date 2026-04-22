#ifndef EventFilter_SiStripRawToDigi_SiStripFEDBuffer_H
#define EventFilter_SiStripRawToDigi_SiStripFEDBuffer_H

#include "SiStripIOHeaders.h"
#include "SiStripFEDChannel.h"

#include <vector>
#include <cstdint>

//class representing standard (non-spy channel) FED buffers
class FEDBuffer {
  public:
    explicit FEDBuffer(const FEDRawData& fedBuffer, const bool allowBadBuffer = false);

    void findChannels();
    const FEDChannel& channel(const uint8_t internalFEDChannelNum) const;

  private:
    const uint8_t* getPointerToDataAfterTrackerSpecialHeader() const;
    const uint8_t* getPointerToByteAfterEndOfPayload() const;
    std::vector<FEDChannel> channels_;
    const uint8_t* originalBuffer_;
    const uint8_t* orderedBuffer_;
    const size_t bufferSize_;
    // FEDDAQHeader daqHeader_;
    // FEDDAQTrailer daqTrailer_;
    // std::unique_ptr<FEDFEHeader> feHeader_;
    const uint8_t* payloadPointer_;
    uint16_t payloadLength_;
    uint8_t validChannels_;
};

  
#endif  //ndef EventFilter_SiStripRawToDigi_SiStripFEDBuffer_H
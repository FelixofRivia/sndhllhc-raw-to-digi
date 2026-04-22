#ifndef SNDHLLHC_SISTRIPFEDCHANNEL_H
#define SNDHLLHC_SISTRIPFEDCHANNEL_H


#include "SiStripFEDChannel.h"

#include <cstdint>

//holds information about position of a channel in the buffer for use by unpacker
class FEDChannel {
    public:
        //gets length from first 2 bytes (assuming normal FED channel)
        constexpr FEDChannel(const uint8_t* const data, const uint32_t offset) : data_(data), offset_(offset), length_(data[(offset) ^ 7] + (data[(offset + 1) ^ 7] << 8)) {}
        constexpr uint16_t stripsInCh(uint8_t num_bits) const;
        constexpr uint16_t length() const { return length_; }
        constexpr const uint8_t* data() const { return data_; }
        constexpr uint32_t offset() const { return offset_; }
        /**
            * Retrieve the APV CM median for a non-lite zero-suppressed channel
            *
            * apvIndex should be either 0 or 1 (there are, by construction, two APVs on every channel)
            * No additional checks are done here, so the caller should check
            * the readout mode and/or packet code.
            */
        constexpr uint16_t cmMedian(const uint8_t apvIndex) const;
        //third byte of channel data for normal FED channels
        constexpr uint8_t packetCode() const { return data_[(offset_ + 2) ^ 7]; }

    private:
        //friend class FEDBuffer;
        const uint8_t* data_;
        uint32_t offset_;
        uint16_t length_;
        uint8_t headerLen_ = 0;
};


// constexpr FEDChannel::FEDChannel(const uint8_t* const data, const uint32_t offset) : data_(data), offset_(offset), lenght_(data[(offset) ^ 7] + (data[(offset + 1) ^ 7] << 8)) {}

constexpr uint16_t FEDChannel::stripsInCh(uint8_t num_bits) const {
    const bool emptyCh = (headerLen_ + 2) >= (length_);
    const uint16_t start = offset_ + headerLen_;
    const uint16_t end = offset_ + length_;
    uint16_t stripN = 0;
    if (!emptyCh) {
        for (uint16_t nStrip_wOfs = start + 1; nStrip_wOfs < end;) {
        const uint8_t clustStripN = data_[(nStrip_wOfs) ^ 7];
        nStrip_wOfs += ((uint32_t)clustStripN) * num_bits / 8 + 2;
        stripN += clustStripN;
        }
    }
    return stripN;
}

constexpr uint16_t FEDChannel::cmMedian(const uint8_t apvIndex) const {
    uint16_t result = 0;
    //CM median is 10 bits with lowest order byte first. First APV CM median starts in 4th byte of channel data
    result |= data_[(offset_ + 3 + 2 * apvIndex) ^ 7];
    result |= (((data_[(offset_ + 4 + 2 * apvIndex) ^ 7]) << 8) & 0x300);
    return result;
}

#endif
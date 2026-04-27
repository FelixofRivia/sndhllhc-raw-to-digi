#ifndef SNDHLLHC_SISTRIPRAWTODIGI_H
#define SNDHLLHC_SISTRIPRAWTODIGI_H

#include "SiStripIOHeaders.h"
#include "SiStripHardwareConstants.h"
#include "SiStripFEDBuffer.h"
#include "SiStripFEDChannel.h"
#include <vector>
#include <cstdint>

template <uint8_t num_words>
constexpr uint16_t getADC_W(const uint8_t* data, uint_fast16_t offset, uint8_t bits_shift) {
    // get ADC from one or two bytes (at most 10 bits), and shift if needed
    return (data[offset ^ 7] + (num_words == 2 ? ((data[(offset + 1) ^ 7] & 0x03) << 8) : 0)) << bits_shift;
}

class SiStripRawToDigi {
    public:
        SiStripRawToDigi() = default;
        std::vector<SiStripDigi> operator()(const edm::Wrapper<FEDRawDataCollection>& sistrip_raw) const;
};

std::vector<SiStripDigi> SiStripRawToDigi::operator()(const edm::Wrapper<FEDRawDataCollection>& sistrip_raw) const {
    std::vector<SiStripDigi> digis;

    size_t fed_index{116};
    FEDRawData data = sistrip_raw.obj.data_[fed_index];
    FEDBuffer buffer(data);
    buffer.findChannels();
    // LOOP ON FED CHANNELS
    for (uint8_t i_ch{0}; i_ch < FEDCH_PER_FED; ++i_ch) {
        auto channel = buffer.channel(i_ch);
        if (channel.length() == 0) {
            continue;
        }
        // const uint8_t pCode = buffer.packetCode(legacy_, iconn->fedCh()); only needed for bits_shift
    
        const uint16_t stripStart{0};
        const uint8_t num_words{1};
        const uint8_t headerLength{7};
        const uint8_t bits_shift{1};
        const uint8_t* const data = channel.data();
        uint_fast16_t offset = channel.offset() + headerLength;  // header is 2 (lite) or 7
        uint_fast8_t firstStrip{0}, nInCluster{0}, inCluster{0};
        const uint_fast16_t end = channel.offset() + channel.length();
        while (offset != end) {
            if (inCluster == nInCluster) {
                if (offset + 2 >= end) {
                    // offset should already be at end then (empty cluster)
                    break;
                }
                const uint_fast8_t newFirstStrip = data[(offset++) ^ 7];
                firstStrip = newFirstStrip;
                nInCluster = data[(offset++) ^ 7];
                inCluster = 0;
            }
            // Use i_ch as det_id, for the moment
            digis.emplace_back(SiStripDigi(stripStart + firstStrip + inCluster, getADC_W<num_words>(data, offset, bits_shift), i_ch));
            offset += num_words;
            ++inCluster;
        }
    }
    return digis;
}

#endif
#ifndef SNDHLLHC_SISTRIPRAWTODIGI_H
#define SNDHLLHC_SISTRIPRAWTODIGI_H

#include "SiStripIOHeaders.h"
#include <vector>

std::vector<SiStripDigi> SiStripRawToDigi::operator()(const edm::Wrapper<FEDRawDataCollection>& sistrip_raw) const {
    size_t fed_index{116};
    FEDRawData data = sistrip_raw.obj.data[fed_index];
    FEDBuffer buffer(data);
    buffer.findChannels();
    FEDChannel channel = buffer.channel(42);
    
    std::vector<SiStripDigi> digis;
    SiStripDigi(stripStart + firstStrip + inCluster, getADC_W<num_words>(data, offset, bits_shift);
    for (uint16_t i{0}; i < 4; ++i) {
        digis.emplace_back(SiStripDigi(i, 2*i + 1));
    }
    return digis;
}

#endif
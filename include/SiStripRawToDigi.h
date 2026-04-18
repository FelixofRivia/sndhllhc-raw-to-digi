#ifndef SNDHLLHC_SISTRIPRAWTODIGI_H
#define SNDHLLHC_SISTRIPRAWTODIGI_H

#include "SNDHLLHCDigiHeaders.h"
#include "SNDHLLHCRawHeaders.h"
#include <vector>

class SiStripRawToDigi {
    public:
        SiStripRawToDigi() = default;
        std::vector<SiStripDigi> operator()(const edm::Wrapper<FEDRawDataCollection>& sistrip_raw) const;
};

std::vector<SiStripDigi> SiStripRawToDigi::operator()(const edm::Wrapper<FEDRawDataCollection>& sistrip_raw) const {
    std::vector<SiStripDigi> digis;
    for (uint16_t i{0}; i < 4; ++i) {
        digis.emplace_back(SiStripDigi(i, 2*i + 1));
    }
    return digis;
}

#endif
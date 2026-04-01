#ifndef ADVSND_ADVRAWTODIGI_H
#define ADVSND_ADVRAWTODIGI_H

#include "SiStripFedCabling.h"
#include "FEDRawDataCollection.h" 
#include "SiStripEventSummary.h"
#include "SiStripRawDigi.h"
#include "SiStriDigi.h"
#include "DetId.h"

namespace advsnd {
  void CreateDigis(const SiStripFedCabling& cabling,
                    const FEDRawDataCollection& buffers,
                    SiStripEventSummary& summary,
                    std::vector<SiStripRawDigi>& scope_mode,
                    std::vector<SiStripRawDigi>& virgin_raw,
                    std::vector<SiStripRawDigi>& proc_raw,
                    std::vector<SiStripDigi>& zero_suppr,
                    std::vector<DetId>& detids,
                    std::vector<SiStripRawDigi>& cm_values); 
}


#endif
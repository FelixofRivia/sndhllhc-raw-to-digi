#include "advRawToDigi.h"
#include "SiStripFedCabling.h"
#include "FEDRawDataCollection.h" 
#include "SiStripEventSummary.h"
#include "SiStripRawDigi.h"
#include "SiStripDigi.h"
#include "SiStripFEDBuffer.h"
#include "SiStripFEDBufferComponents.h"
#include "ConstantsForHardwareSystems.h"
#include "DetId.h"


void advsnd::CreateDigis(const SiStripFedCabling& cabling,
                          const FEDRawDataCollection& buffers,
                          SiStripEventSummary& summary,
                          std::vector<SiStripRawDigi>& scope_mode,
                          std::vector<SiStripRawDigi>& virgin_raw,
                          std::vector<SiStripRawDigi>& proc_raw,
                          std::vector<SiStripDigi>& zero_suppr,
                          std::vector<DetId>& detids,
                          std::vector<SiStripRawDigi>& cm_values) {
    // Clear done at the end
    std::vector<SiStripDigi> zs_work_digis;
    // zs_work_digis.reserve(localRA.upper());
    // Reserve space in bad module list
    detids.reserve(100);

    // Check if FEDs found in cabling map and event data
    if (cabling.fedIds().empty()) {
      std::cout << "[WARNING] No FEDs found in cabling map!\n";
    }

    // Flag for EventSummary update using DAQ register
    bool first_fed = true;

    // Retrieve FED ids from cabling map and iterate through
    std::vector<uint16_t>::const_iterator ifed = cabling.fedIds().begin();
    for (; ifed != cabling.fedIds().end(); ifed++) {
      // ignore trigger FED
      if (*ifed == triggerFedId_) {
        continue;
      }

      // Retrieve FED raw data for given FED
      const FEDRawData& input = buffers.FEDData(static_cast<int>(*ifed));

      // get the cabling connections for this FED
      auto conns = cabling.fedConnections(*ifed);
      // check FEDRawData pointer, size, and more
      const FEDBufferStatusCode st_buffer = preconstructCheckFEDBuffer(input);
      // construct FEDBuffer
      if (FEDBufferStatusCode::SUCCESS != st_buffer) {
        // if (FEDBufferStatusCode::BUFFER_NULL == st_buffer) {
        //   warnings_.add("NULL pointer to FEDRawData for FED", std::format("id {0}", *ifed));
        // } else if (!input.size()) {
        //   warnings_.add("FEDRawData has zero size for FED", std::format("id {0}", *ifed));
        // } else {
        //   warnings_.add("Exception caught when creating FEDBuffer object for FED",
        //                 std::format("id {0}: {1}", *ifed, static_cast<int>(st_buffer)));
        // }
        // FED buffer is bad and should not be unpacked. Skip this FED and mark all modules as bad.
        maskFED(detids, conns);
        continue;
      }
      FEDBuffer buffer{input};
      const FEDBufferStatusCode st_chan = buffer.findChannels();
      if (FEDBufferStatusCode::SUCCESS != st_chan) {
        // warnings_.add("Exception caught when creating FEDBuffer object for FED",
        //               std::format("id {0}: {1}", *ifed, static_cast<int>(st_chan)));
        maskFED(detids, conns);
        continue;
      }
      buffer.setLegacyMode(legacy_);
      if ((!buffer.doChecks(true)) && (!unpackBadChannels_ || !buffer.checkNoFEOverflows())) {
        // warnings_.add("Exception caught when creating FEDBuffer object for FED",
        //               std::format("id {0}: FED Buffer check fails for FED ID {0}.", *ifed));
        maskFED(detids, conns);
        continue;
      }
      if (doFullCorruptBufferChecks_ && !buffer.doCorruptBufferChecks()) {
        // warnings_.add("Exception caught when creating FEDBuffer object for FED",
        //               std::format("id {0}: FED corrupt buffer check fails for FED ID {0}.", *ifed));
        maskFED(detids, conns);
        continue;
      }

      // Check if EventSummary ("trigger FED info") needs updating
      if (first_fed && useDaqRegister_) {
        updateEventSummary(buffer, summary);
        first_fed = false;
      }

      // Check to see if EventSummary info is set
      // if (!quiet_ && !summary.isSet()) {
      //   warnings_.add(
      //       "EventSummary is not set correctly! Missing information from both \"trigger FED\" and \"DAQ registers\"!");
      // }

      // Check to see if event is to be analyzed according to EventSummary
      // if (!summary.valid()) {
      //   if (edm::isDebugEnabled()) {
      //     LogTrace("SiStripRawToDigi") << "[sistrip::RawToDigiUnpacker::" << __func__ << "]"
      //                                  << " EventSummary is not valid: skipping...";
      //   }
      //   continue;
      // }

      /// extract readout mode
      const sistrip::FEDReadoutMode mode = buffer.readoutMode();
      const sistrip::FEDLegacyReadoutMode lmode =
          (legacy_) ? buffer.legacyReadoutMode() : sistrip::READOUT_MODE_LEGACY_INVALID;

      // Retrive run type
      const sistrip::RunType runType_ = summary.runType();
      if (runType_ == sistrip::APV_LATENCY || runType_ == sistrip::FINE_DELAY) {
        useFedKey_ = false;
      }

      // Dump of FED buffer
      // if (edm::isDebugEnabled()) {
      //   if (fedEventDumpFreq_ && !(event_ % fedEventDumpFreq_)) {
      //     std::stringstream ss;
      //     buffer.dump(ss);
      //     edm::LogVerbatim(sistrip::mlRawToDigi_) << ss.str();
      //   }
      // }

      // Iterate through FED channels, extract payload and create Digis
      std::vector<FedChannelConnection>::const_iterator iconn = conns.begin();
      for (; iconn != conns.end(); iconn++) {
        /// FED channel
        uint16_t chan = iconn->fedCh();

        // Check if fed connection is valid
        if (!iconn->isConnected()) {
          continue;
        }

        // Check DetId is valid (if to be used as key)
        if (!useFedKey_ && (!iconn->detId() || iconn->detId() == sistrip::invalid32_)) {
          continue;
        }

        // Check FED channel
        if (!buffer.channelGood(iconn->fedCh(), doAPVEmulatorCheck_)) {
          if (!unpackBadChannels_ || !(buffer.fePresent(iconn->fedCh() / FEDCH_PER_FEUNIT) &&
                                       buffer.feEnabled(iconn->fedCh() / FEDCH_PER_FEUNIT))) {
            detids.push_back(iconn->detId());  //@@ Possible multiple entries (ok for Giovanni)
            continue;
          }
        }

        // Determine whether FED key is inferred from cabling or channel loop
        const uint32_t fed_key =
            (summary.runType() == sistrip::FED_CABLING)
                ? ((*ifed & sistrip::invalid_) << 16) | (chan & sistrip::invalid_)
                : ((iconn->fedId() & sistrip::invalid_) << 16) | (iconn->fedCh() & sistrip::invalid_);

        // Determine whether DetId or FED key should be used to index digi containers
        const uint32_t key = (useFedKey_ || (!legacy_ && mode == sistrip::READOUT_MODE_SCOPE) ||
                              (legacy_ && lmode == sistrip::READOUT_MODE_LEGACY_SCOPE))
                                 ? fed_key
                                 : iconn->detId();

        // Determine APV std::pair number (needed only when using DetId)
        const uint16_t ipair = (useFedKey_ || (!legacy_ && mode == sistrip::READOUT_MODE_SCOPE) ||
                                (legacy_ && lmode == sistrip::READOUT_MODE_LEGACY_SCOPE))
                                   ? 0
                                   : iconn->apvPairNumber();

        const auto& fedChannel = buffer.channel(iconn->fedCh());

        if (fedchannelunpacker::isZeroSuppressed(mode, legacy_, lmode)) {
          Registry regItem(key, 0, zs_work_digis.size(), 0);
          const auto isNonLite = fedchannelunpacker::isNonLiteZS(mode, legacy_, lmode);
          const uint8_t pCode = (isNonLite ? buffer.packetCode(legacy_, iconn->fedCh()) : 0);
          // if (isNonLite)
          //   LogDebug("SiStripRawToDigi") << "Non-lite zero-suppressed mode. Packet code=0x" << std::hex
          //                                << uint16_t(pCode) << std::dec;
          const auto st_ch = fedchannelunpacker::unpackZeroSuppressed(
              fedChannel, std::back_inserter(zs_work_digis), ipair * 256, isNonLite, mode, legacy_, lmode, pCode);
          if (fedchannelunpacker::StatusCode::ZERO_PACKET_CODE == st_ch ||
              fedchannelunpacker::StatusCode::BAD_PACKET_CODE == st_ch) {
            // warnings_.add(std::format("Invalid packet code {0:#x} for zero-suppressed data", uint16_t(pCode)),
            //               std::format("FED {0} channel {1}", *ifed, iconn->fedCh()));
          } else if (fedchannelunpacker::StatusCode::SUCCESS != st_ch) {
            // warnings_.add("Clusters are not ordered",
            //               std::format("FED {0} channel {1}: {2}", *ifed, iconn->fedCh(), toString(st_ch)));
            detids.push_back(iconn->detId());  //@@ Possible multiple entries (ok for Giovanni)
            continue;
          }
          if (regItem.index != zs_work_digis.size()) {
            regItem.length = zs_work_digis.size() - regItem.index;
            regItem.first = zs_work_digis[regItem.index].strip();
            zs_work_registry_.push_back(regItem);
          }
          // Common mode values
          if (isNonLite && extractCm_) {
            if ((pCode == PACKET_CODE_ZERO_SUPPRESSED) || (pCode == PACKET_CODE_ZERO_SUPPRESSED10) ||
                (pCode == PACKET_CODE_ZERO_SUPPRESSED8_BOTBOT) || (pCode == PACKET_CODE_ZERO_SUPPRESSED8_TOPBOT)) {
              Registry regItem2(key, 2 * ipair, cm_work_digis_.size(), 2);
              cm_work_digis_.push_back(SiStripRawDigi(fedChannel.cmMedian(0)));
              cm_work_digis_.push_back(SiStripRawDigi(fedChannel.cmMedian(1)));
              cm_work_registry_.push_back(regItem2);
            } else {
              detids.push_back(iconn->detId());  //@@ Possible multiple entries (ok for Giovanni)
              // warnings_.add("Problem extracting common modes",
              //               std::format("FED {0} channel {1}:\n Request for CM median from channel with non-ZS "
              //                           "packet code. Packet code is {2}.",
              //                           *ifed,
              //                           iconn->fedCh(),
              //                           pCode));
            }
          }
        } else {
          auto st_ch = fedchannelunpacker::StatusCode::SUCCESS;
          if (fedchannelunpacker::isVirginRaw(mode, legacy_, lmode)) {
            Registry regItem(key, 256 * ipair, virgin_work_digis_.size(), 0);
            // LogDebug("SiStripRawToDigi") << "Virgin raw packet code: 0x" << std::hex
            //                              << uint16_t(buffer.packetCode(legacy_)) << "  0x"
            //                              << uint16_t(fedChannel.packetCode()) << std::dec;
            st_ch = fedchannelunpacker::unpackVirginRaw(
                fedChannel, std::back_inserter(virgin_work_digis_), buffer.packetCode(legacy_));
            if (regItem.index != virgin_work_digis_.size()) {
              regItem.length = virgin_work_digis_.size() - regItem.index;
              virgin_work_registry_.push_back(regItem);
            }
          } else if (fedchannelunpacker::isProcessedRaw(mode, legacy_, lmode)) {
            Registry regItem(key, 256 * ipair, proc_work_digis_.size(), 0);
            st_ch = fedchannelunpacker::unpackProcessedRaw(fedChannel, std::back_inserter(proc_work_digis_));
            if (regItem.index != proc_work_digis_.size()) {
              regItem.length = proc_work_digis_.size() - regItem.index;
              proc_work_registry_.push_back(regItem);
            }
          } else if (fedchannelunpacker::isScopeMode(mode, legacy_, lmode)) {
            Registry regItem(key, 0, scope_work_digis_.size(), 0);
            st_ch = fedchannelunpacker::unpackScope(fedChannel, std::back_inserter(scope_work_digis_));
            if (regItem.index != scope_work_digis_.size()) {
              regItem.length = scope_work_digis_.size() - regItem.index;
              scope_work_registry_.push_back(regItem);
            }
          } else {  // Unknown readout mode! => assume scope mode
            // warnings_.add(std::format("Unknown FED readout mode ({0})! Assuming SCOPE MODE...", int(mode)));
            Registry regItem(key, 0, scope_work_digis_.size(), 0);
            st_ch = fedchannelunpacker::unpackScope(fedChannel, std::back_inserter(scope_work_digis_));
            if (regItem.index != scope_work_digis_.size()) {
              regItem.length = scope_work_digis_.size() - regItem.index;
              scope_work_registry_.push_back(regItem);
              // if (edm::isDebugEnabled()) {
              //   std::stringstream ss;
              //   ss << "Extracted " << regItem.length
              //      << " SCOPE MODE digis (samples[0] = " << scope_work_digis_[regItem.index] << ") from FED id/ch "
              //      << iconn->fedId() << "/" << iconn->fedCh();
              //   LogTrace("SiStripRawToDigi") << ss.str();
              // }
            } else {
              // warnings_.add("No SM digis found!");
            }
          }
          // if (fedchannelunpacker::StatusCode::SUCCESS != st_ch) {
          //   warnings_.add(toString(st_ch), std::format("FED {0} channel {1}:", *ifed, iconn->fedCh()));
          // }
        }
      }  // channel loop
    }  // fed loop

    // bad channels warning
    unsigned int detIdsSize = detids.size();
    // if (edm::isDebugEnabled() && detIdsSize) {
    //   std::ostringstream ss;
    //   ss << "[sistrip::RawToDigiUnpacker::" << __func__ << "]"
    //      << " Problems were found in data and " << detIdsSize << " channels could not be unpacked. "
    //      << "See output of FED Hardware monitoring for more information. ";
    //   edm::LogWarning(sistrip::mlRawToDigi_) << ss.str();
    // }
    // if ((errorThreshold_ != 0) && (detIdsSize > errorThreshold_)) {
    //   edm::LogError("TooManyErrors") << "Total number of errors = " << detIdsSize;
    // }

    // update DetSetVectors
    update(scope_mode, virgin_raw, proc_raw, zero_suppr, cm_values);

    // increment event counter
    event_++;

    // no longer first event!
    if (first_) {
      first_ = false;
    }

    // final cleanup, just in case
    cleanupWorkVectors();
  }
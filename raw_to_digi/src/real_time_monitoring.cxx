#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1D.h"
#include <TFile.h>

#if USE_ROOT7_RHIST
#include "ROOT/RHist.hxx"
#include "ROOT/Hist/ConvertToTH1.hxx"
#endif

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

int main(int argc, char* argv[]){
    if (argc != 5) {
        std::cerr << "3 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_root_file> <detector_info> <output_root_file> <n_treads>\n";
        return 1;
    }
    std::string input_root_file(argv[1]);
    std::string detector_info_path(argv[2]);
    std::string output_root_file(argv[3]);

    ROOT::EnableImplicitMT(std::stoi(argv[4]));

    auto extract_adc = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint16_t> adcs;
        adcs.reserve((digis.size()));
        for (const auto& digi : digis) {
            adcs.push_back(digi.GetSignal());
        }
        return adcs;
    };

    auto extract_strip = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint16_t> strips;
        strips.reserve((digis.size()));
        for (const auto& digi : digis) {
            strips.push_back(digi.GetStrip());
        }
        return strips;
    };

    auto extract_fed_key = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint32_t> fed_keys;
        fed_keys.reserve((digis.size()));
        for (const auto& digi : digis) {
            fed_keys.push_back(digi.GetFedKey());
        }
        return fed_keys;
    };

    auto extract_layer = [](const std::vector<SiStripDigi>& digis) {
        std::vector<int> layers;
        layers.reserve((digis.size()));
        for (const auto& digi : digis) {
            layers.push_back(digi.GetLayer());
        }
        return layers;
    };

    auto extract_row = [](const std::vector<SiStripDigi>& digis) {
        std::vector<int> rows;
        rows.reserve((digis.size()));
        for (const auto& digi : digis) {
            rows.push_back(digi.GetRow());
        }
        return rows;
    };

    auto extract_column = [](const std::vector<SiStripDigi>& digis) {
        std::vector<int> columns;
        columns.reserve((digis.size()));
        for (const auto& digi : digis) {
            columns.push_back(digi.GetColumn());
        }
        return columns;
    };

    auto extract_detector_id = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint32_t> detector_ids;
        detector_ids.reserve((digis.size()));
        for (const auto& digi : digis) {
            detector_ids.push_back(digi.GetDetectorId());
        }
        return detector_ids;
    };

    auto df = ROOT::RDataFrame("Events", input_root_file);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(detector_info_path), {"FEDRawDataCollection_rawDataCollector__LHC."});
    auto df3 = df2.Define("ADC", extract_adc, {"FedChannelDigis"});
    auto df4 = df3.Define("strip", extract_strip, {"FedChannelDigis"});
    auto df5 = df4.Define("fed_key", extract_fed_key, {"FedChannelDigis"});
    auto df6 = df5.Define("layer", extract_layer, {"FedChannelDigis"});
    auto df7 = df6.Define("row", extract_row, {"FedChannelDigis"});
    auto df8 = df7.Define("column", extract_column, {"FedChannelDigis"});
    auto df9 = df8.Define("detector_id", extract_detector_id, {"FedChannelDigis"});

    //If RHist is available
    #if USE_ROOT7_RHIST
    auto rhist_adc = df5.Hist(512, {0, 512}, "ADC");
    auto rhist_strip = df5.Hist(512, {0, 512}, "strip");
    auto rhist_fed_key = df5.Hist(512, {0, 512}, "fed_key");
    auto histo_adc = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_adc);
    auto histo_strip = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_strip);
    auto histo_fed_key = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_fed_key);
    // If RHist is not available
    #else
    auto histo_adc = df9.Histo1D<std::vector<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "ADC");
    auto histo_strip = df9.Histo1D<std::vector<uint16_t>>({"SiStrip strip", "SiStrip strip", 512, 0, 512}, "strip");
    auto histo_fed_key = df9.Histo1D<std::vector<uint32_t>>({"SiStrip fed_key", "SiStrip fed_key", 100, 7602100, 7602200}, "fed_key");
    auto histo_layer = df9.Histo1D<std::vector<int>>({"SiStrip layer", "SiStrip layer", 20, 0, 20}, "layer");
    auto histo_row = df9.Histo1D<std::vector<int>>({"SiStrip row", "SiStrip row", 4, 0, 4}, "row");
    auto histo_column = df9.Histo1D<std::vector<int>>({"SiStrip column", "SiStrip column", 2, 0, 2}, "column");
    auto histo_detector_id = df9.Histo1D<std::vector<uint32_t>>({"SiStrip detector_id", "SiStrip detector_id", 10000, 0, 180000}, "detector_id");
    #endif

    TFile output_file(output_root_file.c_str(), "RECREATE");

    histo_adc->Write();
    histo_strip->Write();
    histo_fed_key->Write();
    histo_layer->Write();
    histo_row->Write();
    histo_column->Write();
    histo_detector_id->Write();

    output_file.Close();
}

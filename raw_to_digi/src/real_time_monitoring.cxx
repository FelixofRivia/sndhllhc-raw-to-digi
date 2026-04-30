#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include "ROOT/RDataFrame.hxx"
#include "TCanvas.h"
#include "TH1D.h"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

int main(int argc, char* argv[]){
    if (argc != 4) {
        std::cerr << "3 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_path> <output_directory> <n_treads>\n";
        return 1;
    }
    std::string input_path(argv[1]);
    std::string output_directory(argv[2]);

    ROOT::EnableImplicitMT(std::stoi(argv[3]));

    auto extract_adc = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint16_t> adcs;
        adcs.reserve((digis.size()));
        for (const auto& digi : digis) {
            adcs.push_back(digi.adc());
        }
        return adcs;
    };

    auto extract_strip = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint16_t> strips;
        strips.reserve((digis.size()));
        for (const auto& digi : digis) {
            strips.push_back(digi.strip());
        }
        return strips;
    };

    auto extract_id = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint32_t> ids;
        ids.reserve((digis.size()));
        for (const auto& digi : digis) {
            ids.push_back(digi.id());
        }
        return ids;
    };

    auto df = ROOT::RDataFrame("Events", input_path);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(), {"FEDRawDataCollection_rawDataCollector__LHC."});
    auto df3 = df2.Define("ADC", extract_adc, {"FedChannelDigis"});
    auto df4 = df3.Define("strip", extract_strip, {"FedChannelDigis"});
    auto df5 = df4.Define("id", extract_id, {"FedChannelDigis"});
    auto histo_adc = df5.Histo1D<std::vector<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "ADC");
    auto histo_strip = df5.Histo1D<std::vector<uint16_t>>({"SiStrip strip", "SiStrip strip", 512, 0, 512}, "strip");
    auto histo_id = df5.Histo1D<std::vector<uint32_t>>({"SiStrip id", "SiStrip id", 512, 0, 512}, "id");

    TCanvas canvas_adc("canvas adc", "SiStrip ADC Histogram", 800, 600);
    histo_adc->Draw();
    canvas_adc.SaveAs((output_directory + std::string("/sistrip_adc.png")).c_str());

    TCanvas canvas_strip("canvas strip", "SiStrip strip Histogram", 800, 600);
    histo_strip->Draw();
    canvas_strip.SaveAs((output_directory + std::string("/sistrip_strip.png")).c_str());

    TCanvas canvas_id("canvas id", "SiStrip id Histogram", 800, 600);
    histo_id->Draw();
    canvas_id.SaveAs((output_directory + std::string("/sistrip_id.png")).c_str());
}

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
    if (argc != 3) {
        std::cerr << "2 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_path> <output_path>\n";
        return 1;
    }
    std::string input_path(argv[1]);
    std::string output_path(argv[2]);

    auto df = ROOT::RDataFrame("Events", input_path);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(), {"FEDRawDataCollection_rawDataCollector__LHC."});

    auto extract_adc = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint16_t> adcs;
        adcs.reserve((digis.size()));
        for (const auto& digi : digis) {
            adcs.push_back(digi.adc());
        }
        return adcs;
    };

    auto df3 = df2.Define("ADC", extract_adc, {"FedChannelDigis"});
    auto histo_adc = df3.Histo1D<std::vector<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "ADC");

    TCanvas canvas("canvas", "SiStrip ADC Histogram", 800, 600);
    histo_adc->Draw();
    canvas.SaveAs(output_path.c_str());
}

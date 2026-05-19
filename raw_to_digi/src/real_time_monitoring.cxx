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
    if (argc != 4) {
        std::cerr << "3 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_root_file> <output_root_file> <n_treads>\n";
        return 1;
    }
    std::string input_root_file(argv[1]);
    std::string output_root_file(argv[2]);

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

    auto extract_fed_key = [](const std::vector<SiStripDigi>& digis) {
        std::vector<uint32_t> fed_keys;
        fed_keys.reserve((digis.size()));
        for (const auto& digi : digis) {
            fed_keys.push_back(digi.fed_key());
        }
        return fed_keys;
    };

    auto df = ROOT::RDataFrame("Events", input_root_file);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(), {"FEDRawDataCollection_rawDataCollector__LHC."});
    auto df3 = df2.Define("ADC", extract_adc, {"FedChannelDigis"});
    auto df4 = df3.Define("strip", extract_strip, {"FedChannelDigis"});
    auto df5 = df4.Define("fed_key", extract_fed_key, {"FedChannelDigis"});

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
    auto histo_adc = df5.Histo1D<std::vector<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "ADC");
    auto histo_strip = df5.Histo1D<std::vector<uint16_t>>({"SiStrip strip", "SiStrip strip", 512, 0, 512}, "strip");
    auto histo_fed_key = df5.Histo1D<std::vector<uint32_t>>({"SiStrip fed_key", "SiStrip fed_key", 100, 7602100, 7602200}, "fed_key");
    #endif

    TFile output_file(output_root_file.c_str(), "RECREATE");

    histo_adc->Write();
    histo_strip->Write();
    histo_fed_key->Write();

    output_file.Close();
}

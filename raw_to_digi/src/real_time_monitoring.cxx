#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include "ROOT/RDataFrame.hxx"
#include <ROOT/RVec.hxx>
#include "TCanvas.h"
#include "TH1D.h"
#include <TFile.h>

#if USE_ROOT7_RHIST
#include "ROOT/RHist.hxx"
#include "ROOT/Hist/ConvertToTH1.hxx"
#endif

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"
#include "SiStripDetInfo.h"

template <typename Ret, typename Method>
auto ExtractRVec(Method method)
{
    return [method](const std::vector<SiStripDigi>& digis) {
        ROOT::RVec<Ret> out(digis.size());
        for (size_t i = 0; i < digis.size(); ++i)
            out[i] = (digis[i].*method)();
        return out;
    };
}

int main(int argc, char* argv[]){
    if (argc != 5) {
        std::cerr << "4 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_root_file> <detector_info> <output_root_file> <n_treads>\n";
        return 1;
    }
    std::string input_root_file(argv[1]);
    std::string detector_info_path(argv[2]);
    std::string output_root_file(argv[3]);

    ROOT::EnableImplicitMT(std::stoi(argv[4]));

    auto df = ROOT::RDataFrame("Events", input_root_file);
    // Perform digitization
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(detector_info_path), {"FEDRawDataCollection_rawDataCollector__LHC."});
    // Prepare columns for histos
    auto df3 = df2.Define("adc", ExtractRVec<uint16_t>(&SiStripDigi::GetSignal), {"FedChannelDigis"})
        .Define( "strip", ExtractRVec<uint16_t>(&SiStripDigi::GetStrip), {"FedChannelDigis"})
        .Define( "fed_key", ExtractRVec<uint32_t>(&SiStripDigi::GetFedKey), {"FedChannelDigis"})
        .Define( "layer", ExtractRVec<int>(&SiStripDigi::GetLayer), {"FedChannelDigis"})
        .Define( "row", ExtractRVec<int>(&SiStripDigi::GetRow), {"FedChannelDigis"})
        .Define( "column", ExtractRVec<int>(&SiStripDigi::GetColumn), {"FedChannelDigis"})
        .Define( "detector_id", ExtractRVec<uint32_t>(&SiStripDigi::GetDetectorId), {"FedChannelDigis"});

    // //If RHist is available
    // #if USE_ROOT7_RHIST
    // auto rhist_adc = df3.Hist(512, {0, 512}, "ADC");
    // auto rhist_strip = df3.Hist(512, {0, 512}, "strip");
    // auto rhist_fed_key = df3.Hist(512, {0, 512}, "fed_key");
    // auto histo_adc = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_adc);
    // auto histo_strip = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_strip);
    // auto histo_fed_key = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_fed_key);
    // // If RHist is not available
    // #else
    // auto histo_adc = df3.Histo1D<ROOT::RVec<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "adc");
    // auto histo_strip = df3.Histo1D<ROOT::RVec<uint16_t>>({"SiStrip strip", "SiStrip strip", 756, 0, 756}, "strip");
    // auto histo_fed_key = df3.Histo1D<ROOT::RVec<uint32_t>>({"SiStrip fed_key", "SiStrip fed_key", 100, 7602100, 7602200}, "fed_key");
    // #endif

    // Retrieve max detinfo values to generate histograms dynamically
    std::vector<DetectorInfo> detinfo = GetDetectorInfo(detector_info_path);
    const int max_layer = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.layer < y.layer;})->layer;
    const int max_row = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.row < y.row;})->row;
    const int max_column = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.column < y.column;})->column;

    std::vector<ROOT::RDF::RResultPtr<TH1D>> histos_1d;

    for (int layer{0}; layer <= max_layer; ++layer) {
        for (int row{0}; row <= max_row; ++row) {
            for (int col{0}; col <= max_column; ++col) {

                std::string h_adc_name = Form("adc Layer %d Row %d Column %d", layer, row, col);
                std::string h_strip_name = Form("strip Layer %d Row %d Column %d", layer, row, col);

                std::string select_adc = Form("adc[(layer == %d) && (row == %d) && (column == %d)]", layer, row, col);
                std::string select_strip = Form("strip[(layer == %d) && (row == %d) && (column == %d)]", layer, row, col);

                auto df_module = df3.Define("adc_module", select_adc).Define("sistrip_module", select_strip);

                histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint16_t>>({h_adc_name.c_str(), (h_adc_name + std::string(";adc;Entries")).c_str(), 512, 0, 512}, "adc_module"));
                histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint16_t>>({h_strip_name.c_str(), (h_strip_name + std::string(";strip;Entries")).c_str(), 756, 0, 756}, "sistrip_module"));
            }
        }
    }

    // auto histo_adc = df3.Histo1D<ROOT::RVec<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "adc");
    // auto histo_strip = df3.Histo1D<ROOT::RVec<uint16_t>>({"SiStrip strip", "SiStrip strip", 756, 0, 756}, "strip");
    // auto histo_fed_key = df3.Histo1D<ROOT::RVec<uint32_t>>({"SiStrip fed_key", "SiStrip fed_key", 100, 7602100, 7602200}, "fed_key");
    // auto histo_layer = df3.Histo1D<ROOT::RVec<int>>({"SiStrip layer", "SiStrip layer", 20, 0, 20}, "layer");
    // auto histo_row = df3.Histo1D<ROOT::RVec<int>>({"SiStrip row", "SiStrip row", 4, 0, 4}, "row");
    // auto histo_column = df3.Histo1D<ROOT::RVec<int>>({"SiStrip column", "SiStrip column", 2, 0, 2}, "column");
    // auto histo_detector_id = df3.Histo1D<ROOT::RVec<uint32_t>>({"SiStrip detector_id", "SiStrip detector_id", 10000, 0, 180000}, "detector_id");
    // auto histo_row_vs_column = df3.Histo2D<ROOT::RVec<int>, ROOT::RVec<int>>({"SiStrip row vs column", "SiStrip row vs column;column;row", 2, 0, 2, 4, 0, 4}, "column", "row");

    TFile output_file(output_root_file.c_str(), "RECREATE");

    for (auto& h : histos_1d) {
        h->Write();
    }

    // histo_adc->Write();
    // histo_strip->Write();
    // histo_fed_key->Write();
    // histo_layer->Write();
    // histo_row->Write();
    // histo_column->Write();
    // histo_detector_id->Write();
    // histo_row_vs_column->Write();

    output_file.Close();
}

#include <iostream>
#include <vector>
#include <map>
#include <set>
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
        std::cerr << "Usage: real_time_monitoring <input_root_file> <detector_info> <output_histos_root_file> <n_treads>\n";
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
    // auto rhist_adc = df3.Hist(512, {0, 512}, "adc");
    // auto histo_adc = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_adc);
    // // If RHist is not available
    // #else
    // auto histo_adc = df3.Histo1D<ROOT::RVec<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "adc");
    // #endif

    // Retrieve detinfo values to generate histograms dynamically
    std::vector<DetectorInfo> detinfo = GetDetectorInfo(detector_info_path);
    const int max_row = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.row < y.row;})->row;
    const int max_column = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.column < y.column;})->column;

    std::map<int, std::set<std::pair<int,int>>> layer_map;
    for (const auto& d : detinfo) {
        layer_map[d.layer].insert({d.row, d.column});
    }

    std::vector<ROOT::RDF::RResultPtr<TH1D>> histos_1d;
    std::vector<ROOT::RDF::RResultPtr<TH2D>> histos_2d;

    const int max_layer = layer_map.rbegin()->first;

    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<int>>({"layer", "layer;layer;Entries", max_layer + 1, 0, static_cast<double>(max_layer + 1)}, "layer"));
    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<uint32_t>>({"det_id (advsndsw)", "det_id (advsndsw);det_id;Entries", 18000, 0, 180000}, "detector_id"));

    for (const auto& [layer, modules] : layer_map) {

        std::string h_row_vs_column_name = Form("row vs column Layer %d", layer);

        std::string select_row = Form("row[(layer == %d)]", layer);
        std::string select_column = Form("column[(layer == %d)]", layer);

        auto df_layer = df3.Define("row_layer", select_row).Define("column_layer", select_column);

        histos_2d.emplace_back(df_layer.Histo2D<ROOT::RVec<int>, ROOT::RVec<int>>({h_row_vs_column_name.c_str(), (h_row_vs_column_name + std::string(";column;row;Entries")).c_str(), max_column + 1, 0, static_cast<double>(max_column + 1), max_row + 1, 0, static_cast<double>(max_row + 1)}, "column_layer", "row_layer"));

        for (const auto& [row, col] : modules) {

            std::string h_adc_name = Form("adc Layer %d Row %d Column %d", layer, row, col);
            std::string h_strip_name = Form("strip Layer %d Row %d Column %d", layer, row, col);
            std::string h_nhits_name = Form("nhits Layer %d Row %d Column %d", layer, row, col);
            std::string h_adc_vs_strip_name = Form("adc vs strip Layer %d Row %d Column %d", layer, row, col);

            std::string select_adc = Form("adc[(layer == %d) && (row == %d) && (column == %d)]", layer, row, col);
            std::string select_strip = Form("strip[(layer == %d) && (row == %d) && (column == %d)]", layer, row, col);

            auto df_module = df3.Define("adc_module", select_adc).Define("sistrip_module", select_strip).Define("nhits_module", "adc_module.size()");

            histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint16_t>>({h_adc_name.c_str(), (h_adc_name + std::string(";adc;Entries")).c_str(), 512, 0, 512}, "adc_module"));
            histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint16_t>>({h_strip_name.c_str(), (h_strip_name + std::string(";strip;Entries")).c_str(), 756, 0, 756}, "sistrip_module"));
            histos_1d.emplace_back(df_module.Histo1D<std::size_t>({h_nhits_name.c_str(), (h_nhits_name + std::string(";nhits;Entries")).c_str(), 756, 0, 756}, "nhits_module"));
            histos_2d.emplace_back(df_module.Histo2D<ROOT::RVec<uint16_t>, ROOT::RVec<uint16_t>>({h_adc_vs_strip_name.c_str(), (h_adc_vs_strip_name + std::string(";strip;adc;Entries")).c_str(), 756, 0, 756, 512, 0, 512}, "sistrip_module", "adc_module"));
        }
    }

    TFile output_file(output_root_file.c_str(), "RECREATE");

    for (auto& h : histos_1d) {
        h->Write();
    }

    for (auto& h : histos_2d) {
        h->Write();
    }

    output_file.Close();
}

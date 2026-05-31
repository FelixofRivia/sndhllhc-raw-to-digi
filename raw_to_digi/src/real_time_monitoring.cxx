#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <string>
#include <cstdint>
#include <numeric>
#include "ROOT/RDataFrame.hxx"
#include <ROOT/RVec.hxx>
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"

#if USE_ROOT7_RHIST
#include "ROOT/RHist.hxx"
#include "ROOT/Hist/ConvertToTH1.hxx"
#endif

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"
#include "SiStripDigiClustering.h"
#include "SiStripDigiFilter.h"
#include "SiStripDetInfo.h"
#include "SiStripPosition.h"

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
    if (argc != 6) {
        std::cerr << "5 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: real_time_monitoring <input_root_file> <detector_info> <geometry_file> <output_histos_root_file> <n_treads>\n";
        return 1;
    }
    std::string input_root_file(argv[1]);
    std::string detector_info_path(argv[2]);
    std::string geometry_file(argv[3]);
    std::string output_root_file(argv[4]);

    ROOT::EnableImplicitMT(std::stoi(argv[5]));
    std::cout << "Requested threads: " << std::stoi(argv[5]) << '\n';
    std::cout << "ROOT pool size:    " << ROOT::GetThreadPoolSize() << '\n';

    TGeoManager::Import(geometry_file.c_str());
    gGeoManager->SetMaxThreads(ROOT::GetThreadPoolSize());

    auto df = ROOT::RDataFrame("Events", input_root_file);
    // Perform digitization + clustering
    auto df2 = df.Define("FedChannelDigis_not_clustered", SiStripRawToDigi(detector_info_path), {"FEDRawDataCollection_rawDataCollector__LHC."})
        .Define("FedChannelDigis", SiStripDigiFilter(), {"FedChannelDigis_not_clustered"})
        .Define("Cluster", SiStripDigiClustering(), {"FedChannelDigis_not_clustered"});
    // Prepare columns for histos
    auto df3 = df2.Define("adc", ExtractRVec<uint16_t>(&SiStripDigi::GetSignal), {"FedChannelDigis"})
        .Define( "strip", ExtractRVec<uint16_t>(&SiStripDigi::GetStrip), {"FedChannelDigis"})
        .Define( "fed_key", ExtractRVec<uint32_t>(&SiStripDigi::GetFedKey), {"FedChannelDigis"})
        .Define( "layer", ExtractRVec<int>(&SiStripDigi::GetLayer), {"FedChannelDigis"})
        .Define( "row", ExtractRVec<int>(&SiStripDigi::GetRow), {"FedChannelDigis"})
        .Define( "column", ExtractRVec<int>(&SiStripDigi::GetColumn), {"FedChannelDigis"})
        .Define( "detector_id", ExtractRVec<uint32_t>(&SiStripDigi::GetDetectorId), {"FedChannelDigis"})
        .Define( "is_vertical", ExtractRVec<bool>(&SiStripDigi::IsVertical), {"FedChannelDigis"})
        .Define( "position", [](const ROOT::VecOps::RVec<uint32_t>& detids) {return ROOT::VecOps::Map(detids, GetSiStripPosition);}, {"detector_id"})
        .Define( "x", [](const ROOT::VecOps::RVec<ROOT::Math::XYZPoint>& points) {return ROOT::VecOps::Map(points, [](const auto& p) { return p.X(); });}, {"position"})
        .Define( "y", [](const ROOT::VecOps::RVec<ROOT::Math::XYZPoint>& points) {return ROOT::VecOps::Map(points, [](const auto& p) { return p.Y(); });}, {"position"})
        .Define( "z", [](const ROOT::VecOps::RVec<ROOT::Math::XYZPoint>& points) {return ROOT::VecOps::Map(points, [](const auto& p) { return p.Z(); });}, {"position"})
        .Define( "x_vertical", "x[is_vertical]")
        .Define( "y_not_vertical", "y[is_vertical == false]")
        .Define( "z_vertical", "z[is_vertical]")
        .Define( "z_not_vertical", "z[is_vertical == false]")
        .Define("cluster_adc",
            [](const std::vector<SiStripCluster>& c) {
                ROOT::RVec<uint32_t> out;
                out.reserve(c.size());

                for (const auto& cl : c)
                    out.push_back(cl.adc_);

                return out;
            },
            {"Cluster"})
        .Define("cluster_size",
            [](const std::vector<SiStripCluster>& c) {
                ROOT::RVec<size_t> out;
                out.reserve(c.size());

                for (const auto& cl : c)
                    out.push_back(cl.size_);

                return out;
            },
            {"Cluster"})
        .Define("cluster_layer",
            [](const std::vector<SiStripCluster>& c) {
                ROOT::RVec<int> out;
                out.reserve(c.size());

                for (const auto& cl : c)
                    out.push_back(cl.layer_);

                return out;
            },
            {"Cluster"})
        .Define("cluster_row",
            [](const std::vector<SiStripCluster>& c) {
                ROOT::RVec<int> out;
                out.reserve(c.size());

                for (const auto& cl : c)
                    out.push_back(cl.row_);

                return out;
            },
            {"Cluster"})
        .Define("cluster_column",
            [](const std::vector<SiStripCluster>& c) {
                ROOT::RVec<int> out;
                out.reserve(c.size());

                for (const auto& cl : c)
                    out.push_back(cl.column_);

                return out;
            },
            {"Cluster"});

    // //If RHist is available
    // #if USE_ROOT7_RHIST
    // auto rhist_adc = df3.Hist(512, {0, 512}, "adc");
    // auto histo_adc = ROOT::Experimental::Hist::ConvertToTH1D(*rhist_adc);
    // // If RHist is not available
    // #else
    // auto histo_adc = df3.Histo1D<ROOT::RVec<uint16_t>>({"SiStrip adc", "SiStrip adc", 512, 0, 512}, "adc");
    // #endif

    // Retrieve detinfo values to generate histograms dynamically
    const std::vector<DetectorInfo> detinfo = GetDetectorInfo(detector_info_path);
    const int max_row = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.row < y.row;})->row;
    const int max_column = std::max_element(detinfo.begin(), detinfo.end(), [](const DetectorInfo& x, const DetectorInfo& y) {return x.column < y.column;})->column;

    std::map<int, std::set<std::pair<int,int>>> layer_map;
    for (const auto& d : detinfo) {
        layer_map[d.layer].insert({d.row, d.column});
    }

    std::vector<ROOT::RDF::RResultPtr<TH1D>> histos_1d;
    std::vector<ROOT::RDF::RResultPtr<TH2D>> histos_2d;
    std::vector<ROOT::RDF::RResultPtr<TProfile>> profiles;

    const int max_layer = layer_map.rbegin()->first;
    constexpr double x_min = -30.0;
    constexpr double x_max = 0.0;
    constexpr double y_min = 15.0;
    constexpr double y_max = 45.0;
    constexpr double z_min = 165.0;
    constexpr double z_max = 195.0;

    auto df4 = df3.Define("layer_vec", [max_layer]() {
            ROOT::RVec<int> l_vec(max_layer + 1);
            std::iota(l_vec.begin(), l_vec.end(), 0);
            return l_vec;
        }).Define("saturated_percentage_vec", [max_layer](const ROOT::RVec<uint16_t>& adc, const ROOT::RVec<int>& layer) {
            ROOT::RVec<float> sat_vec(max_layer + 1);
            for (int i{0}; i < max_layer + 1; ++i) {
                const auto& adc_layer = adc[layer == i];
                sat_vec[i] = adc_layer.empty() ? 0.f : 100.f * adc_layer[adc_layer > 253].size() / adc_layer.size();
            }
            return sat_vec;
        }, {"adc", "layer"})
        .Define("adc_vec", [max_layer](const ROOT::RVec<uint16_t>& adc, const ROOT::RVec<int>& layer) {
            ROOT::RVec<uint32_t> adc_vec(max_layer + 1, 0);
            for (int i{0}; i < max_layer + 1; ++i) {
                adc_vec[i] = ROOT::VecOps::Sum(adc[layer == i]);
            }
            return adc_vec;
        }, {"adc", "layer"});


    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<int>>({"layer", "layer;layer;Entries", max_layer + 1, 0, static_cast<double>(max_layer + 1)}, "layer"));
    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<uint32_t>>({"det_id (advsndsw)", "det_id (advsndsw);det_id;Entries", 18000, 0, 180000}, "detector_id"));
    histos_2d.emplace_back(df4.Histo2D<ROOT::RVec<int>, ROOT::RVec<float>>({"saturated percentage (adc > 253) vs layer", "saturated percentage (adc > 253) vs layer;layer;saturated %", max_layer + 1, 0, static_cast<double>(max_layer + 1), 101, 0, 101}, "layer_vec", "saturated_percentage_vec"));
    histos_2d.emplace_back(df4.Histo2D<ROOT::RVec<int>, ROOT::RVec<uint32_t>>({"adc sum vs layer", "adc sum vs layer;layer;adc sum", max_layer + 1, 0, static_cast<double>(max_layer + 1), 1000, 0, 30000}, "layer_vec", "adc_vec"));
    histos_2d.emplace_back(df4.Histo2D<ROOT::RVec<double>, ROOT::RVec<double>>({"x vs z", "x vs z;z [cm];x [cm]", 1000, z_min, z_max, 1000, x_min, x_max}, "z_vertical", "x_vertical"));

    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<double>>({"x", "x;x [cm];Entries", 1000, x_min, x_max}, "x_vertical"));
    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<double>>({"y", "y;y [cm];Entries", 1000, y_min, y_max}, "y_not_vertical"));
    histos_1d.emplace_back(df3.Histo1D<ROOT::RVec<double>>({"z", "z;z [cm];Entries", 1000, z_min, z_max}, "z"));
    histos_2d.emplace_back(df4.Histo2D<ROOT::RVec<double>, ROOT::RVec<double>>({"x vs z", "x vs z;z [cm];x [cm]", 1000, z_min, z_max, 1000, x_min, x_max}, "z_vertical", "x_vertical"));
    histos_2d.emplace_back(df4.Histo2D<ROOT::RVec<double>, ROOT::RVec<double>>({"y vs z", "y vs z;z [cm];y [cm]", 1000, z_min, z_max, 1000, y_min, y_max}, "z_not_vertical", "y_not_vertical"));

    ROOT::RDF::TProfile1DModel saturation_model(
        "saturated percentage (adc > 253) vs layer (profiled)",
        "saturated percentage (adc > 253) vs layer;layer;saturated %",
        max_layer + 1, 0, max_layer + 1
    );
    profiles.emplace_back(df4.Profile1D<float>(saturation_model, "layer_vec", "saturated_percentage_vec"));

    ROOT::RDF::TProfile1DModel adc_model(
        "adc sum vs layer (profiled)",
        "adc sum vs layer;layer;adc sum",
        max_layer + 1, 0, max_layer + 1
    );
    profiles.emplace_back(df4.Profile1D<float>(adc_model, "layer_vec", "adc_vec"));

    for (const auto& [layer, modules] : layer_map) {

        std::string h_row_vs_column_name = Form("row vs column Layer %d", layer);
        std::string h_x_name = Form("x Layer %d", layer);
        std::string h_y_name = Form("y Layer %d", layer);

        std::string select_row = Form("row[(layer == %d)]", layer);
        std::string select_column = Form("column[(layer == %d)]", layer);
        std::string select_x = Form("x[(layer == %d) && is_vertical]", layer);
        std::string select_y = Form("y[(layer == %d) && (is_vertical == false)]", layer);

        if (layer % 2 == 0) {
            auto df_layer = df3.Define("row_layer", select_row).Define("column_layer", select_column).Define("x_vertical_layer", select_x);
            histos_1d.emplace_back(df_layer.Histo1D<ROOT::RVec<double>>({h_x_name.c_str(), (h_x_name + std::string(";x [cm];Entries")).c_str(), 1000, x_min, x_max}, "x_vertical_layer"));
            histos_2d.emplace_back(df_layer.Histo2D<ROOT::RVec<int>, ROOT::RVec<int>>({h_row_vs_column_name.c_str(), (h_row_vs_column_name + std::string(";column;row;Entries")).c_str(), max_column + 1, 0, static_cast<double>(max_column + 1), max_row + 1, 0, static_cast<double>(max_row + 1)}, "column_layer", "row_layer"));

        }
        else {
            auto df_layer = df3.Define("row_layer", select_row).Define("column_layer", select_column).Define("y_not_vertical_layer", select_y);
            histos_1d.emplace_back(df_layer.Histo1D<ROOT::RVec<double>>({h_y_name.c_str(), (h_y_name + std::string(";y [cm];Entries")).c_str(), 1000, y_min, y_max}, "y_not_vertical_layer"));
            histos_2d.emplace_back(df_layer.Histo2D<ROOT::RVec<int>, ROOT::RVec<int>>({h_row_vs_column_name.c_str(), (h_row_vs_column_name + std::string(";column;row;Entries")).c_str(), max_column + 1, 0, static_cast<double>(max_column + 1), max_row + 1, 0, static_cast<double>(max_row + 1)}, "column_layer", "row_layer"));
        }
       
        for (const auto& [row, col] : modules) {

            std::string h_adc_name = Form("adc Layer %d Row %d Column %d", layer, row, col);
            std::string h_strip_name = Form("strip Layer %d Row %d Column %d", layer, row, col);
            std::string h_nhits_name = Form("nhits Layer %d Row %d Column %d", layer, row, col);
            std::string h_saturated_percentage_name = Form("saturated percentage (adc > 253) Layer %d Row %d Column %d", layer, row, col);
            std::string h_adc_vs_strip_name = Form("adc vs strip Layer %d Row %d Column %d", layer, row, col);
            std::string h_cluster_adc_name = Form("cluster adc Layer %d Row %d Column %d", layer, row, col);
            std::string h_cluster_size_name = Form("cluster size Layer %d Row %d Column %d", layer, row, col);
            std::string h_cluster_adc_vs_size_name = Form("cluster adc vs size Layer %d Row %d Column %d", layer, row, col);

            std::string select_adc = Form("adc[(layer == %d) && (row == %d) && (column == %d)]", layer, row, col);
            std::string select_strip = Form("strip[(layer == %d) && (row == %d) && (column == %d)]", layer, row, col);
            std::string compute_saturated_percentage = Form("adc_module.empty() ? 0.f : 100.f * adc_module[adc_module > 253].size() / adc_module.size()");
            std::string select_cluster_adc = Form("cluster_adc[(cluster_layer == %d) && (cluster_row == %d) && (cluster_column == %d)]", layer, row, col);
            std::string select_cluster_size = Form("cluster_size[(cluster_layer == %d) && (cluster_row == %d) && (cluster_column == %d)]", layer, row, col);

            auto df_module = df3.Define("adc_module", select_adc).Define("sistrip_module", select_strip).Define("nhits_module", "adc_module.size()").Define("saturated_percentage_module", compute_saturated_percentage)
                .Define("cluster_adc_module", select_cluster_adc).Define("cluster_size_module", select_cluster_size);

            histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint16_t>>({h_adc_name.c_str(), (h_adc_name + std::string(";adc;Entries")).c_str(), 256, 0, 256}, "adc_module"));
            histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint16_t>>({h_strip_name.c_str(), (h_strip_name + std::string(";strip;Entries")).c_str(), 756, 0, 756}, "sistrip_module"));
            histos_1d.emplace_back(df_module.Histo1D<std::size_t>({h_nhits_name.c_str(), (h_nhits_name + std::string(";nhits;Entries")).c_str(), 756, 0, 756}, "nhits_module"));
            histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<uint32_t>>({h_cluster_adc_name.c_str(), (h_cluster_adc_name + std::string(";adc;Entries")).c_str(), 3000, 0, 3000}, "cluster_adc_module"));
            histos_1d.emplace_back(df_module.Histo1D<ROOT::RVec<size_t>>({h_cluster_size_name.c_str(), (h_cluster_size_name + std::string(";size;Entries")).c_str(), 100, 0, 100}, "cluster_size_module"));
            histos_1d.emplace_back(df_module.Histo1D<float>({h_saturated_percentage_name.c_str(), (h_saturated_percentage_name + std::string(";saturated %;Entries")).c_str(), 101, 0, 101}, "saturated_percentage_module"));
            histos_2d.emplace_back(df_module.Histo2D<ROOT::RVec<uint16_t>, ROOT::RVec<uint16_t>>({h_adc_vs_strip_name.c_str(), (h_adc_vs_strip_name + std::string(";strip;adc;Entries")).c_str(), 756, 0, 756, 256, 0, 256}, "sistrip_module", "adc_module"));
            histos_2d.emplace_back(df_module.Histo2D<ROOT::RVec<size_t>, ROOT::RVec<uint32_t>>({h_cluster_adc_vs_size_name.c_str(), (h_cluster_adc_vs_size_name + std::string(";size;adc;Entries")).c_str(), 100, 0, 100, 3000, 0, 3000}, "cluster_size_module", "cluster_adc_module"));
        }
    }

    TFile output_file(output_root_file.c_str(), "RECREATE");

    for (auto& h : histos_1d) {
        h->SetFillColor(kAzure - 9);
        h->Write();
    }

    for (auto& h : histos_2d) {
        h->Write();
    }

    for (auto& p : profiles) {
        p->Write();
    }

    output_file.Close();
}

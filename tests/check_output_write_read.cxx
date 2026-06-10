#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <filesystem>
#include "ROOT/RDataFrame.hxx"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

int main(int argc, char* argv[]){
    if (argc !=4) {
        std::cerr << "3 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_root_file> <detector_info> <output_folder>\n";
        return 1;
    }
    std::string input_path(argv[1]);
    std::string detector_info_path(argv[2]);
    std::filesystem::path output_folder(argv[3]);

    std::filesystem::create_directories(output_folder);

    std::filesystem::path output_ttree_path = output_folder / "digi_ttree.root";
    std::filesystem::path output_rntuple_path = output_folder / "digi_rntuple.root";

    ROOT::RDF::RSnapshotOptions opts_ttree;
    opts_ttree.fOutputFormat = ROOT::RDF::ESnapshotOutputFormat::kTTree;
    ROOT::RDF::RSnapshotOptions opts_rntuple;
    opts_rntuple.fOutputFormat = ROOT::RDF::ESnapshotOutputFormat::kRNTuple;

    // Check write
    auto df = ROOT::RDataFrame("Events", input_path);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(detector_info_path), {"FEDRawDataCollection_rawDataCollector__LHC."});
    df2.Snapshot("Events", output_ttree_path.string(), {"FedChannelDigis"}, opts_ttree);
    df2.Snapshot("Events", output_rntuple_path.string(), {"FedChannelDigis"}, opts_rntuple);

    // Check read
    auto df_ttree_read = ROOT::RDataFrame("Events", output_ttree_path.string());
    auto df_rntuple_read = ROOT::RDataFrame("Events", output_rntuple_path.string());
    df_ttree_read.Foreach([](const std::vector<SiStripDigi>&) {}, {"FedChannelDigis"});
    df_rntuple_read.Foreach([](const std::vector<SiStripDigi>&) {}, {"FedChannelDigis"});

    if ((df_ttree_read.Count().GetValue() != df2.Count().GetValue()) || (df_rntuple_read.Count().GetValue() != df2.Count().GetValue())) {
        std::cerr << "Entries mismatch.\twrite: " << df2.Count().GetValue() << " read_ttree: " << df_ttree_read.Count().GetValue() << " read_rntuple: " << df_rntuple_read.Count().GetValue() <<"\n";
        return 1;
    }
}
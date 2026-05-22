#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "ROOT/RDataFrame.hxx"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

int main(int argc, char* argv[]){
    if (argc != 4) {
        std::cerr << "3 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_path> <detector_info> <output_path>\n";
        return 1;
    }
    std::string input_path(argv[1]);
    std::string detector_info_path(argv[2]);
    std::string output_path(argv[3]);

    ROOT::RDF::RSnapshotOptions opts;
    opts.fOutputFormat = ROOT::RDF::ESnapshotOutputFormat::kRNTuple;

    auto df = ROOT::RDataFrame("Events", input_path);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(detector_info_path), {"FEDRawDataCollection_rawDataCollector__LHC."});
    df2.Snapshot("Events", output_path, {"FedChannelDigis"}, opts);
}

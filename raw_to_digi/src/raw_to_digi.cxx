#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "ROOT/RDataFrame.hxx"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cerr << "2 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_to_digi <input_path> <output_path>";
        return 1;
    }
    std::string input_path(argv[1]);
    std::string output_path(argv[2]);
    auto df = ROOT::RDataFrame("Events", input_path);
    // df.Describe().Print();
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(), {"FEDRawDataCollection_rawDataCollector__LHC."});
    df2.Range(2,0).Snapshot("Events", output_path, {"FedChannelDigis"});
}

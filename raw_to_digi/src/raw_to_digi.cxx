#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "ROOT/RDataFrame.hxx"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

int main(){
    std::string input_path = "./tests/data/run_1000779_raw.root";
    std::string output_path = "./tests/data/run_1000779_digi.root";
    auto df = ROOT::RDataFrame("Events", input_path);
    // df.Describe().Print();
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(), {"FEDRawDataCollection_rawDataCollector__LHC."});
    df2.Snapshot("Events", output_path, {"FedChannelDigis"});
}

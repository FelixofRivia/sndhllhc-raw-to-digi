#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "SNDHLLHCHeaders.h"

int main(){
    std::string input_root_path = "./data/run_1000779_raw.root";
    std::string input_csv_path = "./data/run_1000779_raw_dump_from_cmssw.csv";
    std::unique_ptr<TFile> input_root_file( TFile::Open(input_root_path.c_str(), "READ") );
    std::ifstream input_csv_file(input_csv_path);

    auto input_raw_tree = input_root_file->Get<TTree>("Events");
    input_raw_tree->SetBranchStatus("*", false);
    input_raw_tree->SetBranchStatus("FEDRawDataCollection_rawDataCollector__LHC.*", true);

    edm::Wrapper<FEDRawDataCollection> *raw_data{nullptr};
    input_raw_tree->SetBranchAddress("FEDRawDataCollection_rawDataCollector__LHC.", &raw_data);

    std::vector<std::string> csv_row;

    for (std::string line; std::getline(input_csv_file, line);) {
        csv_row.clear();
        std::istringstream ss(std::move(line));
        for (std::string value; std::getline(ss, value, ',');) {
            csv_row.push_back(std::move(value));
        }
        for (const auto& s : csv_row) {
            std::cout << s << "\t";
        }
        std::cout << "\n";
    }

    input_csv_file.close();
}

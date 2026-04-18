#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "SNDHLLHCRawHeaders.h"

// .csv table: event number (+1 wrt tree), fed number, 8 bytes index (hex), 8 bytes (hex)

template<typename T>
std::string to_hex(T i, size_t n_digits)
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(n_digits) << std::hex << i;
  return stream.str();
}

int main(){
    std::string input_root_path = "./data/run_1000779_raw.root";
    std::string output_csv_path = "./data/run_1000779_raw_dump.csv";

    std::unique_ptr<TFile> input_root_file( TFile::Open(input_root_path.c_str(), "READ") );
    if (!input_root_file->IsOpen()) {
        std::cerr << "File not found: " << input_root_path << "\n";
        return 1;
    }
    std::ofstream output_csv_file(output_csv_path);
    if (!output_csv_file.is_open()) {
        std::cerr << "File not found: " << output_csv_path << "\n";
        return 1;
    }

    auto input_raw_tree = input_root_file->Get<TTree>("Events");
    input_raw_tree->SetBranchStatus("*", false);
    input_raw_tree->SetBranchStatus("FEDRawDataCollection_rawDataCollector__LHC.*", true);

    edm::Wrapper<FEDRawDataCollection> *raw_data{nullptr};
    input_raw_tree->SetBranchAddress("FEDRawDataCollection_rawDataCollector__LHC.", &raw_data);

    for(int i{0}; i < input_raw_tree->GetEntries() && i<10; ++i){
        input_raw_tree->GetEntry(i);
        if (!raw_data){
            std::cerr << "raw_data is nullptr\n";
            return 1;
        }
        std::vector<FEDRawData> feds = raw_data->obj.data_;
        for (size_t fed_index{0}; fed_index < feds.size(); ++fed_index) {
            if (feds[fed_index].data_.size() > 0) {
                size_t raw_size = feds[fed_index].data_.size();
                for (size_t raw_index{0}; raw_index < raw_size; raw_index += 8) {
                    // Equivalent to writing 8 bytes in line in hex (right to left)
                    std::string data;
                    for (size_t byte_index{raw_index}; byte_index < raw_index + 8; ++byte_index) {
                        data = to_hex(static_cast<int>(feds[fed_index].data_[byte_index]), 2) + data;
                    }
                    std::string csv_row = std::to_string(i+1) + std::string(",") + std::to_string(fed_index) + std::string(",") + to_hex(raw_index / 8, 4) + std::string(",") + data + std::string("\n");
                    output_csv_file << csv_row;
                }
            }
        }
    }
    output_csv_file.close();
    std::cout << "Raw data written to :" << output_csv_path << "\n";
}

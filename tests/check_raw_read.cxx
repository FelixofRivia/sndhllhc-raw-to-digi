#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "SiStripIOHeaders.h"

// .csv table: event number (+1 wrt tree), fed number, 8 bytes index (hex), 8 bytes (hex)

template<typename T>
std::string to_hex(T i, size_t n_digits)
{
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(n_digits) << std::hex << i;
  return stream.str();
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cerr << "2 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: check_raw_read <input_root_path> <input_csv_path>";
        return 1;
    }

    std::string input_root_path(argv[1]);
    std::string input_csv_path(argv[2]);

    std::unique_ptr<TFile> input_root_file( TFile::Open(input_root_path.c_str(), "READ") );
    if (!input_root_file->IsOpen()) {
        std::cerr << "File not found: " << input_root_path << "\n";
        return 1;
    }
    std::ifstream input_csv_file(input_csv_path);
    if (!input_csv_file.is_open()) {
        std::cerr << "File not found: " << input_csv_path << "\n";
        return 1;
    }

    std::string input_csv_line;
    std::vector<std::string> csv_rows;
    while (std::getline(input_csv_file, input_csv_line)) {
        csv_rows.push_back(input_csv_line);
    }

    auto input_raw_tree = input_root_file->Get<TTree>("Events");
    input_raw_tree->SetBranchStatus("*", false);
    input_raw_tree->SetBranchStatus("FEDRawDataCollection_rawDataCollector__LHC.*", true);

    edm::Wrapper<FEDRawDataCollection> *raw_data{nullptr};
    input_raw_tree->SetBranchAddress("FEDRawDataCollection_rawDataCollector__LHC.", &raw_data);

    size_t csv_row_index{0};

    for(int i{0}; i < input_raw_tree->GetEntries() && i < 10; ++i){
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
                    std::string csv_row_to_compare = std::to_string(i+1) + std::string(",") + std::to_string(fed_index) + std::string(",") + to_hex(raw_index / 8, 4) + std::string(",") + data; // + std::string("\n");
                    if (csv_row_to_compare != csv_rows[csv_row_index]) {
                        std::cerr << "Row number " << csv_row_index + 1 << " does not match:\nExpected:\t" << csv_rows[csv_row_index] << "\nProduced:\t" << csv_row_to_compare << "\n";
                        return 1;
                    }
                    csv_row_index++;
                }
            }
        }
    }
    input_csv_file.close();
    if (csv_row_index != csv_rows.size()) {
        std::cerr << "Number of rows does not match:\nExpected:\t" << csv_rows.size() << "\tProduced:\t" << csv_row_index << "\n";
        return 1;
    }
}

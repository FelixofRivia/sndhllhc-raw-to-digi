#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include "ROOT/RDataFrame.hxx"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

// .csv table: event number, detector id, strip, adc

int main(int argc, char* argv[]){
    if (argc != 3) {
        std::cerr << "2 arguments expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: check_raw_read <input_root_path> <input_csv_path>\n";
        return 1;
    }

    std::string input_root_path(argv[1]);
    std::string input_csv_path(argv[2]);
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

    auto df = ROOT::RDataFrame("Events", input_root_path);
    auto df2 = df.Define("FedChannelDigis", SiStripRawToDigi(), {"FEDRawDataCollection_rawDataCollector__LHC."});

    //df2.Range(2,3).Display({"FedChannelDigis"})->Print();
    size_t csv_row_index{0};
    auto print_line = [&csv_row_index](std::vector<SiStripDigi> digis){ 
        for (const auto& digi : digis) {
            if (digi.adc()>0) std::cout << csv_row_index << "," << digi.id() << "," << digi.strip() << "," << digi.adc() << "\n";
        }
        csv_row_index++;
    };
    df2.Range(3,4).Foreach(print_line, {"FedChannelDigis"});

    

    // for(int i{0}; i < input_raw_tree->GetEntries() && i < 10; ++i){
    //     input_raw_tree->GetEntry(i);
    //     if (!raw_data){
    //         std::cerr << "raw_data is nullptr\n";
    //         return 1;
    //     }
    //     std::vector<FEDRawData> feds = raw_data->obj.data_;
    //     for (size_t fed_index{0}; fed_index < feds.size(); ++fed_index) {
    //         if (feds[fed_index].data_.size() > 0) {
    //             size_t raw_size = feds[fed_index].data_.size();
    //             for (size_t raw_index{0}; raw_index < raw_size; raw_index += 8) {
    //                 // Equivalent to writing 8 bytes in line in hex (right to left)
    //                 std::string data;
    //                 for (size_t byte_index{raw_index}; byte_index < raw_index + 8; ++byte_index) {
    //                     data = to_hex(static_cast<int>(feds[fed_index].data_[byte_index]), 2) + data;
    //                 }
    //                 std::string csv_row_to_compare = std::to_string(i+1) + std::string(",") + std::to_string(fed_index) + std::string(",") + to_hex(raw_index / 8, 4) + std::string(",") + data; // + std::string("\n");
    //                 if (csv_row_to_compare != csv_rows[csv_row_index]) {
    //                     std::cerr << "Row number " << csv_row_index + 1 << " does not match:\nExpected:\t" << csv_rows[csv_row_index] << "\nProduced:\t" << csv_row_to_compare << "\n";
    //                     return 1;
    //                 }
    //                 csv_row_index++;
    //             }
    //         }
    //     }
    // }
    input_csv_file.close();
    // if (csv_row_index != csv_rows.size()) {
    //     std::cerr << "Number of rows does not match:\nExpected:\t" << csv_rows.size() << "\tProduced:\t" << csv_row_index << "\n";
    //     return 1;
    // }
}

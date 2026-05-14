#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include "ROOT/RDataFrame.hxx"

#include "SiStripIOHeaders.h"
#include "SiStripRawToDigi.h"

// .csv table: event number, strip, adc

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

    size_t csv_row_index{0};
    size_t event_number{0};
    auto check_line = [&csv_row_index, &event_number, &csv_rows](const std::vector<SiStripDigi>& digis){ 
        for (const auto& digi : digis) {
            //std::cout << event_number << "," << digi.strip() << "," << digi.adc() << "\n";
            std::string csv_row_to_compare = std::to_string(event_number) + std::string(",") + std::to_string(digi.strip()) + std::string(",") + std::to_string(digi.adc());
            if (csv_row_to_compare != csv_rows[csv_row_index]) {
                std::cerr << "Row number " << csv_row_index + 1 << " does not match:\nExpected:\t" << csv_rows[csv_row_index] << "\nProduced:\t" << csv_row_to_compare << "\n";
                throw std::runtime_error("CSV row mismatch");
            }
            csv_row_index++;
        }
        event_number++;
    };
    df2.Foreach(check_line, {"FedChannelDigis"});

    
    input_csv_file.close();
    if (csv_row_index != csv_rows.size()) {
        std::cerr << "Number of rows does not match:\nExpected:\t" << csv_rows.size() << "\tProduced:\t" << csv_row_index << "\n";
        return 1;
    }
}

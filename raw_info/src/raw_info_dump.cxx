#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "SiStripIOHeaders.h"

int main(int argc, char* argv[]){
    if (argc != 2) {
        std::cerr << "1 argument expected but " << argc - 1 << " provided\n";
        std::cerr << "Usage: raw_info_dump <input_root_path>\n";
        return 1;
    }
    std::string input_path(argv[1]);
    std::unique_ptr<TFile> input_raw_file( TFile::Open(input_path.c_str(), "READ") );

    auto input_raw_tree = input_raw_file->Get<TTree>("Events");
    input_raw_tree->SetBranchStatus("*", false);
    input_raw_tree->SetBranchStatus("FEDRawDataCollection_rawDataCollector__LHC.*", true);

    //input_raw_tree->Scan("FEDRawDataCollection_rawDataCollector__LHC.obj.data_.data_");

    edm::Wrapper<FEDRawDataCollection> *raw_data{nullptr};
    input_raw_tree->SetBranchAddress("FEDRawDataCollection_rawDataCollector__LHC.", &raw_data);
    
    for(int i{1}; i < input_raw_tree->GetEntries() && i<2; ++i){
        input_raw_tree->GetEntry(i);
        std::cout << "Entry: " << i << "\n";
        if (!raw_data){
            std::cout << "raw_data is nullptr\n";
            continue;
        }
        std::cout << "raw_data->present: " << raw_data->present << "\n";
        std::vector<FEDRawData> feds = raw_data->obj.data_;
        std::cout << "feds.size(): " << feds.size() << "\n";
        for (size_t fed_index{0}; fed_index < feds.size(); ++fed_index) {
            if (feds[fed_index].data_.size() > 0) {
                size_t raw_size = feds[fed_index].data_.size();
                std::cout << "fed index: " << fed_index << "\tsize: " << raw_size << "\n";
                for (size_t raw_index{0}; raw_index < raw_size; ++raw_index) {
                    printf("byte index: %lu data: %hhu \n", raw_index, feds[fed_index].data_[raw_index]);
                }
            }
        }
    }
}

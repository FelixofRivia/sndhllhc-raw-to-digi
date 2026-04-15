#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "SNDHLLHCHeaders.h"

int main(){
    std::string input_path = "/home/filippo/raw_to_digi/raw_data/run1000779/run1000779.root";
    std::unique_ptr<TFile> input_raw_file( TFile::Open(input_path.c_str(), "READ") );

    auto input_raw_tree = input_raw_file->Get<TTree>("Events");
    input_raw_tree->SetBranchStatus("*", false);
    input_raw_tree->SetBranchStatus("FEDRawDataCollection_rawDataCollector__LHC.*", true);

    // input_raw_tree->Scan("FEDRawDataCollection_rawDataCollector__LHC.present");

    edm::Wrapper<FEDRawDataCollection> *raw_data{nullptr};
    input_raw_tree->SetBranchAddress("FEDRawDataCollection_rawDataCollector__LHC.", &raw_data);
    
    for(int i{0}; i < input_raw_tree->GetEntries(); ++i){
        input_raw_tree->GetEntry(i);
        std::cout << "Entry: " << i << "\n";
        if (!raw_data){
            std::cout << "raw_data is nullptr\n";
            continue;
        }
        std::cout << "raw_data->present: " << raw_data->present << "\n";
        // std::cout << "raw_data->product(): " << raw_data->product() << "\n";
        // if (raw_data->present) std::cout << "present\n";
    }
}

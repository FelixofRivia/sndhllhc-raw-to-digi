#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "TFile.h"
#include "TTree.h"

#include "Wrapper.h"
#include "FEDRawData.h"

int main(){
    std::string input_path = "/home/filippo/raw_to_digi/raw_data/run1000779/run1000779.root";
    std::unique_ptr<TFile> input_raw_file( TFile::Open(input_path.c_str(), "READ") );

    auto input_raw_tree = input_raw_file->Get<TTree>("Events");
    input_raw_tree->SetBranchStatus("*", false);
    input_raw_tree->SetBranchStatus("FEDRawDataCollection_rawDataCollector__LHC.", true);

    sndhllhc::Wrapper<std::vector<sndhllhc::FEDRawData>> raw_data;
    input_raw_tree->SetBranchAddress("FEDRawDataCollection_rawDataCollector__LHC.", &raw_data);

    for(int i{0}; i < input_raw_tree->GetEntries(); ++i){
        std::cout << "Entry: " << i << "\n";
        if (raw_data.product()) std::cout << "present\n";
    }
}

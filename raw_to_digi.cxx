#include <iostream>
#include "TFile.h"

#include "Wrapper.h"
#include "FEDRawData.h"

int main(){
    std::cout << "I like RNTuples\n";
    TFile *file = new TFile("test.root", "RECREATE");
    file->Close();
}

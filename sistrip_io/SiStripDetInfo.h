#ifndef SNDHLLHC_SISTRIP_DET_INFO
#define SNDHLLHC_SISTRIP_DET_INFO

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

struct DetectorInfo {
    int detid;
    int dcuhardid;
    int fedid;
    int ccuaddress;
    int i2channel;
    int fedchannel;
    int napvpairs;
    int layer;
    int row;
    int column;
};

inline bool CheckDetectorInfo(const DetectorInfo& detinfo) {
    if (detinfo.napvpairs > 3) {
        std::cerr << "napvpairs > 3 (" << detinfo.napvpairs << ")\n";
        return false;
    }
    if (detinfo.row > 3) {
        std::cerr << "detinfo.row > 3 (" << detinfo.row << ")\n";
        return false;
    }
    if (detinfo.column > 2) {
        std::cerr << "detinfo.column > 2 (" << detinfo.column << ")\n";
        return false;
    }
    return true;
}

inline std::vector<DetectorInfo> GetDetectorInfo(const std::string& file_name) {
    std::vector<DetectorInfo> records;
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "Could not open file " << file_name << "\n";
        return records;
    }

    std::string line;

    // Skip header line
    std::getline(file, line);

    // Read each CSV row
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;

        DetectorInfo data;
        try {
            std::getline(ss, value, ',');
            data.detid = std::stoi(value);

            std::getline(ss, value, ',');
            data.dcuhardid = std::stoi(value);

            std::getline(ss, value, ',');
            data.fedid = std::stoi(value);

            std::getline(ss, value, ',');
            data.ccuaddress = std::stoi(value);

            std::getline(ss, value, ',');
            data.i2channel = std::stoi(value);

            std::getline(ss, value, ',');
            data.fedchannel = std::stoi(value);

            std::getline(ss, value, ',');
            data.napvpairs = std::stoi(value);

            std::getline(ss, value, ',');
            data.layer = std::stoi(value);

            std::getline(ss, value, ',');
            data.row = std::stoi(value);

            std::getline(ss, value, ',');
            data.column = std::stoi(value);
        }
        catch (const std::exception& e) {
            std::cerr << "CSV parse error: " << e.what() << "\n";
            continue;
        }
        
        if (CheckDetectorInfo(data)) {
            records.push_back(data);
        }
        else {
            std::cerr << "Invalid detector info found\n";
        }
    }

    return records;
}

#endif
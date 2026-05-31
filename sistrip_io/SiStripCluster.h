#ifndef SNDHLLHC_SISTRIP_CLUSTER_H
#define SNDHLLHC_SISTRIP_CLUSTER_H

#include <cstdint>

struct Module {
    int layer, row, col;

    bool operator<(const Module& other) const {
        if (layer != other.layer) return layer < other.layer;
        if (row != other.row) return row < other.row;
        return col < other.col;
    }
};

class SiStripCluster {
public:
    SiStripCluster(uint32_t adc, size_t size, int layer, int row, int column) : 
        adc_(adc), size_(size), layer_(layer), row_(row), column_(column) {}

    uint32_t GetSignal() const { return adc_; }
    size_t GetSize() const { return size_; }
    int GetLayer() const { return layer_; }
    int GetRow() const { return row_; }
    int GetColumn() const { return column_; }

private:
    uint32_t adc_;
    size_t size_;
    int layer_;
    int row_;
    int column_;
};

struct SiStripClusteringProducts {
    std::vector<SiStripDigi> digis;
    std::vector<SiStripCluster> clusters;
};

#endif
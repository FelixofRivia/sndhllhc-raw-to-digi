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

struct SiStripCluster {
  uint32_t adc_;
  size_t size_;
  int layer_;
  int row_;
  int column_;
};

#endif
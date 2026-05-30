#ifndef SNDHLLHC_SISTRIP_DIGI_CLUSTERING_H
#define SNDHLLHC_SISTRIP_DIGI_CLUSTERING_H

#include "SiStripIOHeaders.h"
#include "SiStripHardwareConstants.h"
#include <vector>
#include <map>
#include <cstdint>

struct Module {
    int layer, row, col;

    bool operator<(const Module& other) const {
        if (layer != other.layer) return layer < other.layer;
        if (row != other.row) return row < other.row;
        return col < other.col;
    }
};

class SiStripDigiClustering {
    public:
        SiStripDigiClustering() { ; }
        std::vector<SiStripDigi> operator()(const std::vector<SiStripDigi>& input_digis) const;
};

std::vector<SiStripDigi> SiStripDigiClustering::operator()(const std::vector<SiStripDigi>& input_digis) const
{
    std::map<Module, std::vector<SiStripDigi>> grouped;

    // Group by module
    for (const auto& d : input_digis)
    {
        grouped[{d.GetLayer(), d.GetRow(), d.GetColumn()}].push_back(d);
    }

    std::vector<SiStripDigi> clustered_output;

    constexpr uint16_t seed_thr  = 3.0f * SISTRIP_NOISE_ADC;
    constexpr uint16_t neigh_thr = 2.0f * SISTRIP_NOISE_ADC;
    constexpr uint16_t cluster_cut_factor = SISTRIP_NOISE_ADC * SISTRIP_NOISE_ADC;

    // Cluster each module independently
    for (auto& [key, digis] : grouped)
    {
        std::sort(digis.begin(), digis.end(), [](const SiStripDigi& a, const SiStripDigi& b) { return a.GetStrip() < b.GetStrip();});
        std::vector<bool> used(digis.size(), false);

        for (size_t i = 0; i < digis.size(); ++i)
        {
            if (used[i]) continue;

            const SiStripDigi& seed = digis[i];

            if (seed.GetSignal() <= seed_thr)
                continue;

            std::vector<size_t> cluster;
            uint16_t sum_signal = 0.0f;

            // start cluster
            cluster.push_back(i);
            used[i] = true;
            sum_signal += seed.GetSignal();

            int last_strip = seed.GetStrip();

            // forward
            for (size_t j = i + 1; j < digis.size(); ++j)
            {
                if (used[j]) continue;

                const SiStripDigi& d = digis[j];

                if (d.GetStrip() != last_strip + 1)
                    break;

                if (d.GetSignal() <= neigh_thr)
                    break;

                cluster.push_back(j);
                used[j] = true;
                sum_signal += d.GetSignal();
                last_strip = d.GetStrip();
            }

            // backward
            last_strip = seed.GetStrip();

            for (int j = (int)i - 1; j >= 0; --j)
            {
                if (used[j]) continue;

                const SiStripDigi& d = digis[j];

                if (d.GetStrip() != last_strip - 1)
                    break;

                if (d.GetSignal() <= neigh_thr)
                    break;

                cluster.push_back(j);
                used[j] = true;
                sum_signal += d.GetSignal();
                last_strip = d.GetStrip();
            }

            uint16_t threshold = cluster.size() * cluster_cut_factor;

            if (sum_signal > threshold)
            {
                for (auto idx : cluster)
                    clustered_output.push_back(digis[idx]);
            }
            else
            {
                // reject cluster → free digis
                for (auto idx : cluster)
                    used[idx] = false;
            }
        }
    }

    return clustered_output;
}

#endif
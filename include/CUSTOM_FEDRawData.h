#ifndef SNDHLLHC_RAWTODIGI_FEDRAWDATA
#define SNDHLLHC_RAWTODIGI_FEDRAWDATA

namespace sndhllhc {
    class FEDRawData {
        public:
            const unsigned char * data() const { return data_.data(); }
        private:
            std::vector<unsigned char> data_;
    };
}

#endif
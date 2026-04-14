#ifndef SNDHLLHC_RAWTODIGI_WRAPPER
#define SNDHLLHC_RAWTODIGI_WRAPPER

namespace sndhllhc {
    template <typename T>
    class Wrapper {
        public:
            T const* product() const { return (present_ ? &obj_ : nullptr); }
        private:
            bool present_{};
            T obj_;
    };
}

#endif
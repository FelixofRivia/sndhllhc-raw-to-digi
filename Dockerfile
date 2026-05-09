FROM almalinux:9

RUN dnf update -y
RUN dnf install -y epel-release cmake g++ python python-devel libX11 libX11-devel pcre2-devel libXpm libXpm-devel libXft libXft-devel libXext libXext-devel openssl openssl-devel libuuid-devel
ENV PYTHONPATH $ROOTSYS/lib:$PYTHONPATH

cd home
mkdir ROOT
cd ROOT
mkdir build install
RUN git clone --branch v6-40-00-rc1 --depth=1 https://github.com/root-project/root.git root_src
RUN cd build
RUN cmake -DCMAKE_INSTALL_PREFIX=../install ../root_src/ -Droot7=ON -Dbuiltin_gif=ON -Dbuiltin_jpeg=ON -Dbuiltin_lz4=ON -Dbuiltin_lzma=ON -Dbuiltin_zlib=ON -Dbuiltin_zstd=ON -Dbuiltin_xxhash=ON -Dbuiltin_png=ON -Dbuiltin_tiff=ON -Dbuiltin_freetype=ON -Dbuiltin_xrootd=OFF -Dxrootd=OFF
RUN cmake --build . --target install -j8

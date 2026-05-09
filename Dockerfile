FROM almalinux:9

ARG ROOT_BIN=root_v6.38.04.Linux-almalinux9.7-x86_64-gcc11.5.tar.gz

WORKDIR /opt

RUN dnf -y update && \
    dnf -y install dnf-plugins-core epel-release && \
    dnf config-manager --set-enabled crb

RUN dnf -y install --nobest --skip-broken \
        avahi-compat-libdns_sd-devel \
        avahi-devel \
        binutils \
        curl \
        davix-devel \
        fcgi \
        fcgi-devel \
        fftw-devel \
        ftgl-devel \
        gcc \
        gcc-c++ \
        gcc-gfortran \
        giflib-devel \
        git \
        gl2ps-devel \
        glew-devel \
        graphviz-devel \
        gsl-devel \
        libAfterImage-devel \
        libjpeg-turbo-devel \
        libtiff-devel \
        libX11-devel \
        libXext-devel \
        libXft-devel \
        libxml2-devel \
        libXpm-devel \
        libzstd-devel \
        lz4-devel \
        make \
        mesa-libGL-devel \
        mesa-libGLU-devel \
        ncurses-libs \
        ocaml-findlib-devel \
        openssl-devel \
        pcre-devel \
        protobuf-compiler \
        protobuf-devel \
        pythia8-devel \
        python3 \
        python3-devel \
        python3-numpy \
        python3-pip \
        qt5-qtwebengine-devel \
        rapidyaml-devel \
        redhat-rpm-config \
        tbb-devel \
        unuran-devel \
        xrootd-server-devel \
        xxhash-devel \
        xz-devel \
        zeromq-devel \
        zlib-devel

RUN curl -O https://root.cern/download/${ROOT_BIN} &&\
    tar -xzvf ${ROOT_BIN} &&\
    rm -f ${ROOT_BIN} &&\
    echo /opt/root/lib >> /etc/ld.so.conf &&\
    ldconfig


ENV ROOTSYS=/opt/root
ENV PATH=$ROOTSYS/bin:$PATH
ENV PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH

CMD ["root", "-b"]




FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive
ENV LLVM_DIR /usr/lib/llvm-18/
ENV PASS_DIR /llvm-pass

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    ninja-build \
    build-essential \
    python3-minimal python3-pip\
    wget \
    libzstd-dev \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add && \
    apt-add-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main" && \
    apt-get update && \
    apt-get install -y llvm-18 llvm-18-dev llvm-18-tools clang-18 python3-setuptools

RUN pip3 install lit

COPY PrintBBUIDs/build.sh /root/
RUN chmod +x /root/build.sh

COPY PrintBBUIDs/ ${PASS_DIR}

RUN mkdir -p $PASS_DIR/build && cd $PASS_DIR/build && \
    cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR $PASS_DIR && \
    make -j $(nproc --all) 

ENTRYPOINT ["/root/build.sh"]

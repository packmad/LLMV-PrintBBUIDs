FROM ocaml/opam:ubuntu-22.04-ocaml-4.14

ENV DEBIAN_FRONTEND=noninteractive
ENV LLVM_DIR=/usr/lib/llvm-18/
ENV PASS_DIR=/home/opam/llvm-pass

SHELL ["/bin/bash", "-c"]

#ENV TZ=Europe/Paris
#RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

USER root

RUN apt-get update && apt-get install -y \
    git \
    cmake \
    ninja-build \
    build-essential \
    python3-minimal python3-pip\
    wget \
    libzstd-dev \
    software-properties-common \
    libgmp-dev pkg-config graphviz libcairo2-dev libexpat1-dev libgtk-3-dev libgtksourceview-3.0-dev \
    && rm -rf /var/lib/apt/lists/*

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add && \
    apt-add-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main" && \
    apt-get update && \
    apt-get install -y llvm-18 llvm-18-dev llvm-18-tools clang-18 python3-setuptools


USER opam 
RUN pip3 install lit

# Install framac
RUN opam update
RUN opam install frama-c.33.0~beta
RUN eval $(opam env)

COPY --chown=opam:opam PrintBBUIDs/build.sh /home/opam
RUN chmod +x /home/opam/build.sh

COPY --chown=opam:opam PrintBBUIDs/ ${PASS_DIR}

RUN mkdir -p $PASS_DIR/build && cd $PASS_DIR/build && \
    cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR $PASS_DIR && \
    make -j $(nproc --all) 

ENTRYPOINT ["/home/opam/build.sh"]

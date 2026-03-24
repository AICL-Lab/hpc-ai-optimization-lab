# HPC-AI-Optimization-Lab Development Environment
# Baseline development image aligned with the documented CUDA 12.4+ support floor.
FROM nvcr.io/nvidia/cuda:12.4.1-devel-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    wget \
    curl \
    python3-dev \
    python3-pip \
    python3-venv \
    vim \
    gdb \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir \
    torch \
    numpy \
    pytest \
    matplotlib \
    pandas

ENV CUDA_HOME=/usr/local/cuda
ENV PATH=${CUDA_HOME}/bin:${PATH}
ENV LD_LIBRARY_PATH=${CUDA_HOME}/lib64:${LD_LIBRARY_PATH}

WORKDIR /workspace

CMD ["/bin/bash"]

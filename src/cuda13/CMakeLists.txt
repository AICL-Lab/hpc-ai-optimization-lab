# Experimental newer-CUDA feature demos and compatibility fallbacks.
# These targets are built as part of the lab, but they currently do not imply
# production-grade Hopper/Blackwell feature coverage.
hpc_add_cuda_library(hpc_cuda13
    tma.cu
    cluster.cu
    fp8_gemm.cu
)

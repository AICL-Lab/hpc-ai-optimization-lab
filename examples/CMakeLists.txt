# Examples CMakeLists.txt
# Build examples for HPC-AI-Optimization-Lab

message(STATUS "Building examples...")

# Common include directories
set(EXAMPLE_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
)

# Helper function to add CUDA example
function(add_cuda_example name source_file)
    add_executable(${name} ${source_file})
    target_include_directories(${name} PRIVATE ${EXAMPLE_INCLUDE_DIRS})
    target_compile_features(${name} PRIVATE cxx_std_20)
    set_target_properties(${name} PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES ${CMAKE_CUDA_ARCHITECTURES}
    )
    # Link against project libraries
    target_link_libraries(${name} PRIVATE
        hpc_elementwise
        hpc_reduction
        hpc_gemm
        hpc_attention
    )
endfunction()

# Elementwise examples
add_cuda_example(relu_example elementwise/relu_example.cu)

# Reduction examples
add_cuda_example(softmax_benchmark reduction/softmax_benchmark.cu)

# GEMM examples
add_cuda_example(gemm_benchmark gemm/gemm_benchmark.cu)
target_link_libraries(gemm_benchmark PRIVATE CUDA::cublas)

# Convolution examples
add_cuda_example(conv_example convolution/conv_example.cu)

# Attention examples
add_cuda_example(flash_attention_example attention/flash_attention_example.cu)

# Quantization examples
add_cuda_example(quant_example quantization/quant_example.cu)

# CUDA 13 examples
add_cuda_example(cuda13_example cuda13/cuda13_example.cu)

# Install examples
install(TARGETS relu_example softmax_benchmark gemm_benchmark conv_example flash_attention_example quant_example cuda13_example
    RUNTIME DESTINATION bin/examples
)

# Install Python examples
install(DIRECTORY python/
    DESTINATION share/examples/python
    FILES_MATCHING PATTERN "*.py"
)

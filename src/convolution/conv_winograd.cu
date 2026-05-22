#include <cmath>
#include <stdexcept>

#include "../common/cuda_check.cuh"
#include "conv_implicit_gemm.cuh"
#include "conv_winograd.cuh"

namespace hpc::convolution {

namespace {

__global__ void winograd_conv_kernel(const float* __restrict__ input,
                                     const float* __restrict__ weight, float* __restrict__ output,
                                     int batch, int in_ch, int out_ch, int out_h, int out_w,
                                     int in_h, int in_w);

struct Conv2DOutputShape {
    int height;
    int width;
};

struct WinogradPlan {
    bool use_winograd;
    Conv2DOutputShape output_shape;
    int tiles_h;
    int tiles_w;
};

void validate_conv2d_winograd_args(const float* input, const float* weight, float* output,
                                   const ConvParams& params) {
    if (input == nullptr || weight == nullptr || output == nullptr) {
        throw std::invalid_argument(
            "conv2d_winograd expects non-null input, weight, and output pointers");
    }
    if (params.batch <= 0 || params.in_channels <= 0 || params.out_channels <= 0 ||
        params.in_height <= 0 || params.in_width <= 0) {
        throw std::invalid_argument(
            "conv2d_winograd expects positive batch/channel/input dimensions");
    }
    if (params.kernel_h <= 0 || params.kernel_w <= 0 || params.stride_h <= 0 ||
        params.stride_w <= 0 || params.dilation_h <= 0 || params.dilation_w <= 0) {
        throw std::invalid_argument(
            "conv2d_winograd expects positive kernel, stride, and dilation values");
    }
    if (params.pad_h < 0 || params.pad_w < 0) {
        throw std::invalid_argument("conv2d_winograd expects non-negative padding");
    }
}

Conv2DOutputShape compute_conv2d_output_shape(const ConvParams& params) {
    return {
        (params.in_height + 2 * params.pad_h - params.dilation_h * (params.kernel_h - 1) - 1) /
                params.stride_h +
            1,
        (params.in_width + 2 * params.pad_w - params.dilation_w * (params.kernel_w - 1) - 1) /
                params.stride_w +
            1,
    };
}

WinogradPlan build_winograd_plan(const ConvParams& params, const WinogradConfig& config) {
    const auto output_shape = compute_conv2d_output_shape(params);
    const bool use_winograd =
        config.use_winograd && params.kernel_h == 3 && params.kernel_w == 3 &&
        params.stride_h == 1 && params.stride_w == 1 && params.pad_h == 1 && params.pad_w == 1 &&
        params.dilation_h == 1 && params.dilation_w == 1;
    return {
        use_winograd,
        output_shape,
        (output_shape.height + 1) / 2,
        (output_shape.width + 1) / 2,
    };
}

void launch_winograd_conv(const float* input, const float* weight, float* output,
                          const ConvParams& params, const WinogradPlan& plan,
                          cudaStream_t stream) {
    dim3 block(4, 4);
    dim3 grid(plan.tiles_h * plan.tiles_w);
    size_t smem_size = sizeof(float) * (16 + 16 + 16);

    winograd_conv_kernel<<<grid, block, smem_size, stream>>>(
        input, weight, output, params.batch, params.in_channels, params.out_channels,
        plan.output_shape.height, plan.output_shape.width, params.in_height, params.in_width);
}

void launch_winograd_fallback(const float* input, const float* weight, float* output,
                              const ConvParams& params, cudaStream_t stream) {
    conv2d_implicit_gemm<float>(input, weight, output, params, stream);
}

}  // namespace

__device__ constexpr float winograd_BT[16] = {1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                                              0.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, -1.0f};

__device__ constexpr float winograd_G[16] = {1.0f, 0.0f,  0.0f, 0.0f,  0.5f, 0.5f, 0.5f, 0.5f,
                                             0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f};

__device__ constexpr float winograd_AT[16] = {1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f,
                                              0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  -1.0f};

__device__ __forceinline__ float winograd_transform_input(float d[4][4], int i, int j) {
    float result = 0.0f;
    for (int ri = 0; ri < 4; ++ri) {
        for (int rj = 0; rj < 4; ++rj) {
            result += winograd_AT[i * 4 + ri] * d[ri][rj] * winograd_AT[j * 4 + rj];
        }
    }
    return result;
}

__device__ __forceinline__ float winograd_transform_weight(float g[3][3], int i, int j) {
    float result = 0.0f;
    for (int ri = 0; ri < 3; ++ri) {
        for (int rj = 0; rj < 3; ++rj) {
            result += winograd_G[i * 4 + ri] * g[ri][rj] * winograd_G[j * 4 + rj];
        }
    }
    return result;
}

__global__ void winograd_conv_kernel(const float* __restrict__ input,
                                     const float* __restrict__ weight, float* __restrict__ output,
                                     int batch, int in_ch, int out_ch, int out_h, int out_w,
                                     int in_h, int in_w) {
    const int tile_h = 4;
    const int tile_w = 4;

    extern __shared__ float smem[];
    float* s_input = smem;
    float* s_weight = s_input + tile_h * tile_w * 16;
    float* s_output = s_weight + 16;

    int tile_idx = blockIdx.x;
    int tile_h_idx = tile_idx / ((out_w + tile_w - 1) / tile_w);
    int tile_w_idx = tile_idx % ((out_w + tile_w - 1) / tile_w);

    int output_row = tile_h_idx * (tile_h - 2) + threadIdx.y;
    int output_col = tile_w_idx * (tile_w - 2) + threadIdx.x;

    if (output_row < out_h && output_col < out_w) {
        float d[4][4] = {0};

        for (int c = 0; c < in_ch; ++c) {
            for (int dy = 0; dy < tile_h; ++dy) {
                for (int dx = 0; dx < tile_w; ++dx) {
                    int in_row = output_row + dy - 1;
                    int in_col = output_col + dx - 1;

                    if (in_row >= 0 && in_row < in_h && in_col >= 0 && in_col < in_w) {
                        d[dy][dx] =
                            input[(batch * in_ch + c) * in_h * in_w + in_row * in_w + in_col];
                    }
                }
            }

            float d_win[4][4];
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    d_win[i][j] = 0;
                    for (int ri = 0; ri < 4; ++ri) {
                        d_win[i][j] += winograd_BT[i * 4 + ri] * d[ri][j];
                    }
                }
            }

            for (int ox = 0; ox < 4; ++ox) {
                for (int oy = 0; oy < 4; ++oy) {
                    d[oy][ox] = 0;
                    for (int ri = 0; ri < 4; ++ri) {
                        d[oy][ox] += d_win[oy][ri] * winograd_BT[ox * 4 + ri];
                    }
                }
            }

            for (int oc = 0; oc < out_ch; ++oc) {
                float g[3][3] = {0};
                for (int ky = 0; ky < 3; ++ky) {
                    for (int kx = 0; kx < 3; ++kx) {
                        g[ky][kx] = weight[(oc * in_ch + c) * 9 + ky * 3 + kx];
                    }
                }

                float g_win[4][4];
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        g_win[i][j] = 0;
                        for (int ri = 0; ri < 3; ++ri) {
                            g_win[i][j] += winograd_G[i * 4 + ri] * g[ri][j % 3];
                        }
                    }
                }

                float m[4][4];
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        m[i][j] = d[i][j] * g_win[i][j];
                    }
                }

                if (output_row < out_h && output_col < out_w) {
                    float sum = 0;
                    for (int i = 0; i < 4; ++i) {
                        for (int j = 0; j < 4; ++j) {
                            sum += winograd_AT[i * 4 + j] * m[i][j];
                        }
                    }

                    int out_idx = (oc * out_h + output_row) * out_w + output_col;
                    if (threadIdx.y == 0 && threadIdx.x == 0) {
                        atomicAdd(&output[out_idx], sum);
                    }
                }
            }
        }
    }
}

void conv2d_winograd(const float* input, const float* weight, float* output,
                     const ConvParams& params, const WinogradConfig& config, cudaStream_t stream) {
    validate_conv2d_winograd_args(input, weight, output, params);
    const auto plan = build_winograd_plan(params, config);
    if (!plan.use_winograd) {
        launch_winograd_fallback(input, weight, output, params, stream);
        return;
    }

    launch_winograd_conv(input, weight, output, params, plan, stream);
    CUDA_CHECK_LAST();
}

void conv2d_winograd_fallback(const float* input, const float* weight, float* output,
                              const ConvParams& params, cudaStream_t stream) {
    validate_conv2d_winograd_args(input, weight, output, params);
    launch_winograd_fallback(input, weight, output, params, stream);
}

}  // namespace hpc::convolution

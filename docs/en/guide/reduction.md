# Reduction Optimization Deep Dive

This document provides a detailed introduction to CUDA reduction operation optimization techniques, including Warp Shuffle, Block Reduce, and Online Softmax.

## 1. Reduction Basics

### What is Reduction?

Reduction is the process of combining a set of data into a single value through an operation (such as summation or finding the maximum).

```
输入: [1, 2, 3, 4, 5, 6, 7, 8]
求和归约: 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 = 36
```

### Naive Implementation (Atomic Operations)

```cpp
__global__ void reduce_naive(const float* input, float* output, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {
        atomicAdd(output, input[idx]);  // 极慢！所有线程竞争同一地址
    }
}
```

**Problem**: Atomic operations serialize execution, resulting in extremely poor performance.

---

## 2. Warp Shuffle (线程束洗牌) Reduction

### Warp Shuffle Primitives

Warp Shuffle allows threads within a Warp to directly exchange register data without requiring Shared Memory.

```cpp
// __shfl_down_sync: Retrieve data from higher lane
// mask: Participating thread mask (0xffffffff = all 32 threads)
// var: Variable to exchange
// delta: Offset
float val = __shfl_down_sync(0xffffffff, var, delta);
```

### Warp-level Reduction

```cpp
__device__ float warp_reduce_sum(float val) {
    // Binary reduction
    for (int offset = 16; offset > 0; offset /= 2) {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    return val;  // Only lane 0 has the correct result
}
```

### Reduction Process Illustration

```
初始:  [0] [1] [2] [3] [4] [5] [6] [7] ... [31]

offset=16:
       [0+16] [1+17] [2+18] ... [15+31] [16] [17] ... [31]

offset=8:
       [0+8+16+24] [1+9+17+25] ... [7+15+23+31] ...

offset=4:
       [0+4+8+12+16+20+24+28] ...

offset=2:
       [0+2+4+6+8+10+12+14+16+18+20+22+24+26+28+30] ...

offset=1:
       [sum of all 32 elements] ...
```

---

## 3. Block Reduce (块级归约)

### Combining Warp Shuffle and Shared Memory

```cpp
__device__ float block_reduce_sum(float val) {
    __shared__ float shared[32];  // One slot per Warp
    
    int lane = threadIdx.x % 32;
    int warp_id = threadIdx.x / 32;
    
    // Step 1: Reduce within Warp
    val = warp_reduce_sum(val);
    
    // Step 2: Warp 0's lane 0 writes to Shared Memory
    if (lane == 0) {
        shared[warp_id] = val;
    }
    __syncthreads();
    
    // Step 3: First Warp reduces results from all Warps
    if (warp_id == 0) {
        val = (threadIdx.x < blockDim.x / 32) ? shared[lane] : 0.0f;
        val = warp_reduce_sum(val);
    }
    
    return val;  // Only thread 0 has the correct result
}
```

### Complete Block Reduce Kernel

```cpp
__global__ void reduce_block(const float* input, float* output, int n) {
    float sum = 0.0f;
    
    // Grid Stride Loop accumulation
    for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < n; i += blockDim.x * gridDim.x) {
        sum += input[i];
    }
    
    // Reduce within Block
    sum = block_reduce_sum(sum);
    
    // Block 0's thread 0 writes result
    if (threadIdx.x == 0) {
        atomicAdd(output, sum);  // Only a few Blocks compete
    }
}
```

---

## 4. Softmax Optimization

### Softmax Formula

```
softmax(x_i) = exp(x_i - max(x)) / sum(exp(x_j - max(x)))
```

### Naive Implementation (Three Passes)

```cpp
// Pass 1: Find maximum value
float max_val = -INFINITY;
for (int i = 0; i < n; ++i) {
    max_val = fmaxf(max_val, input[i]);
}

// Pass 2: Compute exp and sum
float sum = 0.0f;
for (int i = 0; i < n; ++i) {
    sum += expf(input[i] - max_val);
}

// Pass 3: Normalize
for (int i = 0; i < n; ++i) {
    output[i] = expf(input[i] - max_val) / sum;
}
```

### Online Softmax (在线Softmax) (Single Pass)

**Core Idea**: Maintain both max and sum simultaneously during traversal.

```cpp
__device__ void online_softmax(const float* input, float* output, int n) {
    float max_val = -INFINITY;
    float sum = 0.0f;
    
    // Single pass: Compute max and sum simultaneously
    for (int i = 0; i < n; ++i) {
        float x = input[i];
        float old_max = max_val;
        max_val = fmaxf(max_val, x);
        
        // Key: Adjust previous sum
        // sum = sum * exp(old_max - new_max) + exp(x - new_max)
        sum = sum * expf(old_max - max_val) + expf(x - max_val);
    }
    
    // Normalize
    for (int i = 0; i < n; ++i) {
        output[i] = expf(input[i] - max_val) / sum;
    }
}
```

### Online Softmax (在线Softmax) Mathematical Derivation

```
Let S_k = sum_{i=1}^{k} exp(x_i - m_k), where m_k = max_{i=1}^{k} x_i

When processing x_{k+1}:
- If x_{k+1} > m_k:
  m_{k+1} = x_{k+1}
  S_{k+1} = S_k * exp(m_k - m_{k+1}) + exp(x_{k+1} - m_{k+1})
          = S_k * exp(m_k - x_{k+1}) + 1

- If x_{k+1} <= m_k:
  m_{k+1} = m_k
  S_{k+1} = S_k + exp(x_{k+1} - m_k)
```

### Parallel Online Softmax (在线Softmax)

```cpp
__global__ void softmax_online(const float* input, float* output, int batch, int seq_len) {
    int batch_idx = blockIdx.x;
    const float* row = input + batch_idx * seq_len;
    float* out_row = output + batch_idx * seq_len;
    
    // Each thread handles partial elements
    float local_max = -INFINITY;
    float local_sum = 0.0f;
    
    for (int i = threadIdx.x; i < seq_len; i += blockDim.x) {
        float x = row[i];
        float old_max = local_max;
        local_max = fmaxf(local_max, x);
        local_sum = local_sum * expf(old_max - local_max) + expf(x - local_max);
    }
    
    // Combine (max, sum) pairs within Warp
    // Special handling needed: combine two (max, sum) pairs
    // ...
    
    // Combine within Block
    // ...
    
    // Final normalization
    // ...
}
```

---

## 5. LayerNorm (层归一化) Optimization

### LayerNorm Formula

```
y = (x - mean) / sqrt(var + eps) * gamma + beta

mean = sum(x) / n
var = sum((x - mean)^2) / n
```

### Welford's Algorithm (Welford算法) (Numerically Stable)

```cpp
__device__ void welford_update(float& mean, float& m2, float& count, float x) {
    count += 1.0f;
    float delta = x - mean;
    mean += delta / count;
    float delta2 = x - mean;
    m2 += delta * delta2;
}

__device__ void welford_combine(float& mean1, float& m2_1, float& count1,
                                 float mean2, float m2_2, float count2) {
    if (count2 == 0) return;
    
    float count = count1 + count2;
    float delta = mean2 - mean1;
    mean1 = (count1 * mean1 + count2 * mean2) / count;
    m2_1 = m2_1 + m2_2 + delta * delta * count1 * count2 / count;
    count1 = count;
}
```

### Optimized LayerNorm (层归一化) Kernel

```cpp
__global__ void layer_norm_kernel(const float* input, const float* gamma, const float* beta,
                                   float* output, int batch, int hidden_size, float eps) {
    int batch_idx = blockIdx.x;
    const float* x = input + batch_idx * hidden_size;
    float* y = output + batch_idx * hidden_size;
    
    // Welford's Algorithm to compute mean and variance
    float mean = 0.0f, m2 = 0.0f, count = 0.0f;
    
    for (int i = threadIdx.x; i < hidden_size; i += blockDim.x) {
        welford_update(mean, m2, count, x[i]);
    }
    
    // Combine within Warp
    // Combine within Block
    // ...
    
    float var = m2 / count;
    float inv_std = rsqrtf(var + eps);
    
    // Normalize
    for (int i = threadIdx.x; i < hidden_size; i += blockDim.x) {
        y[i] = (x[i] - mean) * inv_std * gamma[i] + beta[i];
    }
}
```

---

## 6. RMSNorm (均方根归一化) Optimization

### RMSNorm Formula

```
y = x / sqrt(mean(x^2) + eps) * gamma

RMS = sqrt(sum(x^2) / n)
```

### RMSNorm (均方根归一化) is Simpler than LayerNorm (层归一化)

- No need to calculate mean
- Only need to calculate sum of squares

```cpp
__global__ void rms_norm_kernel(const float* input, const float* gamma,
                                 float* output, int batch, int hidden_size, float eps) {
    int batch_idx = blockIdx.x;
    const float* x = input + batch_idx * hidden_size;
    float* y = output + batch_idx * hidden_size;
    
    // Compute sum of squares
    float sum_sq = 0.0f;
    for (int i = threadIdx.x; i < hidden_size; i += blockDim.x) {
        sum_sq += x[i] * x[i];
    }
    
    // Block reduction
    sum_sq = block_reduce_sum(sum_sq);
    
    // Broadcast RMS
    __shared__ float rms;
    if (threadIdx.x == 0) {
        rms = rsqrtf(sum_sq / hidden_size + eps);
    }
    __syncthreads();
    
    // Normalize
    for (int i = threadIdx.x; i < hidden_size; i += blockDim.x) {
        y[i] = x[i] * rms * gamma[i];
    }
}
```

---

## 7. Performance Comparison

### Softmax Performance

| Implementation | Passes | Relative Performance |
|----------------|--------|---------------------|
| Naive (Atomic) | 3 | 1× |
| Warp Shuffle | 3 | 10× |
| Online Softmax | 2 | 15× |
| Fused Online | 1 | 20× |

### LayerNorm (层归一化) vs RMSNorm (均方根归一化)

| Operation | Computation | Relative Performance |
|-----------|-------------|---------------------|
| LayerNorm | mean + var + norm | 1× |
| RMSNorm | rms + norm | 1.3× |

---

## 8. Best Practices

1. **Prefer Warp Shuffle**: Faster than Shared Memory
2. **Use Online Algorithms**: Reduce number of passes
3. **Welford's Algorithm**: Numerically stable variance calculation
4. **Fused Operations**: Reduce kernel launches and memory access

## References

- [Online Softmax Paper](https://arxiv.org/abs/2205.14135)
- [Welford's Algorithm](https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance)
- [NVIDIA CUB Library](https://nvlabs.github.io/cub/)

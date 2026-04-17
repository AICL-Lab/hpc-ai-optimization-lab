# FlashAttention Deep Dive

This document provides a detailed introduction to the principles and implementation of FlashAttention.

## 1. Problems with Standard Attention

### Standard Attention Formula

```
Attention(Q, K, V) = softmax(Q @ K^T / sqrt(d)) @ V
```

Where:
- Q: (batch, heads, seq_len, head_dim)
- K: (batch, heads, seq_len, head_dim)
- V: (batch, heads, seq_len, head_dim)

### Memory Issues

```
Q @ K^T produces an attention matrix of size (seq_len × seq_len)

seq_len = 2048, heads = 32, batch = 8
Attention matrix size = 8 × 32 × 2048 × 2048 × 4 bytes = 4.3 GB!
```

### IO Bottleneck

```
Standard implementation:
1. Compute S = Q @ K^T          → Write to HBM (N×N)
2. Compute P = softmax(S)       → Read HBM, Write HBM (N×N)
3. Compute O = P @ V            → Read HBM (N×N)

Total HBM accesses: O(N²) times
```

---

## 2. Core Ideas of FlashAttention

### Tiling + Online Softmax

**Core Idea**: Tile Q, K, V into SRAM (Shared Memory), perform all computations within SRAM, avoiding writing the N×N matrix to HBM.

```
FlashAttention:
1. Tile load Q, K, V into SRAM
2. Compute attention scores within SRAM
3. Use Online Softmax for incremental output updates
4. Only write final output O to HBM

Total HBM accesses: O(N) times
```

### Tiling Strategy

```
Q: (seq_len, head_dim) → Split into Tr tiles, each with Br rows
K: (seq_len, head_dim) → Split into Tc tiles, each with Bc rows
V: (seq_len, head_dim) → Split into Tc tiles, each with Bc rows

Br, Bc are chosen such that tiles fit into SRAM
```

---

## 3. Algorithm Details

### FlashAttention Forward Pseudocode

```python
def flash_attention_forward(Q, K, V, Br, Bc):
    N, d = Q.shape
    Tr = ceil(N / Br)  # Number of Q tiles
    Tc = ceil(N / Bc)  # Number of K, V tiles
    
    # Initialize output and statistics
    O = zeros(N, d)
    l = zeros(N)  # Softmax denominator
    m = full(N, -inf)  # Current maximum
    
    # Outer loop: Iterate over K, V tiles
    for j in range(Tc):
        Kj = K[j*Bc : (j+1)*Bc]  # Load K tile into SRAM
        Vj = V[j*Bc : (j+1)*Bc]  # Load V tile into SRAM
        
        # Inner loop: Iterate over Q tiles
        for i in range(Tr):
            Qi = Q[i*Br : (i+1)*Br]  # Load Q tile into SRAM
            
            # Compute attention scores within SRAM
            Sij = Qi @ Kj.T / sqrt(d)  # (Br, Bc)
            
            # Online Softmax update
            m_new = max(m[i*Br:(i+1)*Br], rowmax(Sij))
            P_tilde = exp(Sij - m_new[:, None])
            l_new = exp(m[i*Br:(i+1)*Br] - m_new) * l[i*Br:(i+1)*Br] + rowsum(P_tilde)
            
            # Update output
            O[i*Br:(i+1)*Br] = (
                exp(m[i*Br:(i+1)*Br] - m_new)[:, None] * O[i*Br:(i+1)*Br] +
                P_tilde @ Vj
            ) / l_new[:, None] * l[i*Br:(i+1)*Br][:, None]
            
            # Update statistics
            m[i*Br:(i+1)*Br] = m_new
            l[i*Br:(i+1)*Br] = l_new
    
    return O
```

### Application of Online Softmax in FlashAttention

```
When processing the j-th K, V tile:

1. Compute current tile's attention score: S_ij = Q_i @ K_j^T

2. Update maximum:
   m_new = max(m_old, rowmax(S_ij))

3. Compute scaled exp:
   P_tilde = exp(S_ij - m_new)

4. Update softmax denominator:
   l_new = l_old * exp(m_old - m_new) + rowsum(P_tilde)

5. Update output (Crucial!):
   O_new = (O_old * l_old * exp(m_old - m_new) + P_tilde @ V_j) / l_new
```

---

## 4. CUDA Implementation

### Kernel Structure

```cpp
__global__ void flash_attention_forward_kernel(
    const float* Q, const float* K, const float* V, float* O,
    int batch, int heads, int seq_len, int head_dim,
    int Br, int Bc) {
    
    // Each Block processes one (batch, head, Q_block)
    int batch_idx = blockIdx.x / heads;
    int head_idx = blockIdx.x % heads;
    int q_block_idx = blockIdx.y;
    
    // Shared Memory allocation
    extern __shared__ float smem[];
    float* Qi = smem;                           // (Br, head_dim)
    float* Kj = Qi + Br * head_dim;             // (Bc, head_dim)
    float* Vj = Kj + Bc * head_dim;             // (Bc, head_dim)
    float* Sij = Vj + Bc * head_dim;            // (Br, Bc)
    float* Oi = Sij + Br * Bc;                  // (Br, head_dim)
    float* li = Oi + Br * head_dim;             // (Br,)
    float* mi = li + Br;                        // (Br,)
    
    // Initialization
    // ...
    
    // Load Q tile
    load_tile(Q, Qi, batch_idx, head_idx, q_block_idx * Br, Br, head_dim);
    
    // Iterate over K, V tiles
    int num_kv_blocks = (seq_len + Bc - 1) / Bc;
    for (int j = 0; j < num_kv_blocks; ++j) {
        // Load K, V tiles
        load_tile(K, Kj, batch_idx, head_idx, j * Bc, Bc, head_dim);
        load_tile(V, Vj, batch_idx, head_idx, j * Bc, Bc, head_dim);
        __syncthreads();
        
        // Compute S = Q @ K^T
        compute_qk(Qi, Kj, Sij, Br, Bc, head_dim);
        __syncthreads();
        
        // Online Softmax update
        online_softmax_update(Sij, Vj, Oi, li, mi, Br, Bc, head_dim);
        __syncthreads();
    }
    
    // Write back output
    store_tile(Oi, O, batch_idx, head_idx, q_block_idx * Br, Br, head_dim);
}
```

### Warp Assignment Strategy (FlashAttention-2)

```
FlashAttention-1: Each Block processes one Q tile
FlashAttention-2: Finer-grained Warp assignment

Warp assignment within Block:
- Warps 0-3: Compute different parts of S = Q @ K^T
- Warps 4-7: Compute different parts of P @ V

Advantages:
- Better parallelism
- Reduced synchronization overhead
```

---

## 5. Memory Analysis

### SRAM Usage

```
Br = 64, Bc = 64, head_dim = 128

Qi: 64 × 128 × 4 = 32 KB
Kj: 64 × 128 × 4 = 32 KB
Vj: 64 × 128 × 4 = 32 KB
Sij: 64 × 64 × 4 = 16 KB
Oi: 64 × 128 × 4 = 32 KB
li, mi: 64 × 4 × 2 = 0.5 KB

Total: ~145 KB (requires large SRAM on Hopper)
```

### HBM Access Comparison

```
Standard Attention:
- Read Q, K, V: 3 × N × d
- Write S: N × N
- Read S, Write P: 2 × N × N
- Read P, V, Write O: N × N + N × d + N × d
Total: O(N²)

FlashAttention:
- Read Q: N × d (once)
- Read K, V: Tr × N × d (once per Q tile)
- Write O: N × d (once)
Total: O(N × d × Tr) = O(N²/Br) ≈ O(N) (when Br is large enough)
```

---

## 6. Performance Optimization Techniques

### 1. Using Tensor Cores

```cpp
// Use WMMA to accelerate Q @ K^T
wmma::fragment<wmma::matrix_a, 16, 16, 16, half, wmma::row_major> q_frag;
wmma::fragment<wmma::matrix_b, 16, 16, 16, half, wmma::col_major> k_frag;
wmma::fragment<wmma::accumulator, 16, 16, 16, float> s_frag;

wmma::load_matrix_sync(q_frag, Qi + ...);
wmma::load_matrix_sync(k_frag, Kj + ...);
wmma::mma_sync(s_frag, q_frag, k_frag, s_frag);
```

### 2. Asynchronous Data Loading (CUDA 11+)

```cpp
// Use cp.async for asynchronous loading
__pipeline_memcpy_async(Kj + offset, K + global_offset, sizeof(float4));
__pipeline_commit();
// ... computation ...
__pipeline_wait_prior(0);
```

### 3. Register Optimization

```cpp
// Place frequently accessed data in registers
float reg_o[8];  // Per-thread output accumulators
float reg_l, reg_m;  // Per-thread statistics
```

---

## 7. Causal Mask Support

### Causal Attention

```
Causal Mask: Only attend to current and previous tokens

Mask[i][j] = 1 if j <= i else 0
```

### Implementation Optimization

```cpp
// Skip tiles that don't need computation
if (j * Bc > (q_block_idx + 1) * Br) {
    continue;  // K tile is entirely after Q tile, skip
}

// Mask within tile
for (int i = 0; i < Br; ++i) {
    for (int j = 0; j < Bc; ++j) {
        int q_pos = q_block_idx * Br + i;
        int k_pos = kv_block_idx * Bc + j;
        if (k_pos > q_pos) {
            Sij[i * Bc + j] = -INFINITY;
        }
    }
}
```

---

## 8. Performance Comparison

### Memory Usage

| Implementation | seq_len=2048 | seq_len=4096 | seq_len=8192 |
|----------------|--------------|--------------|--------------|
| Standard | 4.3 GB | 17.2 GB | 68.7 GB |
| FlashAttention | 0.5 GB | 0.5 GB | 0.5 GB |

### Speed Comparison

| Implementation | seq_len=2048 | seq_len=4096 |
|----------------|--------------|--------------|
| PyTorch | 45 ms | 180 ms |
| FlashAttention | 12 ms | 48 ms |
| Speedup | 3.75× | 3.75× |

---

## 9. References

- [FlashAttention Paper](https://arxiv.org/abs/2205.14135)
- [FlashAttention-2 Paper](https://arxiv.org/abs/2307.08691)
- [FlashAttention GitHub](https://github.com/Dao-AILab/flash-attention)
- [Online Softmax Trick](https://arxiv.org/abs/2112.05682)

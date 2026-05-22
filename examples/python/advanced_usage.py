"""
Advanced Python Usage Examples for HPC-AI-Optimization-Lab

This script demonstrates advanced usage patterns including:
- Current shipped Python bindings accept float32 CUDA tensors only.
- Batch processing
- Performance profiling
- Integration with PyTorch workflows
"""

import torch
import time
import hpc_ai_opt
from typing import Tuple


def profile_operation(func, args, name: str, warmup: int = 10, iterations: int = 100):
    """Profile a CUDA operation and return performance metrics."""
    # Warmup
    for _ in range(warmup):
        func(*args)
    torch.cuda.synchronize()
    
    # Benchmark
    start = time.time()
    for _ in range(iterations):
        func(*args)
    torch.cuda.synchronize()
    end = time.time()
    
    avg_time_ms = (end - start) / iterations * 1000
    
    print(f"{name:<30} {avg_time_ms:>8.3f} ms")
    return avg_time_ms


def example_elementwise_operations():
    """Demonstrate elementwise operations on the shipped float32 surface."""
    print("\n" + "="*70)
    print("Example 1: Elementwise Operations")
    print("="*70)
    
    size = 1024 * 1024  # 1M elements
    
    # Float32
    x_f32 = torch.randn(size, device='cuda', dtype=torch.float32)
    y_f32 = torch.empty_like(x_f32)
    hpc_ai_opt.elementwise.relu(x_f32, y_f32)
    print(f"✓ ReLU (float32): output range [{y_f32.min():.3f}, {y_f32.max():.3f}]")
    
    # Sigmoid
    hpc_ai_opt.elementwise.sigmoid(x_f32, y_f32)
    print(f"✓ Sigmoid (float32): output range [{y_f32.min():.3f}, {y_f32.max():.3f}]")


def example_reduction_operations():
    """Demonstrate reduction operations with different configurations."""
    print("\n" + "="*70)
    print("Example 2: Reduction Operations")
    print("="*70)
    
    batch, seq_len = 32, 128
    
    # Softmax
    input = torch.randn(batch, seq_len, device='cuda', dtype=torch.float32)
    output = torch.empty_like(input)
    hpc_ai_opt.reduction.softmax(input, output, batch, seq_len)
    
    # Verify softmax properties
    row_sums = output.sum(dim=1)
    print(f"✓ Softmax: row sums should be ~1.0: mean={row_sums.mean():.6f}, std={row_sums.std():.6f}")
    
    # Check all values are positive
    print(f"✓ Softmax: all values positive: {(output > 0).all()}")


def example_gemm_variations():
    """Demonstrate GEMM with different matrix sizes and types."""
    print("\n" + "="*70)
    print("Example 3: GEMM Variations")
    print("="*70)
    
    # Square matrices
    M = N = K = 512
    A = torch.randn(M, K, device='cuda', dtype=torch.float32)
    B = torch.randn(K, N, device='cuda', dtype=torch.float32)
    C = torch.zeros(M, N, device='cuda', dtype=torch.float32)
    
    hpc_ai_opt.gemm.matmul(A, B, C, M, N, K, 1.0, 0.0)
    
    # Verify with PyTorch
    C_ref = torch.matmul(A, B)
    max_error = (C - C_ref).abs().max().item()
    print(f"✓ Square GEMM (512×512): max error = {max_error:.6e}")
    
    # Non-square matrices
    M, N, K = 1024, 512, 256
    A = torch.randn(M, K, device='cuda', dtype=torch.float32)
    B = torch.randn(K, N, device='cuda', dtype=torch.float32)
    C = torch.zeros(M, N, device='cuda', dtype=torch.float32)
    
    hpc_ai_opt.gemm.matmul(A, B, C, M, N, K, 1.0, 0.0)
    C_ref = torch.matmul(A, B)
    max_error = (C - C_ref).abs().max().item()
    print(f"✓ Non-square GEMM (1024×256 × 256×512): max error = {max_error:.6e}")


def example_batch_processing():
    """Demonstrate batch processing patterns."""
    print("\n" + "="*70)
    print("Example 4: Batch Processing")
    print("="*70)
    
    batch_size = 16
    M, N, K = 256, 256, 256
    
    # Process multiple GEMMs in a loop
    total_time = 0
    for i in range(batch_size):
        A = torch.randn(M, K, device='cuda', dtype=torch.float32)
        B = torch.randn(K, N, device='cuda', dtype=torch.float32)
        C = torch.zeros(M, N, device='cuda', dtype=torch.float32)
        
        start = time.time()
        hpc_ai_opt.gemm.matmul(A, B, C, M, N, K, 1.0, 0.0)
        torch.cuda.synchronize()
        total_time += time.time() - start
    
    avg_time_ms = total_time / batch_size * 1000
    flops = 2 * M * N * K
    tflops = flops / (avg_time_ms / 1000) / 1e12
    
    print(f"✓ Processed {batch_size} GEMMs")
    print(f"  Average time: {avg_time_ms:.3f} ms")
    print(f"  Performance: {tflops:.2f} TFLOPS")


def example_pytorch_integration():
    """Demonstrate integration with PyTorch autograd."""
    print("\n" + "="*70)
    print("Example 5: PyTorch Integration")
    print("="*70)
    
    M, N, K = 512, 512, 512
    
    # Create tensors with gradients enabled
    A = torch.randn(M, K, device='cuda', dtype=torch.float32, requires_grad=True)
    B = torch.randn(K, N, device='cuda', dtype=torch.float32, requires_grad=True)
    
    # Use PyTorch matmul for forward with autograd
    C = torch.matmul(A, B)
    
    # Use HPC GEMM for inference (faster, no gradients)
    with torch.no_grad():
        C_inference = torch.zeros(M, N, device='cuda', dtype=torch.float32)
        hpc_ai_opt.gemm.matmul(A.detach(), B.detach(), C_inference, M, N, K, 1.0, 0.0)
    
    # Compare results
    max_diff = (C - C_inference).abs().max().item()
    print(f"✓ PyTorch vs HPC GEMM: max difference = {max_diff:.6e}")
    
    # Backward pass with PyTorch
    loss = C.sum()
    loss.backward()
    print(f"✓ Backward pass completed")
    print(f"  A gradient shape: {A.grad.shape}")
    print(f"  B gradient shape: {B.grad.shape}")


def example_performance_profiling():
    """Profile performance across different operations."""
    print("\n" + "="*70)
    print("Example 6: Performance Profiling")
    print("="*70)
    print(f"{'Operation':<30} {'Time':>12}")
    print("-"*70)
    
    size = 1024 * 1024
    
    # Elementwise operations
    x = torch.randn(size, device='cuda', dtype=torch.float32)
    y = torch.empty_like(x)
    profile_operation(hpc_ai_opt.elementwise.relu, (x, y), "ReLU")
    profile_operation(hpc_ai_opt.elementwise.sigmoid, (x, y), "Sigmoid")
    
    # Reduction operations
    batch, seq_len = 128, 512
    input = torch.randn(batch, seq_len, device='cuda', dtype=torch.float32)
    output = torch.empty_like(input)
    profile_operation(hpc_ai_opt.reduction.softmax, (input, output, batch, seq_len), "Softmax")
    
    # GEMM operations
    M = N = K = 512
    A = torch.randn(M, K, device='cuda', dtype=torch.float32)
    B = torch.randn(K, N, device='cuda', dtype=torch.float32)
    C = torch.zeros(M, N, device='cuda', dtype=torch.float32)
    profile_operation(hpc_ai_opt.gemm.matmul, (A, B, C, M, N, K, 1.0, 0.0), "GEMM (512×512)")
    
    M = N = K = 1024
    A = torch.randn(M, K, device='cuda', dtype=torch.float32)
    B = torch.randn(K, N, device='cuda', dtype=torch.float32)
    C = torch.zeros(M, N, device='cuda', dtype=torch.float32)
    profile_operation(hpc_ai_opt.gemm.matmul, (A, B, C, M, N, K, 1.0, 0.0), "GEMM (1024×1024)")


def main():
    """Run all examples."""
    print("\n" + "="*70)
    print("HPC-AI-Optimization-Lab: Advanced Python Examples")
    print("="*70)
    
    if not torch.cuda.is_available():
        print("ERROR: CUDA not available. Please run on a GPU.")
        return
    
    print(f"GPU: {torch.cuda.get_device_name(0)}")
    print(f"PyTorch version: {torch.__version__}")
    
    # Run all examples
    example_elementwise_operations()
    example_reduction_operations()
    example_gemm_variations()
    example_batch_processing()
    example_pytorch_integration()
    example_performance_profiling()
    
    print("\n" + "="*70)
    print("All examples completed successfully!")
    print("="*70)


if __name__ == "__main__":
    main()

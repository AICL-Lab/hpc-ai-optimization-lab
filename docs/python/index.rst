hpc_ai_opt Python Bindings
==========================

`hpc_ai_opt` exposes a small set of CUDA kernels through nanobind.
The current Python API is intentionally thin:

- inputs and outputs are CUDA tensors supplied by the caller
- several kernels require explicit shape arguments
- the bindings mirror the C++ kernel layout through submodules

Build and import
----------------

.. code-block:: bash

   cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
   cmake --build build
   export PYTHONPATH="$(pwd)/build/python:${PYTHONPATH}"
   python -c "import hpc_ai_opt; print(hpc_ai_opt.__doc__)"

Current modules
---------------

- ``hpc_ai_opt.elementwise``
  - ``relu(input, output)``
  - ``sigmoid(input, output)``
  - ``transpose(input, output, rows, cols)``
- ``hpc_ai_opt.reduction``
  - ``softmax(input, output, batch, seq_len)``
  - ``layer_norm(input, gamma, beta, output, batch, hidden_size, eps)``
  - ``rms_norm(input, gamma, output, batch, hidden_size, eps)``
- ``hpc_ai_opt.gemm``
  - ``matmul(A, B, C, M, N, K, alpha, beta)``

Minimal example
---------------

.. code-block:: python

   import torch
   import hpc_ai_opt as opt

   x = torch.randn(1024, 1024, device="cuda", dtype=torch.float32)
   y = torch.empty_like(x)

   opt.elementwise.relu(x, y)
   torch.testing.assert_close(y, torch.relu(x))

Notes and current limitations
-----------------------------

- The bindings currently target CUDA tensors rather than NumPy CPU arrays.
- Output tensors are allocated by the caller.
- The public Python surface currently includes elementwise, reduction, and GEMM bindings only.
- Higher-level wrappers such as ``flash_attention`` and device-info helpers are not exposed in the current extension module.

Reference files
---------------

- ``examples/python/basic_usage.py``
- ``python/bindings/bindings.cpp``
- ``python/CMakeLists.txt``

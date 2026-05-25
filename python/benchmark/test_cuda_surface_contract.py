from __future__ import annotations

import pathlib
import unittest


REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
FLASH_ATTENTION_HEADER = REPO_ROOT / "src" / "attention" / "flash_attention.cuh"
CONV_IMPLICIT_GEMM_HEADER = REPO_ROOT / "src" / "convolution" / "conv_implicit_gemm.cuh"
CONV_WINOGRAD_HEADER = REPO_ROOT / "src" / "convolution" / "conv_winograd.cuh"
CONV_WINOGRAD_SOURCE = REPO_ROOT / "src" / "convolution" / "conv_winograd.cu"
CUDA13_DIR = REPO_ROOT / "src" / "cuda13"


class CUDASurfaceContractTest(unittest.TestCase):
    def test_flash_attention_header_only_advertises_float_surface(self) -> None:
        header = FLASH_ATTENTION_HEADER.read_text(encoding="utf-8")

        self.assertIn("requires std::is_same_v<T, float>", header)
        self.assertNotIn("std::is_same_v<T, __half>", header)

    def test_convolution_surface_is_centered_on_implicit_gemm(self) -> None:
        header = CONV_IMPLICIT_GEMM_HEADER.read_text(encoding="utf-8")

        self.assertIn("struct ConvParams", header)
        self.assertIn("void conv2d_implicit_gemm", header)

    def test_experimental_winograd_surface_is_removed(self) -> None:
        self.assertFalse(CONV_WINOGRAD_HEADER.exists())
        self.assertFalse(CONV_WINOGRAD_SOURCE.exists())

    def test_experimental_cuda13_directory_is_removed(self) -> None:
        self.assertFalse(CUDA13_DIR.exists())


if __name__ == "__main__":
    unittest.main()

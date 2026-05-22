from __future__ import annotations

import pathlib
import unittest


REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
FLASH_ATTENTION_HEADER = REPO_ROOT / "src" / "attention" / "flash_attention.cuh"
CONV_WINOGRAD_SOURCE = REPO_ROOT / "src" / "convolution" / "conv_winograd.cu"
TMA_HEADER = REPO_ROOT / "src" / "cuda13" / "tma.cuh"
TMA_SOURCE = REPO_ROOT / "src" / "cuda13" / "tma.cu"
CLUSTER_SOURCE = REPO_ROOT / "src" / "cuda13" / "cluster.cu"
FP8_HEADER = REPO_ROOT / "src" / "cuda13" / "fp8_gemm.cuh"
FP8_SOURCE = REPO_ROOT / "src" / "cuda13" / "fp8_gemm.cu"


class CUDASurfaceContractTest(unittest.TestCase):
    def test_flash_attention_header_only_advertises_float_surface(self) -> None:
        header = FLASH_ATTENTION_HEADER.read_text(encoding="utf-8")

        self.assertIn("requires std::is_same_v<T, float>", header)
        self.assertNotIn("std::is_same_v<T, __half>", header)

    def test_conv_winograd_native_path_is_gated_by_supported_geometry(self) -> None:
        source = CONV_WINOGRAD_SOURCE.read_text(encoding="utf-8")

        self.assertIn("build_winograd_plan", source)
        self.assertIn("params.kernel_h == 3 && params.kernel_w == 3", source)
        self.assertIn("params.stride_h == 1 && params.stride_w == 1", source)
        self.assertIn("params.pad_h == 1 && params.pad_w == 1", source)
        self.assertIn("params.dilation_h == 1 && params.dilation_w == 1", source)

    def test_tma_surface_centralizes_validation_and_dispatch(self) -> None:
        header = TMA_HEADER.read_text(encoding="utf-8")
        source = TMA_SOURCE.read_text(encoding="utf-8")

        self.assertIn("int cluster_width = 1;", header)
        self.assertIn("int cluster_height = 1;", header)
        self.assertIn("int pipeline_depth = 2;", header)
        self.assertIn("validate_tma_copy_args", source)
        self.assertIn("build_tma_copy_plan", source)

    def test_cluster_surface_centralizes_validation_and_launch_planning(self) -> None:
        source = CLUSTER_SOURCE.read_text(encoding="utf-8")

        self.assertIn("validate_cluster_reduce_args", source)
        self.assertIn("build_cluster_launch_plan", source)

    def test_fp8_surface_defaults_match_supported_launch_shape(self) -> None:
        header = FP8_HEADER.read_text(encoding="utf-8")
        source = FP8_SOURCE.read_text(encoding="utf-8")

        self.assertIn("int tile_m = 128;", header)
        self.assertIn("int tile_n = 128;", header)
        self.assertIn("int tile_k = 64;", header)
        self.assertIn("validate_fp8_gemm_args", source)
        self.assertIn("build_fp8_gemm_plan", source)
        self.assertIn('throw std::invalid_argument("fp8_gemm expects finite positive scale_a and scale_b")', source)


if __name__ == "__main__":
    unittest.main()

from __future__ import annotations

import importlib.util
import pathlib
import tempfile
import unittest
from types import SimpleNamespace
from unittest import mock


REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
BENCHMARK_PATH = REPO_ROOT / "python" / "benchmark" / "benchmark.py"
README_PATH = REPO_ROOT / "README.md"
README_ZH_PATH = REPO_ROOT / "README.zh-CN.md"
PYTHON_EXAMPLE_PATH = REPO_ROOT / "examples" / "python" / "basic_usage.py"
ADVANCED_PYTHON_EXAMPLE_PATH = REPO_ROOT / "examples" / "python" / "advanced_usage.py"
GEMM_HEADER_PATH = REPO_ROOT / "src" / "gemm" / "gemm.cuh"
BINDINGS_PATH = REPO_ROOT / "python" / "bindings" / "bindings.cpp"
VALIDATION_HEADER_PATH = REPO_ROOT / "python" / "bindings" / "validation.hpp"


def load_benchmark_module():
    spec = importlib.util.spec_from_file_location("hpc_benchmark", BENCHMARK_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Failed to load benchmark module")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class RuntimeSurfaceContractTest(unittest.TestCase):
    def test_readme_does_not_advertise_nonexistent_python_shortcuts(self) -> None:
        readme = README_PATH.read_text(encoding="utf-8")

        self.assertNotIn("C = hpc_ai_opt.gemm(A, B)", readme)
        self.assertNotIn("hpc_ai_opt.last_tflops", readme)

    def test_localized_readme_does_not_advertise_nonexistent_shortcuts(self) -> None:
        readme = README_ZH_PATH.read_text(encoding="utf-8")

        self.assertNotIn("C = hpc_ai_opt.gemm(A, B)", readme)
        self.assertNotIn("hpc_ai_opt.last_tflops", readme)
        self.assertNotIn("python ../examples/python/basic_usage.py", readme)

    def test_readme_describes_current_python_binding_scope(self) -> None:
        readme = README_PATH.read_text(encoding="utf-8")

        self.assertIn(
            "Current Python bindings expose `elementwise`, `reduction`, and `gemm`.",
            readme,
        )

    def test_localized_readme_describes_current_python_binding_scope(self) -> None:
        readme = README_ZH_PATH.read_text(encoding="utf-8")

        self.assertIn("当前 Python 绑定暴露 `elementwise`、`reduction` 和 `gemm`。", readme)
        self.assertIn(
            "支持矩阵描述的是 C++/CUDA 核心能力。在当前阶段，Python 绑定只覆盖 `elementwise`、`reduction` 和 `gemm`。",
            readme,
        )

    def test_support_matrix_labels_core_vs_python_scope(self) -> None:
        readme = README_PATH.read_text(encoding="utf-8")

        self.assertIn(
            "The support matrix describes the C++/CUDA core. In this phase, Python bindings cover `elementwise`, `reduction`, and `gemm` only.",
            readme,
        )

    def test_readme_cpp_example_matches_current_tensor_and_gemm_api(self) -> None:
        readme = README_PATH.read_text(encoding="utf-8")
        localized_readme = README_ZH_PATH.read_text(encoding="utf-8")

        for content in (readme, localized_readme):
            self.assertNotIn("hpc::common::make_tensor<float>", content)
            self.assertNotIn("hpc::gemm::OptLevel::Advanced", content)
            self.assertIn("hpc::Tensor<float> A(M * K);", content)
            self.assertIn("hpc::gemm::GemmOpt::SharedMemTiling", content)

    def test_readme_gemm_examples_zero_initialize_output_buffers(self) -> None:
        readme = README_PATH.read_text(encoding="utf-8")
        localized_readme = README_ZH_PATH.read_text(encoding="utf-8")

        for content in (readme, localized_readme):
            self.assertIn('c = torch.zeros(128, 96, device="cuda", dtype=torch.float32)', content)
            self.assertNotIn('c = torch.empty(128, 96, device="cuda", dtype=torch.float32)', content)
            self.assertIn("C.zero();", content)

    def test_readmes_use_build_relative_python_example_path(self) -> None:
        readme = README_PATH.read_text(encoding="utf-8")
        localized_readme = README_ZH_PATH.read_text(encoding="utf-8")

        self.assertIn("python3 ../examples/python/basic_usage.py", readme)
        self.assertIn("python3 ../examples/python/basic_usage.py", localized_readme)
        self.assertNotIn("python3 examples/python/basic_usage.py", readme)
        self.assertNotIn("python3 examples/python/basic_usage.py", localized_readme)

    def test_benchmark_cli_parser_matches_current_supported_surface(self) -> None:
        benchmark = load_benchmark_module()
        parser = benchmark.build_argument_parser()
        suite_action = next(action for action in parser._actions if action.dest == "suite")
        help_text = parser.format_help()

        self.assertEqual(tuple(suite_action.choices), ("all", "gemm"))
        self.assertIn(
            "python3 python/benchmark/benchmark.py --suite gemm --roofline --chart",
            help_text,
        )
        self.assertNotIn("elementwise", help_text)
        self.assertNotIn("reduction", help_text)
        self.assertNotIn("attention", help_text)
        self.assertNotIn("--roofline --output roofline.png", help_text)

    def test_main_delegates_device_reporting_to_run_benchmark_cli(self) -> None:
        benchmark = load_benchmark_module()
        args = SimpleNamespace(
            suite="gemm",
            sizes="64",
            output=None,
            html=None,
            roofline=False,
            chart=False,
        )
        fake_parser = SimpleNamespace(parse_args=lambda: args)

        with (
            mock.patch.object(benchmark, "build_argument_parser", return_value=fake_parser),
            mock.patch.object(benchmark, "run_benchmark_cli") as run_benchmark_cli,
            mock.patch.object(
                benchmark,
                "get_device_info",
                side_effect=AssertionError("main should not query device info directly"),
            ),
            mock.patch("builtins.print") as mock_print,
        ):
            benchmark.main()

        run_benchmark_cli.assert_called_once_with(args)
        mock_print.assert_any_call("=" * 60)
        mock_print.assert_any_call("HPC-AI-Optimization-Lab Benchmark Framework")

    def test_gemm_header_describes_cutlass_baseline_as_implemented(self) -> None:
        header = GEMM_HEADER_PATH.read_text(encoding="utf-8")

        self.assertNotIn("not yet implemented", header)
        self.assertIn("CUTLASS-backed GEMM baseline", header)

    def test_bindings_require_contiguous_cuda_ndarrays(self) -> None:
        bindings = BINDINGS_PATH.read_text(encoding="utf-8")
        validation = VALIDATION_HEADER_PATH.read_text(encoding="utf-8")

        self.assertIn('#include "validation.hpp"', bindings)
        self.assertIn("must be contiguous in memory", validation)
        self.assertIn('std::to_string(expected_ndim) + "D CUDA ndarray"', validation)
        self.assertIn("validate_ndim(tensor, 2, name);", validation)
        self.assertIn("validate_ndim(tensor, 1, name);", validation)
        self.assertIn('validate_positive_product(batch, seq_len, "batch", "seq_len");', validation)
        self.assertIn(
            'validate_positive_product(batch, hidden_size, "batch", "hidden_size");',
            validation,
        )
        self.assertIn(".stride(", validation)
        self.assertIn(".ndim()", validation)

    def test_benchmark_kernel_docstring_scopes_to_overwrite_only_kernels(self) -> None:
        benchmark = load_benchmark_module()

        self.assertIn("overwrite-only", benchmark.benchmark_kernel.__doc__)

    def test_run_benchmark_cli_generates_roofline_before_html_report(self) -> None:
        benchmark = load_benchmark_module()
        args = SimpleNamespace(
            suite="gemm",
            sizes="64",
            output=None,
            html="report.html",
            roofline=True,
            chart=False,
        )
        fake_device = benchmark.DeviceInfo(
            name="Fake GPU",
            compute_capability=(9, 0),
            total_memory_gb=80.0,
            peak_fp32_tflops=60.0,
            peak_fp16_tflops=120.0,
            peak_bandwidth_gb_s=3000.0,
        )
        fake_result = benchmark.BenchmarkResult(
            kernel="gemm-64",
            hpc_ms=1.0,
            baseline_ms=2.0,
            speedup=2.0,
        )
        call_order = []

        fake_analyzer = SimpleNamespace(
            plot_roofline=lambda *args, **kwargs: call_order.append(("roofline", args))
        )

        with (
            mock.patch.object(benchmark, "print_results"),
            mock.patch.object(
                benchmark,
                "generate_html_report",
                side_effect=lambda *args, **kwargs: call_order.append(("html", args)),
            ),
            mock.patch.object(benchmark, "RooflineAnalyzer", return_value=fake_analyzer),
        ):
            benchmark.run_benchmark_cli(
                args,
                suite_runners={"gemm": lambda sizes: [fake_result]},
                device_info_provider=lambda: fake_device,
            )

        self.assertEqual([entry[0] for entry in call_order], ["roofline", "html"])

    def test_run_benchmark_cli_places_roofline_image_next_to_html_report(self) -> None:
        benchmark = load_benchmark_module()
        args = SimpleNamespace(
            suite="gemm",
            sizes="64",
            output=None,
            html=None,
            roofline=True,
            chart=False,
        )
        fake_device = benchmark.DeviceInfo(
            name="Fake GPU",
            compute_capability=(9, 0),
            total_memory_gb=80.0,
            peak_fp32_tflops=60.0,
            peak_fp16_tflops=120.0,
            peak_bandwidth_gb_s=3000.0,
        )
        fake_result = benchmark.BenchmarkResult(
            kernel="gemm-64",
            hpc_ms=1.0,
            baseline_ms=2.0,
            speedup=2.0,
        )
        cwd_roofline = pathlib.Path("roofline.png")

        class FakeAnalyzer:
            def plot_roofline(
                self,
                results,
                output_path: str = "roofline.png",
                title: str = "Roofline Analysis",
            ) -> None:
                pathlib.Path(output_path).write_bytes(b"fake-png")

        try:
            with tempfile.TemporaryDirectory() as tmpdir:
                report_path = pathlib.Path(tmpdir) / "reports" / "bench.html"
                report_path.parent.mkdir(parents=True, exist_ok=True)
                args.html = str(report_path)

                with (
                    mock.patch.object(benchmark, "print_results"),
                    mock.patch.object(benchmark, "RooflineAnalyzer", return_value=FakeAnalyzer()),
                ):
                    benchmark.run_benchmark_cli(
                        args,
                        suite_runners={"gemm": lambda sizes: [fake_result]},
                        device_info_provider=lambda: fake_device,
                    )

                self.assertTrue((report_path.parent / "roofline.png").exists())
                report_html = report_path.read_text(encoding="utf-8")
                self.assertIn('<img src="roofline.png" alt="Roofline Plot">', report_html)
        finally:
            if cwd_roofline.exists():
                cwd_roofline.unlink()

    def test_python_example_zero_initializes_gemm_output_buffer(self) -> None:
        example = PYTHON_EXAMPLE_PATH.read_text(encoding="utf-8")

        self.assertIn(
            'c = torch.zeros(m, n, device=device, dtype=torch.float32)',
            example,
        )
        self.assertNotIn(
            'c = torch.empty(m, n, device=device, dtype=torch.float32)',
            example,
        )

    def test_advanced_python_example_stays_within_shipped_float32_surface(self) -> None:
        example = ADVANCED_PYTHON_EXAMPLE_PATH.read_text(encoding="utf-8")

        self.assertNotIn("dtype=torch.float16", example)
        self.assertIn("Current shipped Python bindings accept float32 CUDA tensors only.", example)

    def test_supported_benchmark_suite_runs_injected_runner(self) -> None:
        benchmark = load_benchmark_module()

        results = benchmark.run_benchmark_suite(
            "gemm",
            [128],
            suite_runners={"gemm": lambda sizes: [("gemm", tuple(sizes))]},
        )

        self.assertEqual(results, [("gemm", (128,))])

    def test_unsupported_benchmark_suite_fails_clearly(self) -> None:
        benchmark = load_benchmark_module()

        with self.assertRaisesRegex(ValueError, "unsupported benchmark suite"):
            benchmark.run_benchmark_suite("attention", [128], suite_runners={})

    def test_run_benchmark_cli_executes_supported_suite(self) -> None:
        benchmark = load_benchmark_module()
        args = SimpleNamespace(
            suite="gemm",
            sizes="128,256",
            output=None,
            html=None,
            roofline=False,
            chart=False,
        )
        fake_device = benchmark.DeviceInfo(
            name="Fake GPU",
            compute_capability=(9, 0),
            total_memory_gb=80.0,
            peak_fp32_tflops=60.0,
            peak_fp16_tflops=120.0,
            peak_bandwidth_gb_s=3000.0,
        )
        fake_result = benchmark.BenchmarkResult(
            kernel="gemm",
            hpc_ms=1.0,
            baseline_ms=2.0,
            speedup=2.0,
            tflops=1.0,
        )

        with mock.patch.object(benchmark, "print_results") as print_results:
            results = benchmark.run_benchmark_cli(
                args,
                suite_runners={"gemm": lambda sizes: [fake_result]},
                device_info_provider=lambda: fake_device,
            )

        self.assertEqual(results, [fake_result])
        print_results.assert_called_once_with([fake_result], fake_device)

    def test_run_gemm_suite_benchmarks_each_requested_size(self) -> None:
        benchmark = load_benchmark_module()
        calls = []

        def fake_benchmark_fn(name, hpc_fn, baseline_fn, *args, **kwargs):
            calls.append((name, args, kwargs))
            return benchmark.BenchmarkResult(
                kernel=name,
                hpc_ms=1.0,
                baseline_ms=2.0,
                speedup=2.0,
                tflops=kwargs["flops"] / 1e9,
                bandwidth_gb_s=kwargs["bytes_accessed"] / 1e6,
            )

        def fake_tensor_factory(size):
            return f"A{size}", f"B{size}", f"C{size}"

        results = benchmark.run_gemm_suite(
            [64, 128],
            gemm_runner=lambda *args: None,
            baseline_runner=lambda *args: None,
            benchmark_fn=fake_benchmark_fn,
            tensor_factory=fake_tensor_factory,
        )

        self.assertEqual([result.kernel for result in results], ["gemm-64", "gemm-128"])
        self.assertEqual(len(calls), 2)
        self.assertEqual(calls[0][0], "gemm-64")
        self.assertEqual(calls[1][0], "gemm-128")
        self.assertEqual(calls[0][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))
        self.assertEqual(calls[1][1], ("A128", "B128", "C128", 128, 128, 128, 1.0, 0.0))

    def test_default_gemm_tensor_factory_zero_initializes_output_buffer(self) -> None:
        benchmark = load_benchmark_module()
        calls = []

        def fake_randn(*args, **kwargs):
            calls.append(("randn", args, kwargs))
            return f"randn-{len(calls)}"

        def fake_zeros(*args, **kwargs):
            calls.append(("zeros", args, kwargs))
            return "zero-c"

        with (
            mock.patch.object(benchmark.torch.cuda, "is_available", return_value=True),
            mock.patch.object(benchmark.torch, "randn", side_effect=fake_randn),
            mock.patch.object(benchmark.torch, "zeros", side_effect=fake_zeros),
            mock.patch.object(
                benchmark.torch,
                "empty",
                side_effect=AssertionError("default_gemm_tensor_factory must not use torch.empty"),
            ),
        ):
            a, b, c = benchmark.default_gemm_tensor_factory(64)

        self.assertEqual((a, b, c), ("randn-1", "randn-2", "zero-c"))
        self.assertEqual([call[0] for call in calls], ["randn", "randn", "zeros"])

    def test_build_gemm_suite_runner_uses_hpc_and_cutlass_paths(self) -> None:
        benchmark = load_benchmark_module()
        path_calls = []

        def fake_benchmark_fn(name, hpc_fn, baseline_fn, *args, **kwargs):
            hpc_fn(*args)
            baseline_fn(*args)
            return benchmark.BenchmarkResult(
                kernel=name,
                hpc_ms=1.0,
                baseline_ms=2.0,
                speedup=2.0,
            )

        def hpc_matmul(A, B, C, M, N, K, alpha, beta):
            path_calls.append(("hpc", (A, B, C, M, N, K, alpha, beta)))

        def cutlass_matmul(A, B, C, M, N, K, alpha, beta):
            path_calls.append(("cutlass", (A, B, C, M, N, K, alpha, beta)))

        fake_module = SimpleNamespace(
            gemm=SimpleNamespace(
                matmul=hpc_matmul,
                matmul_cutlass=cutlass_matmul,
            )
        )

        runner = benchmark.build_gemm_suite_runner(
            fake_module,
            benchmark_fn=fake_benchmark_fn,
            tensor_factory=lambda size: (f"A{size}", f"B{size}", f"C{size}"),
        )
        results = runner([64])

        self.assertEqual([result.kernel for result in results], ["gemm-64"])
        self.assertEqual([call[0] for call in path_calls], ["hpc", "cutlass"])
        self.assertEqual(path_calls[0][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))
        self.assertEqual(path_calls[1][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))

    def test_build_default_suite_runners_returns_gemm_runner(self) -> None:
        benchmark = load_benchmark_module()
        path_calls = []

        def fake_benchmark_fn(name, hpc_fn, baseline_fn, *args, **kwargs):
            hpc_fn(*args)
            baseline_fn(*args)
            return benchmark.BenchmarkResult(
                kernel=name,
                hpc_ms=1.0,
                baseline_ms=2.0,
                speedup=2.0,
            )

        def hpc_matmul(A, B, C, M, N, K, alpha, beta):
            path_calls.append(("hpc", (A, B, C, M, N, K, alpha, beta)))

        def cutlass_matmul(A, B, C, M, N, K, alpha, beta):
            path_calls.append(("cutlass", (A, B, C, M, N, K, alpha, beta)))

        fake_module = SimpleNamespace(
            gemm=SimpleNamespace(
                matmul=hpc_matmul,
                matmul_cutlass=cutlass_matmul,
            )
        )

        suite_runners = benchmark.build_default_suite_runners(
            fake_module,
            benchmark_fn=fake_benchmark_fn,
            tensor_factory=lambda size: (f"A{size}", f"B{size}", f"C{size}"),
        )

        self.assertIn("gemm", suite_runners)
        results = suite_runners["gemm"]([64])
        self.assertEqual([result.kernel for result in results], ["gemm-64"])
        self.assertEqual([call[0] for call in path_calls], ["hpc", "cutlass"])
        self.assertEqual(path_calls[0][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))
        self.assertEqual(path_calls[1][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))

    def test_build_default_suite_runners_requires_cutlass_baseline_binding(self) -> None:
        benchmark = load_benchmark_module()
        fake_module = SimpleNamespace(
            gemm=SimpleNamespace(
                matmul=lambda *args: None,
            )
        )

        with self.assertRaisesRegex(ValueError, "gemm\\.matmul_cutlass"):
            benchmark.build_default_suite_runners(
                fake_module,
                benchmark_fn=lambda *args, **kwargs: benchmark.BenchmarkResult(
                    kernel=args[0],
                    hpc_ms=1.0,
                    baseline_ms=2.0,
                    speedup=2.0,
                ),
                tensor_factory=lambda size: (f"A{size}", f"B{size}", f"C{size}"),
            )

    def test_run_benchmark_cli_builds_default_suite_runners(self) -> None:
        benchmark = load_benchmark_module()
        path_calls = []
        args = SimpleNamespace(
            suite="gemm",
            sizes="64",
            output=None,
            html=None,
            roofline=False,
            chart=False,
        )

        def hpc_matmul(A, B, C, M, N, K, alpha, beta):
            path_calls.append(("hpc", (A, B, C, M, N, K, alpha, beta)))

        def cutlass_matmul(A, B, C, M, N, K, alpha, beta):
            path_calls.append(("cutlass", (A, B, C, M, N, K, alpha, beta)))

        def fake_benchmark_fn(name, hpc_fn, baseline_fn, *args, **kwargs):
            hpc_fn(*args)
            baseline_fn(*args)
            return fake_result

        fake_module = SimpleNamespace(
            gemm=SimpleNamespace(
                matmul=hpc_matmul,
                matmul_cutlass=cutlass_matmul,
            )
        )
        fake_device = benchmark.DeviceInfo(
            name="Fake GPU",
            compute_capability=(9, 0),
            total_memory_gb=80.0,
            peak_fp32_tflops=60.0,
            peak_fp16_tflops=120.0,
            peak_bandwidth_gb_s=3000.0,
        )
        fake_result = benchmark.BenchmarkResult(
            kernel="gemm-64",
            hpc_ms=1.0,
            baseline_ms=2.0,
            speedup=2.0,
        )

        with mock.patch.object(benchmark, "print_results") as print_results:
            results = benchmark.run_benchmark_cli(
                args,
                suite_runners=None,
                device_info_provider=lambda: fake_device,
                hpc_module_loader=lambda: fake_module,
                benchmark_fn=fake_benchmark_fn,
                tensor_factory=lambda size: (f"A{size}", f"B{size}", f"C{size}"),
            )

        self.assertEqual(results, [fake_result])
        print_results.assert_called_once_with([fake_result], fake_device)
        self.assertEqual([call[0] for call in path_calls], ["hpc", "cutlass"])
        self.assertEqual(path_calls[0][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))
        self.assertEqual(path_calls[1][1], ("A64", "B64", "C64", 64, 64, 64, 1.0, 0.0))


if __name__ == "__main__":
    unittest.main()

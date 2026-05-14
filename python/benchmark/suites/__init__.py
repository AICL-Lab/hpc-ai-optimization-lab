# Benchmark suite adapters
#
# Each module in this package exposes a suite-specific adapter that maps
# the HPC Python binding surface to the generic benchmark harness interface.
#
# To add a new suite:
#   1. Create a module here (e.g. my_suite.py)
#   2. Export a build_*_suite_runner function
#   3. Register it in benchmark.py::build_default_suite_runners

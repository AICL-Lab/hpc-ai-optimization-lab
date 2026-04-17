# API Specifications

This directory contains API interface definitions for the HPC-AI-Optimization-Lab project.

## Purpose

API specifications define the contract between modules and external consumers. These specifications serve as the single source of truth for:

- Function signatures
- Parameter types and constraints
- Return value specifications
- Error codes and handling

## Current Status

API specifications for this project are embedded within the source code headers (`.cuh` files) and documented in:

- [API Reference (English)](../../docs/en/API_REFERENCE.md)
- [API Reference (Chinese)](../../docs/zh-CN/API_REFERENCE.md)

## Future Enhancements

As the project grows, formal API specifications may be added here:

- `openapi.yaml` - REST API definitions (if web services are added)
- `cuda_api.md` - CUDA kernel API formal specification
- `python_api.md` - Python binding API specification

## Guidelines

When adding API specifications:

1. Use machine-readable formats (OpenAPI, AsyncAPI, etc.) when possible
2. Include examples for all endpoints/functions
3. Document all error conditions
4. Keep specifications in sync with implementation

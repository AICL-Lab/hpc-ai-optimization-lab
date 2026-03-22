# Python bindings using nanobind
if(BUILD_PYTHON_BINDINGS)
    nanobind_add_module(hpc_ai_opt
        bindings/bindings.cpp
    )

    target_link_libraries(hpc_ai_opt PRIVATE
        hpc_elementwise
        hpc_reduction
        hpc_gemm
        hpc_attention
    )

    target_include_directories(hpc_ai_opt PRIVATE ${CMAKE_SOURCE_DIR}/src)
    set_target_properties(hpc_ai_opt PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python
    )

    install(TARGETS hpc_ai_opt LIBRARY DESTINATION .)
endif()

add_library(nuklear_sfml
    nuklear_sfml_gl2.cpp
    demos.cpp
    nuklear_impl.c
)
target_compile_options(nuklear_sfml PRIVATE -fPIE)	
# Junky cmake script to drive test matrix.  Ideally would do this inside the
# main build, but cmake 2.8 just isn't up to it.

set (build_base "build")

set (source_dir ${CMAKE_CURRENT_LIST_DIR})

set (anyfail FALSE)

foreach (compiler g++ clang++)
    foreach (build_type Release Debug)
        foreach (cxx_std c++98 c++11)
            message (STATUS "Build and test with ${compiler}, ${cxx_std}, ${build_type}")
            set(build_dir "${build_base}/${compiler}-${cxx_std}-${build_type}")
            file(MAKE_DIRECTORY ${build_dir})
            execute_process(COMMAND ${CMAKE_COMMAND}
                -DCMAKE_CXX_COMPILER=${compiler} -DCMAKE_BUILD_TYPE=${build_type} -DCXX_STD=${cxx_std}
                ${source_dir}
                WORKING_DIRECTORY ${build_dir}
                OUTPUT_QUIET
            )
            execute_process(COMMAND ${CMAKE_COMMAND}
                --build . --target testall
                WORKING_DIRECTORY ${build_dir}
                RESULT_VARIABLE res
            )
            if (res)
                set (anyfail TRUE)
            endif()
        endforeach()
    endforeach()
endforeach()

if (anyfail)
    message (FATAL_ERROR "Test failed")
endif()

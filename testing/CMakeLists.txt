## Copyright 2009-2022 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

#PROJECT(rthwif_testing)
#CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

SET(CMAKE_CXX_STANDARD 17)

# create and install abn test file
SET(ABN_TEST_FILE "${CMAKE_BINARY_DIR}/abn_metadata.json")
FILE(WRITE "${ABN_TEST_FILE}" "")
FUNCTION(ADD_ABN_TEST NAME COMMAND CHECK)
  FILE(APPEND "${ABN_TEST_FILE}" "{\n  \"${NAME}\": {\n    \"api\": \"L0\",\n    \"commandLine\": \"${COMMAND}\",\n    \"functionalCheck\": [\"${CHECK}\"]\n  }\n}\n")
ENDFUNCTION()

IF (WIN32)
  ADD_ABN_TEST("level-zero-gpu-raytracing-test" "run.bat" "0 tests failed")
ELSE()
  ADD_ABN_TEST("level-zero-gpu-raytracing-test" "run.sh" "0 tests failed")
ENDIF()

INSTALL(FILES "${ABN_TEST_FILE}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." COMPONENT test)

SET(BAT_TEST_FILE_EXP "${CMAKE_BINARY_DIR}/run.bat")
SET(SHELL_TEST_FILE_EXP "${CMAKE_BINARY_DIR}/run.sh")

SET(BAT_TEST_FILE_EXT "${CMAKE_BINARY_DIR}/run_ext.bat")
SET(SHELL_TEST_FILE_EXT "${CMAKE_BINARY_DIR}/run_ext.sh")

# creation of windows batch test file
FUNCTION(INIT_BAT_TEST BAT_TEST_FILE)
  FILE(WRITE "${BAT_TEST_FILE}" "@echo off\n")
  FILE(APPEND "${BAT_TEST_FILE}" "set errors=0\n")
  FILE(APPEND "${BAT_TEST_FILE}" "cd bin\n")
  FILE(APPEND "${BAT_TEST_FILE}" "\n")
ENDFUNCTION()

FUNCTION(ADD_BAT_TEST BAT_TEST_FILE)
  CMAKE_PARSE_ARGUMENTS(BAT_TEST "" "NAME" "COMMAND" ${ARGN})
  LIST(POP_FRONT BAT_TEST_COMMAND CMD)
  LIST(PREPEND BAT_TEST_COMMAND "${CMD}.exe")
  STRING(REPLACE ";" " " BAT_TEST_COMMAND_STR "${BAT_TEST_COMMAND}")
  FILE(APPEND "${BAT_TEST_FILE}" "echo Executing test ${BAT_TEST_NAME}\n")
  FILE(APPEND "${BAT_TEST_FILE}" "echo ${BAT_TEST_COMMAND_STR}\n")
  FILE(APPEND "${BAT_TEST_FILE}" "${BAT_TEST_COMMAND_STR}\n")
  FILE(APPEND "${BAT_TEST_FILE}" "if %ERRORLEVEL% equ 0 (\n  echo Test ${BAT_TEST_NAME} passed!\n")
  FILE(APPEND "${BAT_TEST_FILE}" ") else (\n  echo Test ${BAT_TEST_NAME} failed!\n  set /a errors=errors+1\n)\n\n")
  FILE(APPEND "${BAT_TEST_FILE}" "echo:\n")
  FILE(APPEND "${BAT_TEST_FILE}" "echo:\n")
  FILE(APPEND "${BAT_TEST_FILE}" "\n")
ENDFUNCTION()

FUNCTION(FINALIZE_BAT_TEST BAT_TEST_FILE)
  FILE(APPEND "${BAT_TEST_FILE}" "cd ..\n")
  FILE(APPEND "${BAT_TEST_FILE}" "echo %errors% tests failed\n")
  FILE(APPEND "${BAT_TEST_FILE}" "if %errors% neq 0 exit /b 1\n")
ENDFUNCTION()

# creation of Linux batch test file
FUNCTION(INIT_SHELL_TEST SHELL_TEST_FILE)
  FILE(WRITE "${SHELL_TEST_FILE}" "#!/bin/bash\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "errors=0\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "cd bin\n")
  GET_FILENAME_COMPONENT(DPCPP_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME_WE)
  IF (DPCPP_COMPILER_NAME STREQUAL "icpx")
    FILE(APPEND "${SHELL_TEST_FILE}" "export LD_LIBRARY_PATH=\$\{LD_LIBRARY_PATH\}:.\n")  # required as workaround for ICX
  ENDIF()
  FILE(APPEND "${SHELL_TEST_FILE}" "\n")
ENDFUNCTION()

FUNCTION(ADD_SHELL_TEST SHELL_TEST_FILE)
  CMAKE_PARSE_ARGUMENTS(SHELL_TEST "" "NAME" "COMMAND" ${ARGN})
  LIST(POP_FRONT SHELL_TEST_COMMAND CMD)
  LIST(PREPEND SHELL_TEST_COMMAND "./${CMD}")
  STRING(REPLACE ";" " " SHELL_TEST_COMMAND_STR "${SHELL_TEST_COMMAND}")
  FILE(APPEND "${SHELL_TEST_FILE}" "echo Executing test ${SHELL_TEST_NAME}\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "echo ${SHELL_TEST_COMMAND_STR}\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "${SHELL_TEST_COMMAND_STR}\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "if (($? == 0)); then\n  echo Test ${SHELL_TEST_NAME} passed!\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "else\n  echo Test ${SHELL_TEST_NAME} failed!\n  ((errors++))\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "fi\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "echo\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "echo\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "\n")
ENDFUNCTION()

FUNCTION(FINALIZE_SHELL_TEST SHELL_TEST_FILE)
  FILE(APPEND "${SHELL_TEST_FILE}" "cd ..\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "echo $errors tests failed\n")
  FILE(APPEND "${SHELL_TEST_FILE}" "if (($errors != 0)); then exit 1; fi\n")
ENDFUNCTION()

# install windows batch test file or linux shell script
IF (WIN32)
  INSTALL(FILES "${BAT_TEST_FILE_EXP}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." COMPONENT test)
  INSTALL(FILES "${BAT_TEST_FILE_EXT}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." COMPONENT test)
ELSE()
  INSTALL(FILES "${SHELL_TEST_FILE_EXP}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." COMPONENT test
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  INSTALL(FILES "${SHELL_TEST_FILE_EXT}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." COMPONENT test
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
ENDIF()

# install ctest file
SET(CTEST_TEST_FILE_ROOT "${CMAKE_BINARY_DIR}/CTestTestfile.root")
file(WRITE "${CTEST_TEST_FILE_ROOT}" "# CTestTestfile.cmake self generated for package testing\n")
file(APPEND "${CTEST_TEST_FILE_ROOT}" "subdirs(\"bin\")\n")
INSTALL(FILES "${CTEST_TEST_FILE_ROOT}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." RENAME "CTestTestfile.cmake" COMPONENT test)

SET(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
SET(CTEST_TEST_FILE "${CMAKE_BINARY_DIR}/CTestTestfile.install")
file(WRITE "${CTEST_TEST_FILE}" "# CTestTestfile.cmake self generated for package testing\n")

FUNCTION(ADD_CTEST_TEST)
  CMAKE_PARSE_ARGUMENTS(ADD_CTEST_TEST "" "NAME" "COMMAND" ${ARGN})
  STRING(REPLACE ";" " " ADD_CTEST_TEST_COMMAND_STR "${ADD_CTEST_TEST_COMMAND}")
  FILE(APPEND "${CTEST_TEST_FILE}" "add_test(${ADD_CTEST_TEST_NAME} ./${ADD_CTEST_TEST_COMMAND_STR})\n")
  ADD_TEST(NAME ${ADD_CTEST_TEST_NAME} COMMAND ${ADD_CTEST_TEST_COMMAND} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ENDFUNCTION()

FUNCTION(MY_ADD_TEST)
  ADD_CTEST_TEST(${ARGN})
  ADD_BAT_TEST(${BAT_TEST_FILE_EXP} ${ARGN})
  ADD_SHELL_TEST(${SHELL_TEST_FILE_EXP} ${ARGN})
ENDFUNCTION()

FUNCTION(MY_ADD_TEST_EXT)
#  ADD_CTEST_TEST(${ARGN})
  ADD_BAT_TEST(${BAT_TEST_FILE_EXT} ${ARGN})
  ADD_SHELL_TEST(${SHELL_TEST_FILE_EXT} ${ARGN})
ENDFUNCTION()

GET_FILENAME_COMPONENT(SYCL_COMPILER_DIR ${CMAKE_CXX_COMPILER} PATH)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem \"${SYCL_COMPILER_DIR}/../include/sycl\" -isystem \"${SYCL_COMPILER_DIR}/../include/\"")       # disable warning from SYCL header (FIXME: why required?)

IF (ZE_RAYTRACING_RT_SIMULATION)
  SET(RT_SIM_LIBRARY rtcore)
  ADD_COMPILE_DEFINITIONS(ZE_RAYTRACING_RT_SIMULATION)
ENDIF()

ADD_EXECUTABLE(embree_rthwif_cornell_box rthwif_cornell_box.cpp)
TARGET_LINK_LIBRARIES(embree_rthwif_cornell_box sys simd tbb ze_wrapper ${RT_SIM_LIBRARY} ${EMBREE_RTHWIF_SYCL})
SET_PROPERTY(TARGET embree_rthwif_cornell_box APPEND PROPERTY COMPILE_FLAGS "-fsycl -fsycl-targets=spir64")
SET_PROPERTY(TARGET embree_rthwif_cornell_box APPEND PROPERTY LINK_FLAGS "-fsycl -fsycl-targets=spir64 -Xsycl-target-backend=spir64 \" -cl-intel-greater-than-4GB-buffer-required \"")
TARGET_COMPILE_DEFINITIONS(embree_rthwif_cornell_box PUBLIC EMBREE_LEVEL_ZERO ZE_RAYTRACING)
ADD_CUSTOM_COMMAND(TARGET embree_rthwif_cornell_box POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/cornell_box_reference.tga" "$<TARGET_FILE_DIR:embree_rthwif_cornell_box>")

ADD_EXECUTABLE(embree_rthwif_test rthwif_test.cpp)
TARGET_LINK_LIBRARIES(embree_rthwif_test sys simd tbb ze_wrapper ${RT_SIM_LIBRARY}  ${EMBREE_RTHWIF_SYCL})
SET_PROPERTY(TARGET embree_rthwif_test APPEND PROPERTY COMPILE_FLAGS "-fsycl -fsycl-targets=spir64")
SET_PROPERTY(TARGET embree_rthwif_test APPEND PROPERTY LINK_FLAGS "-fsycl -fsycl-targets=spir64 -Xsycl-target-backend=spir64 \" -cl-intel-greater-than-4GB-buffer-required \"")
TARGET_COMPILE_DEFINITIONS(embree_rthwif_test PUBLIC EMBREE_LEVEL_ZERO ZE_RAYTRACING)

ADD_EXECUTABLE(embree_rthwif_cornell_box_ext rthwif_cornell_box_ext.cpp)
TARGET_LINK_LIBRARIES(embree_rthwif_cornell_box_ext sys simd tbb ze_wrapper ${RT_SIM_LIBRARY} ${EMBREE_RTHWIF_SYCL})
SET_PROPERTY(TARGET embree_rthwif_cornell_box_ext APPEND PROPERTY COMPILE_FLAGS "-fsycl -fsycl-targets=spir64")
SET_PROPERTY(TARGET embree_rthwif_cornell_box_ext APPEND PROPERTY LINK_FLAGS "-fsycl -fsycl-targets=spir64 -Xsycl-target-backend=spir64 \" -cl-intel-greater-than-4GB-buffer-required \"")
TARGET_COMPILE_DEFINITIONS(embree_rthwif_cornell_box_ext PUBLIC EMBREE_LEVEL_ZERO ZE_RAYTRACING)
#ADD_CUSTOM_COMMAND(TARGET embree_rthwif_cornell_box_ext POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/cornell_box_reference.tga" "$<TARGET_FILE_DIR:embree_rthwif_cornell_box_ext>")

ADD_EXECUTABLE(embree_rthwif_test_ext rthwif_test_ext.cpp)
TARGET_LINK_LIBRARIES(embree_rthwif_test_ext sys simd tbb ze_wrapper ${RT_SIM_LIBRARY}  ${EMBREE_RTHWIF_SYCL})
SET_PROPERTY(TARGET embree_rthwif_test_ext APPEND PROPERTY COMPILE_FLAGS "-fsycl -fsycl-targets=spir64")
SET_PROPERTY(TARGET embree_rthwif_test_ext APPEND PROPERTY LINK_FLAGS "-fsycl -fsycl-targets=spir64 -Xsycl-target-backend=spir64 \" -cl-intel-greater-than-4GB-buffer-required \"")
TARGET_COMPILE_DEFINITIONS(embree_rthwif_test_ext PUBLIC EMBREE_LEVEL_ZERO ZE_RAYTRACING)

IF (ZE_RAYTRACING_SYCL_TESTS STREQUAL "INTERNAL_RTAS_BUILDER")
  SET(RTAS_BUILDER_MODE "--internal-rtas-builder")
ELSEIF (ZE_RAYTRACING_SYCL_TESTS STREQUAL "LEVEL_ZERO_RTAS_BUILDER")
  SET(RTAS_BUILDER_MODE "--level-zero-rtas-builder")
ELSE()
  MESSAGE(FATAL_ERROR "invalid test mode")
ENDIF()

# EXP API version tests
INIT_BAT_TEST(${BAT_TEST_FILE_EXP})
INIT_SHELL_TEST(${SHELL_TEST_FILE_EXP})

MY_ADD_TEST(NAME rthwif_cornell_box COMMAND embree_rthwif_cornell_box ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga)

MY_ADD_TEST(NAME rthwif_test_builder_triangles_expected      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_expected)
MY_ADD_TEST(NAME rthwif_test_builder_procedurals_expected    COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_expected)
MY_ADD_TEST(NAME rthwif_test_builder_instances_expected      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_expected)
MY_ADD_TEST(NAME rthwif_test_builder_mixed_expected          COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_expected)

MY_ADD_TEST(NAME rthwif_test_benchmark_triangles             COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --benchmark_triangles)
MY_ADD_TEST(NAME rthwif_test_benchmark_procedurals           COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --benchmark_procedurals)

MY_ADD_TEST(NAME rthwif_test_builder_triangles_worst_case      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_worst_case)
MY_ADD_TEST(NAME rthwif_test_builder_procedurals_worst_case    COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_worst_case)
MY_ADD_TEST(NAME rthwif_test_builder_instances_worst_case      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_worst_case)
MY_ADD_TEST(NAME rthwif_test_builder_mixed_worst_case          COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_worst_case)

MY_ADD_TEST(NAME rthwif_test_triangles_committed_hit        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-committed-hit)
MY_ADD_TEST(NAME rthwif_test_triangles_potential_hit        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-potential-hit)
MY_ADD_TEST(NAME rthwif_test_triangles_anyhit_shader_commit COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST(NAME rthwif_test_triangles_anyhit_shader_reject COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST(NAME rthwif_test_procedurals_committed_hit      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --procedurals-committed-hit)

MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_committed_hit        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-committed-hit)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_potential_hit        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-potential-hit)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_anyhit_shader_commit COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_anyhit_shader_reject COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_procedurals_committed_hit      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --procedurals-committed-hit)

MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_committed_hit        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-committed-hit)
MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_potential_hit        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-potential-hit)
MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_anyhit_shader_commit COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_anyhit_shader_reject COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST(NAME rthwif_test_swinstancing_procedurals_committed_hit      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --procedurals-committed-hit)

FINALIZE_BAT_TEST(${BAT_TEST_FILE_EXP})
FINALIZE_SHELL_TEST(${SHELL_TEST_FILE_EXP})

# EXT API version tests
INIT_BAT_TEST(${BAT_TEST_FILE_EXT})
INIT_SHELL_TEST(${SHELL_TEST_FILE_EXT})

MY_ADD_TEST_EXT(NAME rthwif_cornell_box_ext COMMAND embree_rthwif_cornell_box_ext ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga)

MY_ADD_TEST_EXT(NAME rthwif_test_builder_triangles_expected_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_expected)
MY_ADD_TEST_EXT(NAME rthwif_test_builder_procedurals_expected_ext    COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_expected)
MY_ADD_TEST_EXT(NAME rthwif_test_builder_instances_expected_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_expected)
MY_ADD_TEST_EXT(NAME rthwif_test_builder_mixed_expected_ext          COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_expected)

MY_ADD_TEST_EXT(NAME rthwif_test_benchmark_triangles_ext             COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --benchmark_triangles)
MY_ADD_TEST_EXT(NAME rthwif_test_benchmark_procedurals_ext           COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --benchmark_procedurals)

MY_ADD_TEST_EXT(NAME rthwif_test_builder_triangles_worst_case_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_worst_case)
MY_ADD_TEST_EXT(NAME rthwif_test_builder_procedurals_worst_case_ext    COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_worst_case)
MY_ADD_TEST_EXT(NAME rthwif_test_builder_instances_worst_case_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_worst_case)
MY_ADD_TEST_EXT(NAME rthwif_test_builder_mixed_worst_case_ext          COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_worst_case)

MY_ADD_TEST_EXT(NAME rthwif_test_triangles_committed_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-committed-hit)
MY_ADD_TEST_EXT(NAME rthwif_test_triangles_potential_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-potential-hit)
MY_ADD_TEST_EXT(NAME rthwif_test_triangles_anyhit_shader_commit_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST_EXT(NAME rthwif_test_triangles_anyhit_shader_reject_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST_EXT(NAME rthwif_test_procedurals_committed_hit_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --procedurals-committed-hit)

MY_ADD_TEST_EXT(NAME rthwif_test_hwinstancing_triangles_committed_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-committed-hit)
MY_ADD_TEST_EXT(NAME rthwif_test_hwinstancing_triangles_potential_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-potential-hit)
MY_ADD_TEST_EXT(NAME rthwif_test_hwinstancing_triangles_anyhit_shader_commit_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST_EXT(NAME rthwif_test_hwinstancing_triangles_anyhit_shader_reject_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST_EXT(NAME rthwif_test_hwinstancing_procedurals_committed_hit_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --procedurals-committed-hit)

MY_ADD_TEST_EXT(NAME rthwif_test_swinstancing_triangles_committed_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-committed-hit)
MY_ADD_TEST_EXT(NAME rthwif_test_swinstancing_triangles_potential_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-potential-hit)
MY_ADD_TEST_EXT(NAME rthwif_test_swinstancing_triangles_anyhit_shader_commit_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST_EXT(NAME rthwif_test_swinstancing_triangles_anyhit_shader_reject_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST_EXT(NAME rthwif_test_swinstancing_procedurals_committed_hit_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --procedurals-committed-hit)

FINALIZE_BAT_TEST(${BAT_TEST_FILE_EXT})
FINALIZE_SHELL_TEST(${SHELL_TEST_FILE_EXT})

INSTALL(TARGETS embree_rthwif_cornell_box RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)
INSTALL(TARGETS embree_rthwif_cornell_box_ext RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)
INSTALL(FILES   "${CMAKE_CURRENT_SOURCE_DIR}/cornell_box_reference.tga" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)

INSTALL(TARGETS embree_rthwif_test RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)
INSTALL(TARGETS embree_rthwif_test_ext RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)

INSTALL(FILES   "${CTEST_TEST_FILE}" DESTINATION "${CMAKE_INSTALL_BINDIR}" RENAME "CTestTestfile.cmake" COMPONENT test)

##############################################################
# Install SYCL specific files
##############################################################

IF (ZE_RAYTRACING_SYCL_TESTS)

  GET_FILENAME_COMPONENT(DPCPP_COMPILER_DIR ${CMAKE_CXX_COMPILER} PATH)
  GET_FILENAME_COMPONENT(DPCPP_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME_WE)
  
  IF (WIN32)
    
    FILE(GLOB_RECURSE LIB_SYCL_LIB_FILES "${DPCPP_COMPILER_DIR}/../lib/sycl?.lib")
    IF (NOT LIB_SYCL_LIB_FILES)
      SET(LIB_SYCL_LIB_FILES "${DPCPP_COMPILER_DIR}/../lib/sycl?.lib")
    ENDIF()
    INSTALL(FILES ${LIB_SYCL_LIB_FILES} DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib)

    FILE(GLOB_RECURSE LIB_SYCL_DLL_FILES "${DPCPP_COMPILER_DIR}/../bin/sycl?.dll")
    IF (NOT LIB_SYCL_DLL_FILES)
      SET(LIB_SYCL_DLL_FILES "${DPCPP_COMPILER_DIR}/../bin/sycl?.dll")
    ENDIF()
    INSTALL(FILES ${LIB_SYCL_DLL_FILES} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
    
    INSTALL(FILES "${DPCPP_COMPILER_DIR}/../bin/pi_level_zero.dll" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
    INSTALL(FILES "${DPCPP_COMPILER_DIR}/../bin/pi_win_proxy_loader.dll" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

  ELSE()

    IF (DPCPP_COMPILER_NAME STREQUAL "clang++")
    
      FILE(GLOB_RECURSE LIB_SYCL_FILES "${DPCPP_COMPILER_DIR}/../lib/libsycl.so.*")
      INSTALL(FILES ${LIB_SYCL_FILES} DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libpi_level_zero.so" DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lib)
      
    ELSE()

      FILE(GLOB_RECURSE LIB_SYCL_FILES "${DPCPP_COMPILER_DIR}/../lib/libsycl.so.*")
      INSTALL(FILES ${LIB_SYCL_FILES} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

      FILE(GLOB_RECURSE LIB_URLOADER_FILES "${DPCPP_COMPILER_DIR}/../lib/libur_loader.so.*")
      INSTALL(FILES ${LIB_URLOADER_FILES} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

      FILE(GLOB_RECURSE LIB_URADAPTER_FILES "${DPCPP_COMPILER_DIR}/../lib/libur_adapter_level_zero.so.*")
      INSTALL(FILES ${LIB_URADAPTER_FILES} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

      FILE(GLOB_RECURSE LIB_UMF_FILES "${DPCPP_COMPILER_DIR}/../lib/libumf.so.*")
      INSTALL(FILES ${LIB_UMF_FILES} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libsvml.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libirng.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libimf.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libintlc.so.5" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libiomp5.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

    ENDIF()
  ENDIF()
      
ENDIF()

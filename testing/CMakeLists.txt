## Copyright 2009-2022 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

#PROJECT(rthwif_testing)
#CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

SET(CMAKE_CXX_STANDARD 17)

SET(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
SET(CTEST_TEST_FILE "${CMAKE_BINARY_DIR}/CTestTestfile.install")
file(WRITE "${CTEST_TEST_FILE}" "# CTestTestfile.cmake self generated for package testing\n")

MACRO(MY_ADD_TEST)
  CMAKE_PARSE_ARGUMENTS(MY_ADD_TEST "" "NAME;WORKING_DIRECTORY" "COMMAND" ${ARGN})
  STRING(REPLACE ";" " " MY_ADD_TEST_COMMAND_STR "${MY_ADD_TEST_COMMAND}")
  FILE(APPEND "${CTEST_TEST_FILE}" "add_test(${MY_ADD_TEST_NAME} ./${MY_ADD_TEST_COMMAND_STR})\n")
  #FILE(APPEND "${CTEST_TEST_FILE}" "set_tests_properties(${MY_ADD_TEST_NAME} PROPERTIES WORKING_DIRECTORY \"${MY_ADD_TEST_WORKING_DIRECTORY}\")\n")
  ADD_TEST(NAME ${MY_ADD_TEST_NAME} COMMAND ${MY_ADD_TEST_COMMAND} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ENDMACRO()

GET_FILENAME_COMPONENT(SYCL_COMPILER_DIR ${CMAKE_CXX_COMPILER} PATH)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem \"${SYCL_COMPILER_DIR}/../include/sycl\" -isystem \"${SYCL_COMPILER_DIR}/../include/\"")       # disable warning from SYCL header (FIXME: why required?)

ADD_COMPILE_DEFINITIONS(EMBREE_SYCL_SUPPORT)

IF (ZE_RAYTRACING_RT_SIMULATION)
  SET(RT_SIM_LIBRARY rtcore)
  ADD_COMPILE_DEFINITIONS(ZE_RAYTRACING_RT_SIMULATION)
ENDIF()

ADD_EXECUTABLE(embree_rthwif_cornell_box rthwif_cornell_box.cpp)
TARGET_LINK_LIBRARIES(embree_rthwif_cornell_box embree_rthwif sys simd tbb ${RT_SIM_LIBRARY})
SET_PROPERTY(TARGET embree_rthwif_cornell_box APPEND PROPERTY COMPILE_FLAGS "-fsycl -fsycl-targets=spir64")
SET_PROPERTY(TARGET embree_rthwif_cornell_box APPEND PROPERTY LINK_FLAGS "-fsycl -lze_loader -fsycl-targets=spir64 -Xsycl-target-backend=spir64 \" -cl-intel-greater-than-4GB-buffer-required \"")
TARGET_COMPILE_DEFINITIONS(embree_rthwif_cornell_box PUBLIC EMBREE_LEVEL_ZERO)
ADD_CUSTOM_COMMAND(TARGET embree_rthwif_cornell_box POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/cornell_box_reference.tga" "$<TARGET_FILE_DIR:embree_rthwif_cornell_box>")

ADD_EXECUTABLE(embree_rthwif_test rthwif_test.cpp)
TARGET_LINK_LIBRARIES(embree_rthwif_test embree_rthwif sys simd tbb ${RT_SIM_LIBRARY})
SET_PROPERTY(TARGET embree_rthwif_test APPEND PROPERTY COMPILE_FLAGS "-fsycl -fsycl-targets=spir64")
SET_PROPERTY(TARGET embree_rthwif_test APPEND PROPERTY LINK_FLAGS "-fsycl -lze_loader -fsycl-targets=spir64 -Xsycl-target-backend=spir64 \" -cl-intel-greater-than-4GB-buffer-required \"")
TARGET_COMPILE_DEFINITIONS(embree_rthwif_test PUBLIC EMBREE_LEVEL_ZERO)

MY_ADD_TEST(NAME rthwif_cornell_box COMMAND embree_rthwif_cornell_box --compare cornell_box_reference.tga)

MY_ADD_TEST(NAME rthwif_test_builder_triangles_expected      COMMAND embree_rthwif_test --build_test_triangles   --build_mode_expected)
MY_ADD_TEST(NAME rthwif_test_builder_procedurals_expected    COMMAND embree_rthwif_test --build_test_procedurals --build_mode_expected)
MY_ADD_TEST(NAME rthwif_test_builder_instances_expected      COMMAND embree_rthwif_test --build_test_instances   --build_mode_expected)
MY_ADD_TEST(NAME rthwif_test_builder_mixed_expected          COMMAND embree_rthwif_test --build_test_mixed       --build_mode_expected)

MY_ADD_TEST(NAME rthwif_test_benchmark_triangles             COMMAND embree_rthwif_test --benchmark_triangles)
MY_ADD_TEST(NAME rthwif_test_benchmark_procedurals           COMMAND embree_rthwif_test --benchmark_procedurals)

MY_ADD_TEST(NAME rthwif_test_builder_triangles_worst_case      COMMAND embree_rthwif_test --build_test_triangles   --build_mode_worst_case)
MY_ADD_TEST(NAME rthwif_test_builder_procedurals_worst_case    COMMAND embree_rthwif_test --build_test_procedurals --build_mode_worst_case)
MY_ADD_TEST(NAME rthwif_test_builder_instances_worst_case      COMMAND embree_rthwif_test --build_test_instances   --build_mode_worst_case)
MY_ADD_TEST(NAME rthwif_test_builder_mixed_worst_case          COMMAND embree_rthwif_test --build_test_mixed       --build_mode_worst_case)

MY_ADD_TEST(NAME rthwif_test_triangles_committed_hit        COMMAND embree_rthwif_test --no-instancing --triangles-committed-hit)
MY_ADD_TEST(NAME rthwif_test_triangles_potential_hit        COMMAND embree_rthwif_test --no-instancing --triangles-potential-hit)
MY_ADD_TEST(NAME rthwif_test_triangles_anyhit_shader_commit COMMAND embree_rthwif_test --no-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST(NAME rthwif_test_triangles_anyhit_shader_reject COMMAND embree_rthwif_test --no-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST(NAME rthwif_test_procedurals_committed_hit      COMMAND embree_rthwif_test --no-instancing --procedurals-committed-hit)

MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_committed_hit        COMMAND embree_rthwif_test --hw-instancing --triangles-committed-hit)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_potential_hit        COMMAND embree_rthwif_test --hw-instancing --triangles-potential-hit)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_anyhit_shader_commit COMMAND embree_rthwif_test --hw-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_triangles_anyhit_shader_reject COMMAND embree_rthwif_test --hw-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST(NAME rthwif_test_hwinstancing_procedurals_committed_hit      COMMAND embree_rthwif_test --hw-instancing --procedurals-committed-hit)

MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_committed_hit        COMMAND embree_rthwif_test --sw-instancing --triangles-committed-hit)
MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_potential_hit        COMMAND embree_rthwif_test --sw-instancing --triangles-potential-hit)
MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_anyhit_shader_commit COMMAND embree_rthwif_test --sw-instancing --triangles-anyhit-shader-commit)
MY_ADD_TEST(NAME rthwif_test_swinstancing_triangles_anyhit_shader_reject COMMAND embree_rthwif_test --sw-instancing --triangles-anyhit-shader-reject)
MY_ADD_TEST(NAME rthwif_test_swinstancing_procedurals_committed_hit      COMMAND embree_rthwif_test --sw-instancing --procedurals-committed-hit)

INSTALL(TARGETS embree_rthwif_cornell_box RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)
INSTALL(FILES   "${CMAKE_CURRENT_SOURCE_DIR}/cornell_box_reference.tga" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)

INSTALL(TARGETS embree_rthwif_test RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test)

INSTALL(FILES   "${CTEST_TEST_FILE}" DESTINATION "${CMAKE_INSTALL_BINDIR}" RENAME "CTestTestfile.cmake" COMPONENT test)

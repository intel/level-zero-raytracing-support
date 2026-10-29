## Copyright 2009-2022 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

#PROJECT(rthwif_testing)
#CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

SET(CMAKE_CXX_STANDARD 17)

# create and install abn test file
SET(ABN_TEST_FILE "${CMAKE_BINARY_DIR}/abn_metadata.json")
FILE(WRITE "${ABN_TEST_FILE}" "{\n")
SET(ABN_TEST_FIRST "ON")
FUNCTION(WRITE_ABN_TEST)
  CMAKE_PARSE_ARGUMENTS(ABN_TEST "" "NAME;COMMAND;CHECK;DIR" "" ${ARGN})
  IF (NOT ABN_TEST_FIRST)
    FILE(APPEND "${ABN_TEST_FILE}" ",\n")
  ENDIF()
  FILE(APPEND "${ABN_TEST_FILE}" "  \"${ABN_TEST_NAME}\": {\n")
  FILE(APPEND "${ABN_TEST_FILE}" "    \"api\": \"L0\",\n")
  GET_FILENAME_COMPONENT(DPCPP_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME_WE)
  IF (NOT WIN32 AND ABN_TEST_DIR)
    FILE(APPEND "${ABN_TEST_FILE}" "    \"commandLine\": \"run_test.sh ${ABN_TEST_COMMAND}\"")
  ELSE()
    FILE(APPEND "${ABN_TEST_FILE}" "    \"commandLine\": \"${ABN_TEST_COMMAND}\"")
  ENDIF()
  IF (ABN_TEST_DIR)
    FILE(APPEND "${ABN_TEST_FILE}" ",\n    \"workingSubDir\": \"${ABN_TEST_DIR}\"")
  ENDIF()
  IF (ABN_TEST_CHECK)
    FILE(APPEND "${ABN_TEST_FILE}" ",\n    \"functionalCheck\": [\"${ABN_TEST_CHECK}\"]")
  ENDIF()
  FILE(APPEND "${ABN_TEST_FILE}" "\n")
  FILE(APPEND "${ABN_TEST_FILE}" "  }")
ENDFUNCTION()

FUNCTION(FINALIZE_ABN_TEST)
  FILE(APPEND "${ABN_TEST_FILE}" "\n}\n")
ENDFUNCTION()

IF (NOT WIN32)
  INSTALL(FILES "run_test.sh" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT test
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
ENDIF()
  
INSTALL(FILES "${ABN_TEST_FILE}" DESTINATION "${CMAKE_INSTALL_BINDIR}/.." COMPONENT test)

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

FUNCTION(ADD_ABN_TEST)
  CMAKE_PARSE_ARGUMENTS(ABN_TEST "" "NAME;EXT" "COMMAND" ${ARGN})
  LIST(POP_FRONT ABN_TEST_COMMAND CMD)
  LIST(PREPEND ABN_TEST_COMMAND "./${CMD}")
  STRING(REPLACE ";" " " ABN_TEST_COMMAND_STR "${ABN_TEST_COMMAND}")
  WRITE_ABN_TEST(NAME "level-zero-gpu-raytracing-${ABN_TEST_NAME}${ABN_TEST_EXT}" COMMAND ${ABN_TEST_COMMAND_STR} CHECK "PASSED" DIR "bin")
ENDFUNCTION()

FUNCTION(ADD_INDIVIDUAL_TEST)
  CMAKE_PARSE_ARGUMENTS(ADD_INDIVIDUAL_TEST "C;G;S32" "NAME;COUNT" "COMMAND" ${ARGN})
  IF (ADD_INDIVIDUAL_TEST_COUNT)
    SET(ADD_INDIVIDUAL_TEST_COMMAND ${ADD_INDIVIDUAL_TEST_COMMAND} --test-count ${ADD_INDIVIDUAL_TEST_COUNT})
  ENDIF()
  
  IF (ADD_INDIVIDUAL_TEST_C)
    IF (ADD_INDIVIDUAL_TEST_COUNT)
       ADD_CTEST_TEST(NAME ${ADD_INDIVIDUAL_TEST_NAME} COMMAND ${ADD_INDIVIDUAL_TEST_COMMAND})
       
#      MATH(EXPR ADD_INDIVIDUAL_TEST_LAST "${ADD_INDIVIDUAL_TEST_COUNT} - 1")
#      FOREACH(TESTID RANGE 0 ${ADD_INDIVIDUAL_TEST_LAST})
#        ADD_CTEST_TEST(NAME ${ADD_INDIVIDUAL_TEST_NAME}_${TESTID} COMMAND ${ADD_INDIVIDUAL_TEST_COMMAND} --test-id ${TESTID})
#      ENDFOREACH()

    ELSE()
      ADD_CTEST_TEST(NAME ${ADD_INDIVIDUAL_TEST_NAME} COMMAND ${ADD_INDIVIDUAL_TEST_COMMAND})
    ENDIF()
  ENDIF()
  IF (ADD_INDIVIDUAL_TEST_G)
    ADD_ABN_TEST(NAME ${ADD_INDIVIDUAL_TEST_NAME} COMMAND ${ADD_INDIVIDUAL_TEST_COMMAND})
  ENDIF()
  IF (ADD_INDIVIDUAL_TEST_S32)
    ADD_ABN_TEST(EXT _simd32 NAME ${ADD_INDIVIDUAL_TEST_NAME} COMMAND ${ADD_INDIVIDUAL_TEST_COMMAND})
  ENDIF()
ENDFUNCTION()

GET_FILENAME_COMPONENT(SYCL_COMPILER_DIR ${CMAKE_CXX_COMPILER} PATH)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem \"${SYCL_COMPILER_DIR}/../include/sycl\" -isystem \"${SYCL_COMPILER_DIR}/../include/\"")       # disable warning from SYCL header (FIXME: why required?)


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

ADD_INDIVIDUAL_TEST(C G NAME cornell_box_exp               COMMAND embree_rthwif_cornell_box ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga)
SET(ABN_TEST_FIRST "OFF")
ADD_INDIVIDUAL_TEST(C G NAME cornell_box_instance_exp      COMMAND embree_rthwif_cornell_box ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga --instance)
IF (NOT ZE_RAYTRACING_RT_SIMULATION)
  ADD_INDIVIDUAL_TEST(C G NAME cornell_box_device_memory_exp COMMAND embree_rthwif_cornell_box ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga --instance --device-memory)
ENDIF()

ADD_INDIVIDUAL_TEST(C   NAME benchmark_triangles_exp             COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --benchmark_triangles COUNT 21)
ADD_INDIVIDUAL_TEST(C   NAME benchmark_procedurals_exp           COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --benchmark_procedurals COUNT 21)

ADD_INDIVIDUAL_TEST(C G NAME builder_triangles_expected_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_procedurals_expected_exp    COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_instances_expected_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_mixed_expected_exp          COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_expected COUNT 128)

ADD_INDIVIDUAL_TEST(C G NAME builder_triangles_worst_case_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_worst_case COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_procedurals_worst_case_exp    COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_worst_case COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_instances_worst_case_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_worst_case COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_mixed_worst_case_exp          COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_worst_case COUNT 128)

ADD_INDIVIDUAL_TEST(C G NAME triangles_committed_hit_exp        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-committed-hit)
ADD_INDIVIDUAL_TEST(C G NAME triangles_potential_hit_exp        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-potential-hit)
ADD_INDIVIDUAL_TEST(C G NAME triangles_anyhit_shader_commit_exp COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-commit)
ADD_INDIVIDUAL_TEST(C G NAME triangles_anyhit_shader_reject_exp COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-reject)
ADD_INDIVIDUAL_TEST(C G NAME procedurals_committed_hit_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --no-instancing --procedurals-committed-hit)

ADD_INDIVIDUAL_TEST(C G NAME hwinstancing_triangles_committed_hit_exp        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-committed-hit)
ADD_INDIVIDUAL_TEST(C G NAME hwinstancing_triangles_potential_hit_exp        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-potential-hit)
ADD_INDIVIDUAL_TEST(C G NAME hwinstancing_triangles_anyhit_shader_commit_exp COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-commit)
ADD_INDIVIDUAL_TEST(C G NAME hwinstancing_triangles_anyhit_shader_reject_exp COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-reject)
ADD_INDIVIDUAL_TEST(C G NAME hwinstancing_procedurals_committed_hit_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --hw-instancing --procedurals-committed-hit)

ADD_INDIVIDUAL_TEST(C G NAME swinstancing_triangles_committed_hit_exp        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-committed-hit)
ADD_INDIVIDUAL_TEST(C G NAME swinstancing_triangles_potential_hit_exp        COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-potential-hit)
ADD_INDIVIDUAL_TEST(C G NAME swinstancing_triangles_anyhit_shader_commit_exp COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-commit)
ADD_INDIVIDUAL_TEST(C G NAME swinstancing_triangles_anyhit_shader_reject_exp COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-reject)
ADD_INDIVIDUAL_TEST(C G NAME swinstancing_procedurals_committed_hit_exp      COMMAND embree_rthwif_test ${RTAS_BUILDER_MODE} --sw-instancing --procedurals-committed-hit)

# EXT API version tests

ADD_INDIVIDUAL_TEST(C G NAME cornell_box_ext               COMMAND embree_rthwif_cornell_box_ext ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga)
ADD_INDIVIDUAL_TEST(C G NAME cornell_box_instance_ext      COMMAND embree_rthwif_cornell_box_ext ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga --instance)
IF (NOT ZE_RAYTRACING_RT_SIMULATION)
  ADD_INDIVIDUAL_TEST(C G NAME cornell_box_device_memory_ext COMMAND embree_rthwif_cornell_box_ext ${RTAS_BUILDER_MODE} --compare cornell_box_reference.tga --instance --device-memory)
ENDIF()

ADD_INDIVIDUAL_TEST(C   NAME benchmark_triangles_ext             COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --benchmark_triangles COUNT 21)
ADD_INDIVIDUAL_TEST(C   NAME benchmark_procedurals_ext           COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --benchmark_procedurals COUNT 21)

ADD_INDIVIDUAL_TEST(C G NAME builder_triangles_expected_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_procedurals_expected_ext    COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_instances_expected_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_mixed_expected_ext          COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_expected COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_many_instances_ext       COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_many_instances)

ADD_INDIVIDUAL_TEST(C G NAME builder_triangles_worst_case_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_triangles   --build_mode_worst_case COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_procedurals_worst_case_ext    COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_procedurals --build_mode_worst_case COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_instances_worst_case_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_instances   --build_mode_worst_case COUNT 128)
ADD_INDIVIDUAL_TEST(C G NAME builder_mixed_worst_case_ext          COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --build_test_mixed       --build_mode_worst_case COUNT 128)

ADD_INDIVIDUAL_TEST(C G S32 NAME triangles_committed_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-committed-hit)
ADD_INDIVIDUAL_TEST(C G S32 NAME triangles_potential_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-potential-hit)
ADD_INDIVIDUAL_TEST(C G S32 NAME triangles_anyhit_shader_commit_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-commit)
ADD_INDIVIDUAL_TEST(C G S32 NAME triangles_anyhit_shader_reject_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --triangles-anyhit-shader-reject)
ADD_INDIVIDUAL_TEST(C G S32 NAME procedurals_committed_hit_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --no-instancing --procedurals-committed-hit)

ADD_INDIVIDUAL_TEST(C G S32 NAME hwinstancing_triangles_committed_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-committed-hit)
ADD_INDIVIDUAL_TEST(C G S32 NAME hwinstancing_triangles_potential_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-potential-hit)
ADD_INDIVIDUAL_TEST(C G S32 NAME hwinstancing_triangles_anyhit_shader_commit_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-commit)
ADD_INDIVIDUAL_TEST(C G S32 NAME hwinstancing_triangles_anyhit_shader_reject_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --triangles-anyhit-shader-reject)
ADD_INDIVIDUAL_TEST(C G S32 NAME hwinstancing_procedurals_committed_hit_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --hw-instancing --procedurals-committed-hit)

ADD_INDIVIDUAL_TEST(C G S32 NAME swinstancing_triangles_committed_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-committed-hit)
ADD_INDIVIDUAL_TEST(C G S32 NAME swinstancing_triangles_potential_hit_ext        COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-potential-hit)
ADD_INDIVIDUAL_TEST(C G S32 NAME swinstancing_triangles_anyhit_shader_commit_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-commit)
ADD_INDIVIDUAL_TEST(C G S32 NAME swinstancing_triangles_anyhit_shader_reject_ext COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --triangles-anyhit-shader-reject)
ADD_INDIVIDUAL_TEST(C G S32 NAME swinstancing_procedurals_committed_hit_ext      COMMAND embree_rthwif_test_ext ${RTAS_BUILDER_MODE} --sw-instancing --procedurals-committed-hit)

FINALIZE_ABN_TEST()

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

      FILE(GLOB_RECURSE LIB_HWLOC_FILES "${DPCPP_COMPILER_DIR}/../lib/libhwloc.so.*")
      FOREACH(so_file ${LIB_HWLOC_FILES})
        # In newer versions of CMake FOLLOW_SYMLINKS can be used instead of this mechanism
        GET_FILENAME_COMPONENT(name "${so_file}" NAME)
        GET_FILENAME_COMPONENT(real_file "${so_file}" REALPATH)
        INSTALL(FILES "${real_file}" DESTINATION "${CMAKE_INSTALL_BINDIR}" RENAME "${name}" COMPONENT lib)
      ENDFOREACH()

      FILE(GLOB_RECURSE LIB_UMF_FILES
        "${DPCPP_COMPILER_DIR}/../lib/libumf.so.*"
        "${DPCPP_COMPILER_DIR}/../../../umf/latest/libumf.so.*")
      INSTALL(FILES ${LIB_UMF_FILES} DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libsvml.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libirng.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libimf.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libintlc.so.5" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libiomp5.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)

      IF (EXISTS "${DPCPP_COMPILER_DIR}/../lib/libpi_level_zero.so")
        INSTALL(FILES "${DPCPP_COMPILER_DIR}/../lib/libpi_level_zero.so" DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lib)
      ENDIF()

    ENDIF()
  ENDIF()
      
ENDIF()

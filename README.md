
Level Zero Ray Tracing Build Extension
======================================

This Level Zero Ray Tracing Build Extension implements high
performance CPU based construction algorithms for 3D acceleration
structures that are compatible with the ray tracing hardware of
Intel(R) Xe GPUs. This library is used by Intel(R) oneAPI Level Zero
to implement part of the ray tracing extension (ze_raytracing). This
library should not get used directly but only through Level Zero.

To compile the library under Linux execute:

  cmake -G Ninja -D CMAKE_CXX_COMPILER=g++ -D CMAKE_C_COMPILER=gcc -D CMAKE_BUILD_TYPE=Release -D ZE_RAYTRACING_SYCL_TESTS=OFF ..

  cmake --build . --target package

To compile the library under Windows execute:

  cmake -G "Visual Studio 16 2019" -T "V142" -A "x64" -D CMAKE_BUILD_TYPE=Release -D ZE_RAYTRACING_SYCL_TESTS=OFF ..

  cmake --build . --target package




Level Zero Ray Tracing Support
==============================

The Level Zero Ray Tracing Support library implements high
performance CPU based construction algorithms for 3D acceleration
structures that are compatible with the ray tracing hardware of
Intel GPUs. This library is used by Intel(R) oneAPI Level Zero
to implement part of the ray tracing extension (ze_raytracing). This
library should not get used directly but only through Level Zero.


To compile the library under Linux execute:

  mkdir build
  
  cd build
  
  cmake -G Ninja -D CMAKE_CXX_COMPILER=g++ -D CMAKE_C_COMPILER=gcc -D CMAKE_BUILD_TYPE=Release ..

  cmake --build . --target package


To compile the library under Windows execute:

  mkdir build

  cd build

  cmake -G "Visual Studio 16 2019" -T "V142" -A "x64" -D CMAKE_BUILD_TYPE=Release ..

  cmake --build . --target package


To compile the library under Linux including SYCL examples:

  wget https://github.com/intel/llvm/releases/download/sycl-nightly%2F20230304/dpcpp-compiler.tar.gz
  
  tar xzf dpcpp-compiler
  
  source dpcpp_compiler/startup.sh

  mkdir build

  cd build
  
  cmake -G Ninja -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_C_COMPILER=clang -D CMAKE_BUILD_TYPE=Release -D ZE_RAYTRACING_SYCL_TESTS=ON ..

  cmake --build . --target package

  ctest


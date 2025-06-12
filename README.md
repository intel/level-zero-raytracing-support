<!---

Copyright (C) 2018-2025 Intel Corporation

SPDX-License-Identifier: Apache-2.0

-->

# oneAPI Level Zero Ray Tracing Support


## Introduction

The oneAPI Level Zero Ray Tracing Support library is an open source project used by Intel(R)
oneAPI Level Zero as the implementation of the RTAS extension
[ZE_extension_rtas](https://oneapi-src.github.io/level-zero-spec/level-zero/latest/core/EXT_RTAS.html#ze-extension-rtas)
and deprecated experimental RTAS builder extension
[ZE_experimental_rtas_builder](https://oneapi-src.github.io/level-zero-spec/level-zero/latest/core/EXT_Exp_RTASBuilder.html#ze-experimental-rtas-builder).

The library implements high performance CPU based ray tracing
acceleration structure (RTAS) construction algorithms that take input
from the above mentioned APIs and produce an RTAS that is compatible
with the ray tracing hardware of Intel GPUs.


## License

The oneAPI Level Zero Ray Tracing Support library is distributed under the [Apache 2.0 license](https://opensource.org/license/apache-2-0).


## Supported Platforms

|Platform|Supported
|--------|:----:|
|Alchemist| Y |
|Meteor Lake| Y |
|Arrow Lake| Y |
|Battlemage| Y |
|Lunar Lake| Y |
|Panther Lake| Y |

_No code changes may be introduced that would regress support for any currently supported hardware. All contributions must ensure continued compatibility and functionality across all supported hardware platforms. Failure to maintain hardware compatibility may result in the rejection or reversion of the contribution. Any deliberate modifications or removal of hardware support will be transparently communicated in the release notes._

_Debug parameters, environmental variables, and internal data structures are considered as internal implementation detail and may be changed or removed at any time._


## Installation

The oneAPI Level Zero Ray Tracing Support library is available for installation on a variety of Linux distributions and can be installed via the distro's package manager.

For example on Ubuntu* 22.04:

```
apt-get install ze-intel-gpu-raytracing
```


## Compilation

To compile the library under Linux execute:

```
cmake -B build -G Ninja -D CMAKE_BUILD_TYPE=Release .
cmake --build build --target package
```

To compile the library under Windows execute:

```
cmake -B build -G "Visual Studio 17 2022" -A "x64" -D CMAKE_BUILD_TYPE=Release .
cmake --build build --target package
```

To compile the library under Linux including SYCL tests:

```
wget https://github.com/intel/llvm/releases/download/sycl-nightly%2F20230304/dpcpp-compiler.tar.gz
tar xzf dpcpp-compiler
source dpcpp_compiler/startup.sh
cmake -B build -G Ninja -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_C_COMPILER=clang -D CMAKE_BUILD_TYPE=Release -D ZE_RAYTRACING_SYCL_TESTS=INTERNAL_RTAS_BUILDER .
cmake --build build --target package
cd build
ctest
```


## Linking applications

Directly linking to the oneAPI Level Zero Ray Tracing Support library
is not supported. Level Zero applications should link with [Level Zero
Loader](https://github.com/oneapi-src/level-zero) and use the
[ZE_extension_rtas](https://oneapi-src.github.io/level-zero-spec/level-zero/latest/core/EXT_RTAS.html#ze-extension-rtas)
API.


## Dependencies

* Intel(R) oneAPI Threading Building Blocks [TBB](https://github.com/uxlfoundation/oneTBB).


## How to provide feedback

Please submit an issue using github.com [interface](https://github.com/intel/level-zero-raytracing-support/issues).


## How to contribute

Create a [pull
request](https://github.com/intel/level-zero-raytracing-support/pulls)
on github.com with your patch. A maintainer will contact you if there
are questions or concerns.

## See also
* [oneAPI Level Zero RTAS extension](https://oneapi-src.github.io/level-zero-spec/level-zero/latest/core/EXT_RTAS.html#ze-extension-rtas)
* [oneAPI Level Zero experimental RTAS builder extension](https://oneapi-src.github.io/level-zero-spec/level-zero/latest/core/EXT_Exp_RTASBuilder.html#ze-experimental-rtas-builder)
* [Intel(R) OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)

___(*) Other names and brands may be claimed as property of others.___



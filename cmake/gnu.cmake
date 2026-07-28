## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")                       # enables most warnings
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")                    # enables more warnings
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")                # enables conversion warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wimplicit-fallthrough")      # warns when a switch case falls through
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat -Wformat-security")  # enables string format vulnerability warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-class-memaccess")        # disables clearing an object of type ‘XXX’ with no trivial copy-assignment; use assignment or value-initialization instead

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-overflow")            # assume that signed overflow occurs
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks") # keep all checks for NULL pointers
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fwrapv")                         # this option instructs the compiler to assume that signed arithmetic overflow warps around.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")                   # treat char as signed on all processors, including ARM

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")                     # enables support for more secure position independent execution
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")                     # generate position independent code suitable for shared libraries

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")        # disables strict aliasing rules
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=3")         # perform extra security checks for some standard library calls
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_ASSERTIONS")       # enable some GLIBCXX assertions
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")    # protects against return address overrides
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-clash-protection")    # enables runtime checks for variable-sized stack allocations
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcf-protection=full")        # use control flow enforcement (CET)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mindirect-branch=thunk")     # enables Spectre v2 thunk-based mitigations (incompatible with fcf-protection)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfunction-return=thunk")     # enables Spectre v2 thunk-based mitigations (incompatible with fcf-protection)

SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined") # issues link error for undefined symbols in shared library
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack")     # we do not need an executable stack
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z relro -z now")    # re-arranges data sections to increase security
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack")           # we do not need an executable stack
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z relro -z now")          # re-arranges data sections to increase security
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")                     # enables position independent execution for executable

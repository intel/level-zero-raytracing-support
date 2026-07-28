## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")          # enables comprehensive set of warnings
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /LTCG")        # enables link-time code generation
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /sdl")         # enables SDL checks
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /analyze")     # performs static code analysis
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /guard:cf")    # enables control flow guard
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /CETCOMPAT")   # protects against control target attacks
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS")          # protects against return address overrides
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qspectre")    # enables Spectre mitigations

SET(SECURE_LINKER_FLAGS "")
SET(SECURE_LINKER_FLAGS "${SECURE_LINKER_FLAGS} /NXCompat")    # compatible with data execution prevention (on by default)
SET(SECURE_LINKER_FLAGS "${SECURE_LINKER_FLAGS} /DynamicBase") # random rebase of executable at load time
SET(SECURE_LINKER_FLAGS "${SECURE_LINKER_FLAGS} /guard:cf")    # enables control flow guard

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SECURE_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SECURE_LINKER_FLAGS}")


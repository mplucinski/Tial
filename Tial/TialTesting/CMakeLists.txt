# Copyright (c) 2015, Mariusz Plucinski
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted
# provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions
#    and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of
#    conditions and the following disclaimer in the documentation and/or other materials provided
#    with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
# IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
set(UPPER_PROJECT_VERSION "${PROJECT_VERSION}")
project(TialTesting VERSION "${UPPER_PROJECT_VERSION}")

add_subdirectory(Preprocessor)
set(TialTestingPreprocessor "${TialTestingPreprocessor}" PARENT_SCOPE)

include(TialTesting.cmake)

find_path(DLT_SOURCE_DIR dtl/dtl.hpp
	HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../../Dependencies/dtl-cpp
)

add_tial_library(dtl
	HEADERS
		${DLT_SOURCE_DIR}/dtl/Diff.hpp
		${DLT_SOURCE_DIR}/dtl/Diff3.hpp
		${DLT_SOURCE_DIR}/dtl/dtl.hpp
		${DLT_SOURCE_DIR}/dtl/functors.hpp
		${DLT_SOURCE_DIR}/dtl/Lcs.hpp
		${DLT_SOURCE_DIR}/dtl/Sequence.hpp
		${DLT_SOURCE_DIR}/dtl/Ses.hpp
		${DLT_SOURCE_DIR}/dtl/variables.hpp
)
set_property(TARGET dtl PROPERTY LINKER_LANGUAGE CXX)

add_tial_library(${PROJECT_NAME}
	HEADERS
		TialTesting.hpp
		Check.hpp
		Main.hpp
		Suite.hpp
		Thread.hpp

	SOURCES
		src/Check.cpp
		src/Main.cpp
		src/Thread.cpp

	CMAKE_CONFIG_FILE
		TialTestingConfig.cmake.in

	CMAKE_SOURCES
		TialTesting.cmake
)
install(PROGRAMS "${TialTestingPreprocessor}" DESTINATION bin)
target_include_directories(${PROJECT_NAME} SYSTEM
	PRIVATE ${Boost_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} TialUtility dtl)

add_tial_test(Test${PROJECT_NAME}
	SOURCES tests/Testing.cpp
	LIBRARIES ${PROJECT_NAME}
)

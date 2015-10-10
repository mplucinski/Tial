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
cmake_policy(SET CMP0025 NEW)
cmake_minimum_required(VERSION 3.3)

project(Tial
	VERSION 0.0.1.0
	LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(Tial/TialUtility/TialUtility.cmake)

find_package(Boost 1.55.0 REQUIRED)
find_package(Threads REQUIRED)

tial_set_common_settings()

option(TIAL_TEST_DATA_ENABLE "Use externally provided testing data (must be already installed)")
set(TIAL_TEST_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../Tial_TestData" CACHE PATH "Path to external test data")

function(tial_test_load_external_data VARIABLE SET_ID)
	if(TIAL_TEST_DATA_ENABLE)
		message(STATUS "Adding external test data set ${SET_ID}")
		set(DIR "${TIAL_TEST_DATA_PATH}/${SET_ID}")
		set(${VARIABLE})

		macro(tial_add_external_data DATA)
			set(${VARIABLE} "${${VARIABLE}}\n${DATA}")
		endmacro()

		include("${DIR}/index.cmake")

		set(${VARIABLE} "${${VARIABLE}}" PARENT_SCOPE)
	endif()
endfunction()

include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/dtl-cpp"
)

enable_testing()
set(CTEST_OUTPUT_ON_FAILURE ON)

add_subdirectory(Tial)

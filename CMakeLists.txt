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

include(CMakeParseArguments)
include(ExternalProject)
include(GenerateExportHeader)

find_package(Boost 1.55.0 REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# Compiler-specific settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-pedantic -Wall -Wextra -Werror)
else()
	message(FATAL_ERROR "Unknown compiler, can not set flags correctly")
endif()

# System-specific settings
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(TIAL_OSX_BUILD_FRAMEWORKS_DIR "${CMAKE_CURRENT_BINARY_DIR}/Frameworks")
	file(MAKE_DIRECTORY "${TIAL_OSX_BUILD_FRAMEWORKS_DIR}")
endif()

# Build configuration
set(TIAL_UTILITY_LOGGER_DEFAULT_COMPILE_LEVEL 25)
if(DEFINED CMAKE_BUILD_TYPE)
	string(TOUPPER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
	if(BUILD_TYPE MATCHES "DEBUG")
		set(TIAL_UTILITY_LOGGER_DEFAULT_COMPILE_LEVEL 0)
	elseif(BUILD_TYPE MATCHES "RELEASE")
		set(TIAL_UTILITY_LOGGER_DEFAULT_COMPILE_LEVEL 50)
	elseif(BUILD_TYPE MATCHES "RELWITHDEBINFO")
		set(TIAL_UTILITY_LOGGER_DEFAULT_COMPILE_LEVEL 25)
	elseif(BUILD_TYPE MATCHES "MINSIZEREL")
		set(TIAL_UTILITY_LOGGER_DEFAULT_COMPILE_LEVEL 50)
		find_package(SelfPackers REQUIRED)
		if(NOT SELF_PACKER_FOR_EXECUTABLE OR NOT SELF_PACKER_FOR_SHARED_LIB)
			message(FATAL_ERROR "Self packer not found")
		endif()
	endif()
endif()

set(TIAL_UTILITY_LOGGER_COMPILE_LEVEL ${TIAL_UTILITY_LOGGER_DEFAULT_COMPILE_LEVEL} CACHE STRING "Level of logger output messages below which messages are disabled in compliation stage")
message(STATUS "Tial logger messages below level ${TIAL_UTILITY_LOGGER_COMPILE_LEVEL} will not be available in this build")

add_definitions(-DTIAL_UTILITY_LOGGER_COMPILE_LEVEL=${TIAL_UTILITY_LOGGER_COMPILE_LEVEL})

add_definitions(-DTIAL_SOURCE_BASE_DIRECTORY="${CMAKE_SOURCE_DIR}")

function(pack_binary TARGET PACKER FLAGS)
	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET}>" "$<TARGET_FILE:${TARGET}>.uncompressed"
		COMMAND ${PACKER} ${FLAGS} $<TARGET_FILE:${TARGET}>
	)
endfunction()

function(pack_executable TARGET)
	pack_binary(${TARGET} ${SELF_PACKER_FOR_EXECUTABLE} "${SELF_PACKER_FOR_EXECUTABLE_FLAGS}")
endfunction()

function(pack_library TARGET)
	pack_binary(${TARGET} ${SELF_PACKER_FOR_SHARED_LIB} "${SELF_PACKER_FOR_SHARED_LIB_FLAGS}")
endfunction()

function(add_tial_library TARGET)
	set(OPTIONS "")
	set(ONE_VALUE_ARGS "")
	set(MULTI_VALUE_ARGS HEADERS SOURCES)
	cmake_parse_arguments(add_tial_library
			"${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN}
	)

	if(BUILD_TYPE MATCHES "MINSIZEREL")
		add_library(${TARGET} ${add_tial_library_HEADERS} ${add_tial_library_SOURCES})
	else()
		add_library(${TARGET} SHARED ${add_tial_library_HEADERS} ${add_tial_library_SOURCES})
	endif()
	generate_export_header(
		${TARGET}
	)
	set_property(TARGET ${TARGET}
			PROPERTY FRAMEWORK ON
	)
	set_property(TARGET ${TARGET}
			PROPERTY PUBLIC_HEADER ${add_tial_library_HEADERS}
	)
	target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
	install(TARGETS ${TARGET}
			ARCHIVE DESTINATION lib
			LIBRARY DESTINATION lib
			FRAMEWORK DESTINATION Frameworks
			PUBLIC_HEADER DESTINATION include/${TARGET}
	)
endfunction()

function(add_tial_executable)
	set(OPTIONS BUNDLE)
	set(ONE_VALUE_ARGS TARGET PLIST)
	set(MULTI_VALUE_ARGS SOURCES)
	cmake_parse_arguments(add_tial_executable
		"${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN}
	)

	add_executable(${add_tial_executable_TARGET} ${add_tial_executable_SOURCES})
	if(BUILD_TYPE MATCHES "MINSIZEREL")
		pack_executable(${add_tial_executable_TARGET})
	endif()

	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		if(add_tial_executable_BUNDLE)
			set_property(TARGET ${add_tial_executable_TARGET} APPEND_STRING
				PROPERTY MACOSX_BUNDLE TRUE
		 	)
			if(add_tial_executable_PLIST)
				set_property(TARGET ${add_tial_executable_TARGET} APPEND_STRING
					PROPERTY MACOSX_BUNDLE_INFO_PLIST ${add_tial_executable_PLIST}
		 		)
			endif()
		else()
			if(add_tial_executable_PLIST)
				set_property(TARGET ${add_tial_executable_TARGET} APPEND_STRING
					PROPERTY LINK_FLAGS "-sectcreate __TEXT __info_plist ${add_tial_executable_PLIST}"
		 		)
		 	endif()
		endif()
	endif()

	target_link_libraries(${add_tial_executable_TARGET} ${CMAKE_THREAD_LIBS_INIT})
endfunction()

function(_use_tial_framework FRAMEWORK)
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		file(COPY "${FRAMEWORK}" DESTINATION "${TIAL_OSX_BUILD_FRAMEWORKS_DIR}")
	endif()
endfunction()

function(use_tial_frameworks)
	foreach(FRAMEWORK ${ARGN})
		_use_tial_framework(${FRAMEWORK})
	endforeach()
endfunction()

function(_bundle_contents_directory VAR TARGET)
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		set(BUNDLE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.app")
		set(${VAR} "${BUNDLE}/Contents" PARENT_SCOPE)
	endif()
endfunction()

function(target_use_tial_data TARGET DATA_DIR)
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		_bundle_contents_directory(CONTENTS_DIR ${TARGET})
		set(RESOURCES_DIR "${CONTENTS_DIR}/Resources")
		add_custom_target(${TARGET}Data ALL
			COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCES_DIR}"
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${DATA_DIR}" "${RESOURCES_DIR}"
		)
	endif()
endfunction()

function(_target_use_tial_framework TARGET FRAMEWORK)
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		# set(BUNDLE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.app")
		_bundle_contents_directory(CONTENTS_DIR ${TARGET})
		set(FRAMEWORKS_DIR "${CONTENTS_DIR}/Frameworks")
		get_filename_component(FRAMEWORK_NAME "${FRAMEWORK}" NAME)
		add_custom_command(TARGET ${TARGET}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${FRAMEWORKS_DIR}"
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${FRAMEWORK}" "${FRAMEWORKS_DIR}/${FRAMEWORK_NAME}"
		)
	endif()
endfunction()

function(target_use_tial_frameworks TARGET)
	foreach(FRAMEWORK ${ARGN})
		_target_use_tial_framework(${TARGET} ${FRAMEWORK})
	endforeach()
endfunction()


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

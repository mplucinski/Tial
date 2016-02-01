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
function(tial_set_common_settings)
	if(TIAL_COMMON_SETTINGS_DONE)
		return()
	endif()

	include(CMakePackageConfigHelpers)
	include(CMakeParseArguments)

	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		include(BundleUtilities)
	endif()

	set(CMAKE_INCLUDE_CURRENT_DIR ON PARENT_SCOPE)
	set(CMAKE_C_STANDARD 11 PARENT_SCOPE)
	set(CMAKE_C_STANDARD_REQUIRED ON PARENT_SCOPE)
	set(CMAKE_CXX_STANDARD 14 PARENT_SCOPE)
	set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
	set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)

	set(CMAKE_C_VISIBILITY_PRESET hidden PARENT_SCOPE)
	set(CMAKE_CXX_VISIBILITY_PRESET hidden PARENT_SCOPE)

	set(CMAKE_INCLUDE_CURRENT_DIR ON PARENT_SCOPE)
	set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON PARENT_SCOPE)

	# Compiler-specific settings
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-pedantic")
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-Wall")
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-Wextra")
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "-Werror")
	else()
		message(FATAL_ERROR "Unknown compiler ${CMAKE_CXX_COMPILER_ID}, can not set flags correctly")
	endif()

	# System-specific settings

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

	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			APPEND PROPERTY COMPILE_DEFINITIONS "TIAL_UTILITY_LOGGER_COMPILE_LEVEL=${TIAL_UTILITY_LOGGER_COMPILE_LEVEL}"
	)

	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			APPEND PROPERTY COMPILE_DEFINITIONS "TIAL_SOURCE_BASE_DIRECTORY=\"${CMAKE_SOURCE_DIR}\""
	)

	set(TIAL_COMMON_SETTINGS_DONE ON PARENT_SCOPE)
endfunction()

function(tial_pack_binary TARGET PACKER FLAGS)
	add_custom_command(
			TARGET ${TARGET}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${TARGET}>" "$<TARGET_FILE:${TARGET}>.uncompressed"
			COMMAND ${PACKER} ${FLAGS} $<TARGET_FILE:${TARGET}>
	)
endfunction()

function(tial_pack_executable TARGET)
	tial_pack_binary(${TARGET} ${SELF_PACKER_FOR_EXECUTABLE} "${SELF_PACKER_FOR_EXECUTABLE_FLAGS}")
endfunction()

function(tial_pack_library TARGET)
	tial_pack_binary(${TARGET} ${SELF_PACKER_FOR_SHARED_LIB} "${SELF_PACKER_FOR_SHARED_LIB_FLAGS}")
endfunction()
set(TIAL_UTILITY_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(add_tial_library TARGET)
	set(OPTIONS "")
	set(ONE_VALUE_ARGS CMAKE_CONFIG_FILE)
	set(MULTI_VALUE_ARGS HEADERS SOURCES RESOURCES CMAKE_SOURCES LIBRARIES)
	cmake_parse_arguments(add_tial_library
			"${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN}
	)

	include(GenerateExportHeader)

	if(NOT add_tial_library_CMAKE_CONFIG_FILE)
		set(add_tial_library_CMAKE_CONFIG_FILE "${TIAL_UTILITY_CMAKE_DIR}/TialDefaultConfig.cmake.in")
	endif()

	set(TIAL_PACKAGE_INIT "@PACKAGE_INIT@

if(TARGET ${TARGET})
	return()
endif()")

	if("${TARGET}" STREQUAL "TialUtility")
		set(TIAL_PACKAGE_INIT "${TIAL_PACKAGE_INIT}
include(\"\${CMAKE_CURRENT_LIST_DIR}/TialUtility.cmake\")")
	else()
		set(TIAL_PACKAGE_INIT "${TIAL_PACKAGE_INIT}
find_package(TialUtility REQUIRED)")
	endif()

	set(TIAL_PACKAGE_INIT "${TIAL_PACKAGE_INIT}
tial_set_common_settings()

add_library(${TARGET} UNKNOWN IMPORTED)
set_property(TARGET ${TARGET} PROPERTY INTERFACE_INCLUDE_DIRECTORIES \"\${PACKAGE_PREFIX_DIR}/include\")
find_library(_${TARGET}_IMPORTED_LOCATION ${TARGET}
	PATHS \"\${PACKAGE_PREFIX_DIR}/lib/\"
)
set_property(TARGET ${TARGET} PROPERTY IMPORTED_LOCATION \"\${_${TARGET}_IMPORTED_LOCATION}\")
set_property(TARGET ${TARGET} PROPERTY INTERFACE_LINK_LIBRARIES ${add_tial_library_LIBRARIES})")

	configure_file(${add_tial_library_CMAKE_CONFIG_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake.in" @ONLY)

	configure_package_config_file(
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake.in"
			"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
			INSTALL_DESTINATION "share/${TARGET}/cmake"
	)
	write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake"
			VERSION ${PROJECT_VERSION}
			COMPATIBILITY ExactVersion
	)

	list(APPEND add_tial_library_CMAKE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake")
	list(APPEND add_tial_library_CMAKE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}ConfigVersion.cmake")

	foreach(SRC ${add_tial_library_CMAKE_SOURCES})
		set_property(SOURCE "${SRC}"
				PROPERTY MACOSX_PACKAGE_LOCATION "Resources/CMake"
		)
	endforeach()

	list(APPEND LIBRARY_ARGS ${TARGET})

	if(NOT BUILD_TYPE MATCHES "MINSIZEREL")
		list(APPEND LIBRARY_ARGS SHARED)
	endif()

	list(APPEND LIBRARY_ARGS
			${add_tial_library_HEADERS}
			${add_tial_library_SOURCES}
			${add_tial_library_RESOURCES}
			${add_tial_library_CMAKE_SOURCES}
	)
	add_library(${LIBRARY_ARGS})

	set(EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Export.hpp")
	generate_export_header(${TARGET}
			EXPORT_FILE_NAME ${EXPORT_HEADER}
	)
	set_property(TARGET ${TARGET} APPEND PROPERTY SOURCES ${EXPORT_HEADER})
	list(APPEND add_tial_library_HEADERS ${EXPORT_HEADER})

	set_property(TARGET ${TARGET} PROPERTY PUBLIC_HEADER ${add_tial_library_HEADERS})
	set_property(TARGET ${TARGET} PROPERTY RESOURCE ${add_tial_library_RESOURCES})
	target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT} ${add_tial_library_LIBRARIES})

	install(TARGETS ${TARGET}
			ARCHIVE DESTINATION lib
			LIBRARY DESTINATION lib
			FRAMEWORK DESTINATION "."
			PUBLIC_HEADER DESTINATION "include/${TARGET}"
			RESOURCE DESTINATION "share/${TARGET}"
	)
	install(FILES ${add_tial_library_CMAKE_SOURCES}
			DESTINATION "share/${TARGET}/cmake"
	)
endfunction()

function(add_tial_executable TARGET)
	set(OPTIONS BUNDLE)
	set(ONE_VALUE_ARGS PLIST)
	set(MULTI_VALUE_ARGS SOURCES RESOURCES)
	cmake_parse_arguments(add_tial_executable
			"${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN}
			)

	add_executable(${TARGET} ${add_tial_executable_SOURCES} ${add_tial_executable_RESOURCES})
	# if(BUILD_TYPE MATCHES "MINSIZEREL")
	# 	tial_pack_executable(${TARGET})
	# endif()

	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		if(add_tial_executable_BUNDLE)
			set_property(TARGET ${TARGET}
					PROPERTY MACOSX_BUNDLE TRUE
			)
			# set_property(TARGET ${TARGET}
					# PROPERTY RESOURCE ${add_tial_executable_RESOURCES}
			# )
			if(add_tial_executable_PLIST)
				set_property(TARGET ${TARGET} APPEND_STRING
						PROPERTY MACOSX_BUNDLE_INFO_PLIST ${add_tial_executable_PLIST}
				)
			endif()
		else()
			if(add_tial_executable_PLIST)
				set_property(TARGET ${TARGET} APPEND_STRING
						PROPERTY LINK_FLAGS "-sectcreate __TEXT __info_plist ${add_tial_executable_PLIST}"
				)
			endif()
		endif()
	endif()

	set(resources "")
	foreach(resource ${add_tial_executable_RESOURCES})
		if(NOT IS_ABSOLUTE "${resource}")
			set(resource "${CMAKE_CURRENT_SOURCE_DIR}/${resource}")
		endif()
		list(APPEND resources "${resource}")
	endforeach()

	add_custom_command(TARGET ${TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND}
			-D TARGET:STRING="${TARGET}"
			-D BUNDLE:BOOL="${add_tial_executable_BUNDLE}"
			-D OUTPUT_NAME:STRING="$<TARGET_FILE:${TARGET}>"
			-D RESOURCES:STRING="${resources}"
			-P "${TIAL_UTILITY_CMAKE_DIR}/TialPostBuild.cmake"
	)

	target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
	install(TARGETS ${TARGET}
		RUNTIME DESTINATION bin
		BUNDLE DESTINATION "."
		RESOURCE DESTINATION "share/${TARGET}"
	)
endfunction()

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
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
set(TIAL_TESTING_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TIAL_TESTING_LIST_DIR "${TIAL_TESTING_LIST_DIR}" ABSOLUTE)
set(TIAL_TESTING_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}" PARENT_SCOPE)

find_package(PythonInterp 3.4 REQUIRED)

function(add_tial_test TARGET)
	set(OPTIONS BUNDLE)
	set(ONE_VALUE_ARGS "")
	set(MULTI_VALUE_ARGS SOURCES LIBRARIES COMMAND)
	cmake_parse_arguments(ADD_TIAL_TEST "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

	set(PROCESSED_SOURCES "")
	foreach(SOURCE ${ADD_TIAL_TEST_SOURCES})
		if(NOT IS_ABSOLUTE "${SOURCE}")
			set(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}")
		endif()
		file(RELATIVE_PATH OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}" "${SOURCE}")
		string(REPLACE ".." "__" OUTPUT "${OUTPUT}")
		set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}")
		get_filename_component(OUTPUT_DIRECTORY "${OUTPUT}" DIRECTORY)
		get_filename_component(OUTPUT_NAME "${OUTPUT}" NAME)
		set(OUTPUT "${OUTPUT_DIRECTORY}/test-${OUTPUT_NAME}")
		add_custom_command(OUTPUT "${OUTPUT}"
			DEPENDS "${TialTestingPreprocessor}" "${SOURCE}"
			COMMAND
				${PYTHON_EXECUTABLE} ${TialTestingPreprocessor} "${SOURCE}" -o "${OUTPUT}"
					--compiler "${CMAKE_CXX_COMPILER}"
					--compiler-definitions \\\\$<JOIN:$<TARGET_PROPERTY:${TARGET},COMPILE_DEFINITIONS>,$<COMMA>>
					--compiler-options \\\\$<JOIN:$<TARGET_PROPERTY:${TARGET},COMPILE_OPTIONS>,$<COMMA>>
					--compiler-include-directories \\\\$<JOIN:$<TARGET_PROPERTY:${TARGET},INCLUDE_DIRECTORIES>,$<COMMA>>
					--compiler-cxx-standard $<TARGET_PROPERTY:${TARGET},CXX_STANDARD>
		)
		list(APPEND PROCESSED_SOURCES "${OUTPUT}")
	endforeach()

	list(APPEND EXECUTABLE_ARGS ${TARGET})
	if(ADD_TIAL_TEST_BUNDLE)
		# set(INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
		# configure_file("${TIAL_TESTING_LIST_DIR}/../Info.plist.in" "${INFO_PLIST}" @ONLY)
		set(INFO_PLIST "${TIAL_TESTING_LIST_DIR}/Info.plist.in")
		list(APPEND EXECUTABLE_ARGS BUNDLE)
	endif()

	list(APPEND EXECUTABLE_ARGS SOURCES ${PROCESSED_SOURCES})
	if(ADD_TIAL_TEST_BUNDLE)
		list(APPEND EXECUTABLE_ARGS PLIST ${INFO_PLIST})
	endif()
	add_tial_executable(${EXECUTABLE_ARGS})

	target_compile_definitions(${TARGET} PRIVATE TIAL_TESTING_NAME=${TARGET})
	target_link_libraries(${TARGET} TialTesting ${ADD_TIAL_TEST_LIBRARIES})

	# add_custom_command(TARGET ${TARGET}
	# 	POST_BUILD
	# 	COMMAND ${CMAKE_COMMAND}
	# 		-D TEST_TARGET="${TARGET}"
	# 		-D TEST_BUNDLE:BOOL="${ADD_TIAL_TEST_BUNDLE}"
	# 		-D TEST_OUTPUT_NAME="$<TARGET_FILE:${TARGET}>"
	# 		-P "${TIAL_TESTING_LIST_DIR}/TialTestingPostBuild.cmake"
	# )
	set(COMMAND ${ADD_TIAL_TEST_COMMAND})
	set(WORKING_DIRECTORY "")
	if(NOT COMMAND)
		if(ADD_TIAL_TEST_BUNDLE)
			set(COMMAND ${TARGET}.app/Contents/MacOS/${TARGET})
		else()
			get_property(DIRECTORY TARGET ${TARGET} PROPERTY RUNTIME_OUTPUT_DIRECTORY)
			set(COMMAND "${DIRECTORY}/${TARGET}")
			set(WORKING_DIRECTORY "${DIRECTORY}")
		endif()
	endif()
	add_test(${TARGET} ${COMMAND})
	if(WORKING_DIRECTORY)
		set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${WORKING_DIRECTORY})
	endif()
endfunction()

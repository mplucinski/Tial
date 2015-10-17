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
function(add_tial_test TARGET)
	set(OPTIONS "")
	set(ONE_VALUE_ARGS PLIST)
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
				${TialTestingPreprocessor} "${SOURCE}" -o "${OUTPUT}"
		)
		list(APPEND PROCESSED_SOURCES "${OUTPUT}")
	endforeach()

	add_tial_executable(TARGET ${TARGET} SOURCES ${PROCESSED_SOURCES} PLIST ${ADD_TIAL_TEST_PLIST})
	target_compile_definitions(${TARGET} PRIVATE TIAL_TESTING_NAME=${TARGET})
	target_link_libraries(${TARGET} TialTesting ${ADD_TIAL_TEST_LIBRARIES})
	set(COMMAND ${ADD_TIAL_TEST_COMMAND})
	if(NOT COMMAND)
		set(COMMAND ${TARGET})
	endif()
	add_test(${TARGET} ${COMMAND})
endfunction()

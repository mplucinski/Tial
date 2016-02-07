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
project(TialUtility VERSION "${UPPER_PROJECT_VERSION}")

add_tial_library(${PROJECT_NAME}
	HEADERS
		TialUtility.hpp
		ABI.hpp
		Algorithm.hpp
		ArgumentParser.hpp
		Directory.hpp
		Enum.hpp
		Exception.hpp
		Language.hpp
		Logger.hpp
		Path.hpp
		StreamOperator.hpp
		Strings.hpp
		Thread.hpp
		Time.hpp
		TypeTraits.hpp
		Wildcards.hpp

	SOURCES
		src/ABI.cpp
		src/Algorithm.cpp
		src/ArgumentParser.cpp
		src/Directory.cpp
		src/Exception.cpp
		src/Logger.cpp
		src/Path.cpp
		src/Thread.cpp

	CMAKE_CONFIG_FILE
		TialUtilityConfig.cmake.in

	CMAKE_SOURCES
		TialUtility.cmake
		TialDefaultConfig.cmake.in
		TialPostBuild.cmake
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.." ${Boost_INCLUDE_DIRS}
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_include_directories(${PROJECT_NAME} SYSTEM
		PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/compat"
	)

	add_tial_library(${PROJECT_NAME}StdCompat
		HEADERS
			compat/experimental/string_view

		SOURCES
			"src/compat/codecvt.cpp"
	)

	target_link_libraries(${PROJECT_NAME} Dbghelp ${PROJECT_NAME}StdCompat)
endif()

add_tial_test(Test${PROJECT_NAME}
	SOURCES
		tests/Algorithm.cpp
		tests/ArgumentParser.cpp
		tests/Directory.cpp
		tests/Exception.cpp
		tests/Language.cpp
		tests/Path.cpp
		tests/StreamOperator.cpp
		tests/Strings.cpp
		tests/Time.cpp
		tests/Wildcards.cpp
	LIBRARIES ${PROJECT_NAME}
)
add_custom_command(TARGET Test${PROJECT_NAME}
	COMMAND ${CMAKE_COMMAND} -E
		"copy_directory" "${CMAKE_CURRENT_SOURCE_DIR}/tests/sample" "${CMAKE_CURRENT_BINARY_DIR}/sample"
)

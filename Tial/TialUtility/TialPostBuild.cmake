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
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	include(BundleUtilities)
endif()

message(WARNING "Tial build step; TARGET=${TARGET}; BUNDLE=${BUNDLE}; OUTPUT_NAME=${OUTPUT_NAME}; RESOURCES=${RESOURCES}")
if(NOT EXISTS "${OUTPUT_NAME}")
	message(FATAL_ERROR "${OUTPUT_NAME} does not yet exist")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND BUNDLE)
	execute_process(COMMAND /usr/bin/otool -L "${OUTPUT_NAME}")
	get_dotapp_dir("${OUTPUT_NAME}" BUNDLE_DIR)
	fixup_bundle("${BUNDLE_DIR}" "" "")
	foreach(resource ${RESOURCES})
		file(COPY "${resource}"
			DESTINATION "${BUNDLE_DIR}/Contents/Resources"
		)
	endforeach()
endif()

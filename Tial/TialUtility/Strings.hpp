/* Copyright (c) 2015, Mariusz Plucinski
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <iomanip>
#include <sstream>
#include <type_traits>

namespace Tial {
namespace Utility {
namespace Strings {

template<typename String, typename Character = typename String::value_type>
String escapedBytes(
		const String &string, const Character &printableRangeBegin = 0x20, const Character &printableRangeEnd = 0x7E
) {
	std::basic_ostringstream<Character> oss;
	for(auto &element: string)
		if(element < printableRangeBegin || element > printableRangeEnd)
			oss << "\\x" << std::hex << static_cast<unsigned int>(
					static_cast<typename std::make_unsigned<Character>::type>(element)
				);
		else
			oss << static_cast<char>(element);
	return oss.str();
}

template<typename String, typename Character = typename String::value_type>
void escapeBytes(
		String &string, const Character &printableRangeBegin = 0x20, const Character &printableRangeEnd = 0x7E
) {
	string = escapedBytes(string, printableRangeBegin, printableRangeEnd);
}

template<typename Input>
void dumpHex(std::ostream &ostream, const Input &input) {
	for(auto &i: input)
		ostream << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(i);
}

}
}
}
